/* -*- c-basic-offset: 8 -*- */
/* Copyright 2019 Lawrence Livermore National Security, LLC and other
 * ldms-plugins-llnl project developers.  See the top-level COPYRIGHT file
 * for details.
 *
 * Copyright 2020 HPC Center at Shanghai Jiao Tong University.
 *
 * SPDX-License-Identifier: (GPL-2.0-or-later OR BSD-3-Clause)
 */

#include <limits.h>
#include <string.h>
#include <dirent.h>
#include <coll/rbt.h>
#include <sys/queue.h>
#include <unistd.h>
#include <ldms/ldms.h>
#include <ldms/ldmsd.h>
#include "config.h"
#include "lustre_ost_io.h"

#define _GNU_SOURCE

#define OSS_IO_PATH "/sys/kernel/debug/lustre/ost/OSS/ost_io"

static char *oss_io_stats_uint64_t_entries[] = {
	"req_waittime",
	"req_qdepth",
        "req_active",
        "req_timeout",
        "reqbuf_avail",
        "ost_read",
        "ost_write",
        "ost_punch",
        NULL
};

static ldms_schema_t oss_io_schema;

ldmsd_msg_log_f log_fn;
char producer_name[LDMS_PRODUCER_NAME_MAX];

static struct oss_io_data oss_io;

struct oss_io_data {
        char *name;
        char *path;
        char *stats_path;
        ldms_set_t oss_io_metric_set; /* a pointer */
};

static int string_comparator(void *a, const void *b)
{
        return strcmp((char *)a, (char *)b);
}

int oss_io_schema_is_initialized()
{
        if (oss_io_schema != NULL)
                return 0;
        else
                return -1;
}

void oss_io_schema_fini()
{
        log_fn(LDMSD_LDEBUG, SAMP" oss_io_schema_fini()\n");
        if (oss_io_schema != NULL) {
                ldms_schema_delete(oss_io_schema);
                oss_io_schema = NULL;
        }
}

int oss_io_schema_init(const char *producer_name)
{
        ldms_schema_t sch;
        int rc;
        int i;

        log_fn(LDMSD_LDEBUG, SAMP" oss_io_schema_init()\n");
        sch = ldms_schema_new("llnl_lustre_oss_io");
        if (sch == NULL)
                goto err1;
        rc = ldms_schema_meta_array_add(sch, "oss", LDMS_V_CHAR_ARRAY, 64);
        if (rc < 0)
                goto err2;
        /* add oss io stats entries */
        for (i = 0; oss_io_stats_uint64_t_entries[i] != NULL; i++) {
                rc = ldms_schema_metric_add(sch, oss_io_stats_uint64_t_entries[i],
                                            LDMS_V_U64);
                if (rc < 0)
                        goto err2;
        }

        oss_io_schema = sch;

        return 0;
err2:
        ldms_schema_delete(sch);
err1:
        log_fn(LDMSD_LERROR, SAMP" lustre_oss_io schema creation failed\n");
        return -1;
}

ldms_set_t oss_create(const char *producer_name, const char *oss_name)
{
        ldms_set_t set;
        int index;
        char instance_name[256];

        log_fn(LDMSD_LDEBUG, SAMP" oss_create()\n");
        snprintf(instance_name, sizeof(instance_name), "%s/io",
                 producer_name);
        set = ldms_set_new(instance_name, oss_io_schema);
        ldms_set_producer_name_set(set, producer_name);
        index = ldms_metric_by_name(set, "oss");
        ldms_metric_array_set_str(set, index, oss_name);
        ldms_set_publish(set);

        return set;
}

static struct oss_io_data *oss_io_create(const char *oss_name, const char *basedir)
{
        struct oss_io_data *oss_io;
        char path_tmp[PATH_MAX]; 
        char *state;

        log_fn(LDMSD_LDEBUG, SAMP" oss_io_create() %s from %s\n",
               oss_name, basedir);
        oss_io = calloc(1, sizeof(*oss_io));
        if (oss_io == NULL)
                goto out1;
        oss_io->name = strdup(oss_name);
        if (oss_io->name == NULL)
                goto out2;
        snprintf(path_tmp, PATH_MAX, "%s", basedir);
        oss_io->path = strdup(path_tmp);
        if (oss_io->path == NULL)
                goto out3;
        snprintf(path_tmp, PATH_MAX, "%s/stats", oss_io->path);
        oss_io->stats_path = strdup(path_tmp);
        if (oss_io->stats_path == NULL)
                goto out4;
        
