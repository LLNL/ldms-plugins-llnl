/* -*- c-basic-offset: 8 -*- */
/* Copyright 2019 Lawrence Livermore National Security, LLC and other
 * ldms-plugins-llnl project developers.  See the top-level COPYRIGHT file
 * for details.
 *
 * SPDX-License-Identifier: (GPL-2.0-or-later OR BSD-3-Clause)
 */
#ifndef __LUSTRE_OST_GENERAL_H
#define __LUSTRE_OST_GENERAL_H

#include <ldms/ldms.h>
#include <ldms/ldmsd.h>

int ost_general_schema_is_initialized();
int ost_general_schema_init();
void ost_general_schema_fini();
ldms_set_t ost_general_create(const char *producer_name, const char *fs_name,
                              const char *ost_name);
char *ost_general_osd_path_find(const char *search_path, const char *ost_name);
void ost_general_sample(const char *ost_name, const char *stats_path,
                        const char *osd_path, ldms_set_t general_metric_set);
void ost_general_destroy(ldms_set_t set);

#endif /* __LUSTRE_OST_GENERAL_H */
