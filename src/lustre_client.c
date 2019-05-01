/* -*- c-basic-offset: 8 -*- */
/* Copyright 2019 Lawrence Livermore National Security, LLC and other
 * ldms-plugins-llnl project developers.  See the top-level COPYRIGHT file
 * for details.
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
#include "lustre_client.h"
#include "lustre_client_general.h"

#define _GNU_SOURCE

#define LLITE_PATH "/proc/fs/lustre/llite"
#define OSD_SEARCH_PATH "/proc/fs/lustre"

ldmsd_msg_log_f log_fn;
char producer_name[LDMS_PRODUCER_NAME_MAX];

/* red-black tree root for llites */
static struct rbt llite_tree;

struct llite_data {
        char *fs_name;
        char *name;
        char *path;
        char *stats_path;
        ldms_set_t general_metric_set; /* a pointer */
        struct rbn llite_tree_node;
};

static int string_comparator(void *a, const void *b)
{
        return strcmp((char *)a, (char *)b);
}

static struct llite_data *llite_create(const char *llite_name, const char *basedir)
{
        struct llite_data *llite;
        char path_tmp[PATH_MAX]; /* TODO: move large stack allocation to heap */
        char *state;

        log_fn(LDMSD_LDEBUG, SAMP" llite_create() %s from %s\n",
               llite_name, basedir);
        llite = calloc(1, sizeof(*llite));
        if (llite == NULL)
                goto out1;
        llite->name = strdup(llite_name);
        if (llite->name == NULL)
                goto out2;
        snprintf(path_tmp, PATH_MAX, "%s/%s", basedir, llite_name);
        llite->path = strdup(path_tmp);
        if (llite->path == NULL)
                goto out3;
        snprintf(path_tmp, PATH_MAX, "%s/stats", llite->path);
        llite->stats_path = strdup(path_tmp);
        if (llite->stats_path == NULL)
                goto out4;
        llite->fs_name = strdup(llite_name);
        if (llite->fs_name == NULL)
                goto out5;
        if (strtok_r(llite->fs_name, "-", &state) == NULL) {
                log_fn(LDMSD_LWARNING, SAMP" unable to parse filesystem name from \"%s\"\n",
                       llite->fs_name);
                goto out6;
        }
        llite->general_metric_set = llite_general_create(producer_name,
                                                         llite->fs_name,
                                                         llite->name);
        if (llite->general_metric_set == NULL)
                goto out6;
        rbn_init(&llite->llite_tree_node, llite->name);

        return llite;
out6:
        free(llite->fs_name);
out5:
        free(llite->stats_path);
out4:
        free(llite->path);
out3:
        free(llite->name);
out2:
        free(llite);
out1:
        return NULL;
}

static void llite_destroy(struct llite_data *llite)
{
        log_fn(LDMSD_LDEBUG, SAMP" llite_destroy() %s\n", llite->name);
        llite_general_destroy(llite->general_metric_set);
        free(llite->fs_name);
        free(llite->stats_path);
        free(llite->path);
        free(llite->name);
        free(llite);
}

static void llites_destroy()
{
        struct rbn *rbn;
        struct llite_data *llite;

        while (!rbt_empty(&llite_tree)) {
                rbn = rbt_min(&llite_tree);
                llite = container_of(rbn, struct llite_data,
                                   llite_tree_node);
                rbt_del(&llite_tree, rbn);
                llite_destroy(llite);
        }
}

/* List subdirectories in LLITE_PATH to get list of
   LLITE names.  Create llite_data structures for any LLITES any that we
   have not seen, and delete any that we no longer see. */
static void llites_refresh()
{
        struct dirent *dirent;
        DIR *dir;
        struct rbt new_llite_tree;

        rbt_init(&new_llite_tree, string_comparator);

        /* Make sure we have llite_data objects in the new_llite_tree for
           each currently existing directory.  We can find the objects
           cached in the global llite_tree (in which case we move them
           from llite_tree to new_llite_tree), or they can be newly allocated
           here. */

        dir = opendir(LLITE_PATH);
        if (dir == NULL) {
                log_fn(LDMSD_LWARNING, SAMP" unable to open llite dir %s\n",
                       LLITE_PATH);
                return;
        }
        while ((dirent = readdir(dir)) != NULL) {
                struct rbn *rbn;
                struct llite_data *llite;

                if (dirent->d_type != DT_DIR ||
                    strcmp(dirent->d_name, ".") == 0 ||
                    strcmp(dirent->d_name, "..") == 0)
                        continue;
                rbn = rbt_find(&llite_tree, dirent->d_name);
                if (rbn) {
                        llite = container_of(rbn, struct llite_data,
                                           llite_tree_node);
                        rbt_del(&llite_tree, &llite->llite_tree_node);
                } else {
                        llite = llite_create(dirent->d_name, LLITE_PATH);
                }
                if (llite == NULL)
                        continue;
                rbt_ins(&new_llite_tree, &llite->llite_tree_node);
        }
        closedir(dir);

        /* destroy any llites remaining in the global llite_tree since we
           did not see their associated directories this time around */
        llites_destroy();

        /* copy the new_llite_tree into place over the global llite_tree */
        memcpy(&llite_tree, &new_llite_tree, sizeof(struct rbt));

        return;
}

static void llites_sample()
{
        struct rbn *rbn;

        /* walk tree of known LLITEs */
        RBT_FOREACH(rbn, &llite_tree) {
                struct llite_data *llite;
                llite = container_of(rbn, struct llite_data, llite_tree_node);
                llite_general_sample(llite->name, llite->stats_path,
                                   llite->general_metric_set);
        }
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
        if (llite_general_schema_is_initialized() < 0) {
                if (llite_general_schema_init() < 0) {
                        log_fn(LDMSD_LERROR, SAMP" general schema create failed\n");
                        return ENOMEM;
                }
        }

        llites_refresh();
        llites_sample();

        return 0;
}

static void term(struct ldmsd_plugin *self)
{
        log_fn(LDMSD_LDEBUG, SAMP" term() called\n");
        llites_destroy();
        llite_general_schema_fini();
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

static struct ldmsd_sampler llite_plugin = {
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
        log_fn(LDMSD_LDEBUG, SAMP" get_plugin() called\n");
        rbt_init(&llite_tree, string_comparator);
        gethostname(producer_name, sizeof(producer_name));

        return &llite_plugin.base;
}