        oss_io->oss_io_metric_set = oss_create(producer_name, oss_io->name);
        if (oss_io->oss_io_metric_set == NULL)
                goto out5;

        return oss_io;

out5:
        free(oss_io->stats_path);
out4:
        free(oss_io->path);
out3:
        free(oss_io->name);
out2:
        free(oss_io);
out1:
        return NULL;
}

static void oss_io_destroy()
{
        log_fn(LDMSD_LDEBUG, SAMP" oss_io_destroy() %s\n", oss_io->name);
        ldms_set_unpublish(oss_io->oss_io_metric_set);
        ldms_set_delete(oss_io->oss_io_metric_set);
        free(oss_io->stats_path);
        free(oss_io->path);
        free(oss_io->name);
        free(oss_io);
}

static void oss_io_stats_sample(const char *stats_path,
                                   ldms_set_t oss_io_metric_set)
{
        FILE *sf;
        char buf[512];
        char str1[64+1];

        sf = fopen(stats_path, "r");
        if (sf == NULL) {
                log_fn(LDMSD_LWARNING, SAMP" file %s not found\n",
                       stats_path);
                return;
        }

        /* The first line should always be "snapshot_time"
           we will ignore it because it always contains the time that we read
           from the file, not any information about when the stats last
           changed */
        if (fgets(buf, sizeof(buf), sf) == NULL) {
                log_fn(LDMSD_LWARNING, SAMP" failed on read from %s\n",
                       stats_path);
                goto out1;
        }
        if (strncmp("snapshot_time", buf, sizeof("snapshot_time")-1) != 0) {
                log_fn(LDMSD_LWARNING, SAMP" first line in %s is not \"snapshot_time\": %s\n",
                       stats_path, buf);
                goto out1;
        }

        ldms_transaction_begin(oss_io_metric_set);
        index = ldms_metric_by_name(oss_io_metric_set, "oss");
        ldms_metric_set_u64(oss_io_metric_set, index, producer_name);
        while (fgets(buf, sizeof(buf), sf)) {
                uint64_t val1, val2;
                int rc;
                int index;

                rc = sscanf(buf, "%64s %lu samples [%*[^]]] %*u %*u %lu %*u",
                            str1, &val1, &val2);
        
                if (rc == 3) {
                        index = ldms_metric_by_name(oss_io_metric_set, str1);
                        if (index == -1) {
                                log_fn(LDMSD_LWARNING, SAMP" oss io stats metric not found: %s\n",
                                       str1);
                        } else {
                                ldms_metric_set_u64(oss_io_metric_set, index, val2);
                        }
                        continue;
                }
        }
        ldms_transaction_end(oss_io_metric_set);
out1:
        fclose(sf);

        return;
}

static int config(struct ldmsd_plugin *self,
                  struct attr_value_list *kwl, struct attr_value_list *avl)
{
        log_fn(LDMSD_LDEBUG, SAMP" config() called\n");
        return 0;
}

static int sample(struct ldmsd_sampler *self)
{
        log_fn(LDMSD_LDEBUG, SAMP" sample() called\n");
        if (oss_io_schema_is_initialized() < 0) {
                if (oss_io_schema_init() < 0) {
                        log_fn(LDMSD_LERROR, SAMP" oss io schema create failed\n");
                        return ENOMEM;
                }
        }

        oss_io_stats_sample(oss_io->stats_path, oss_io->oss_io_metric_set);

        return 0;
}

static void term(struct ldmsd_plugin *self)
{
        log_fn(LDMSD_LDEBUG, SAMP" term() called\n");
        oss_io_destroy();
        oss_io_schema_fini();
}

static ldms_set_t get_set(struct ldmsd_sampler *self)
{
	return NULL;
}

static const char *usage(struct ldmsd_plugin *self)
{
        log_fn(LDMSD_LDEBUG, SAMP" usage() called\n");
	return  "config name=" SAMP;
}

static struct ldmsd_sampler oss_io_plugin = {
	.base = {
		.name = SAMP,
		.type = LDMSD_PLUGIN_SAMPLER,
		.term = term,
		.config = config,
		.usage = usage,
	},
	.get_set = get_set,
	.sample = sample,
};

struct ldmsd_plugin *get_plugin(ldmsd_msg_log_f pf)
{
        log_fn = pf;
        log_fn(LDMSD_LDEBUG, SAMP" get_plugin() called ("PACKAGE_STRING")\n");
        gethostname(producer_name, sizeof(producer_name));

        return &oss_io_plugin.base;
}
