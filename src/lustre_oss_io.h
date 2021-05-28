/* -*- c-basic-offset: 8 -*- */
/* Copyright 2019 Lawrence Livermore National Security, LLC and other
 * ldms-plugins-llnl project developers.  See the top-level COPYRIGHT file
 * for details.
 *
 * SPDX-License-Identifier: (GPL-2.0-or-later OR BSD-3-Clause)
 */
#ifndef __LUSTRE_OST_H
#define __LUSTRE_OST_H

#include <ldms/ldms.h>
#include <ldms/ldmsd.h>

#define SAMP "lustre_oss_io"

extern ldmsd_msg_log_f log_fn;

#ifndef RBT_FOREACH
#define RBT_FOREACH(rbn, rbt) \
        for ((rbn) = rbt_min((rbt)); (rbn); (rbn) = rbn_succ((rbn)))
#endif

#endif /* __LUSTRE_OST_H */
