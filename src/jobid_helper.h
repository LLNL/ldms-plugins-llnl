/* -*- c-basic-offset: 8 -*- */
/* Copyright 2020 Lawrence Livermore National Security, LLC and other
 * ldms-plugins-llnl project developers.  See the top-level COPYRIGHT file
 * for details.
 *
 * SPDX-License-Identifier: (GPL-2.0-or-later OR BSD-3-Clause)
 */
#ifndef __JOBID_HELPER_H
#define __JOBID_HELPER_H

#include <ldms/ldms.h>
#include <ldms/ldmsd.h>

int jobid_helper_schema_add(ldms_schema_t schema);
void jobid_helper_metric_update(ldms_set_t set);
void jobid_helper_config(struct attr_value_list *avl);

#endif /* __JOBID_HELPER_H */
