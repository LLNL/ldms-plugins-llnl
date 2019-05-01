/* -*- c-basic-offset: 8 -*- */
/* Copyright 2019 Lawrence Livermore National Security, LLC and other
 * ldms-plugins-llnl project developers.  See the top-level COPYRIGHT file
 * for details.
 *
 * SPDX-License-Identifier: (GPL-2.0-or-later OR BSD-3-Clause)
 */
#ifndef __LUSTRE_LLITE_GENERAL_H
#define __LUSTRE_LLITE_GENERAL_H

#include <ldms/ldms.h>
#include <ldms/ldmsd.h>

int llite_general_schema_is_initialized();
int llite_general_schema_init();
void llite_general_schema_fini();
ldms_set_t llite_general_create(const char *producer_name,
                                const char *fs_name,
                                const char *llite_name);
char *llite_general_osd_path_find(const char *search_path, const char *llite_name);
void llite_general_sample(const char *llite_name, const char *stats_path,
                          ldms_set_t general_metric_set);
void llite_general_destroy(ldms_set_t set);

#endif /* __LUSTRE_LLITE_GENERAL_H */
