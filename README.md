The plugins in ldms-plugins-llnl compile against LDMS v3.  They will likely
also compile against LDLMS v4 when that is released.

The llnl_lustre_client plugin is fully functional against LDMS v3.  The llnl_lustre_mdt
and llnl_lustre_ost plugins will require an LDMS v4 that fixes ldms_set_delete() (Likely
OVIS 4.3.0 or later).

License
----------------

ldms-plugins-llnl is distributed under the terms of both the GNU General Public
License (Version 2) and the BSD 3-Clause License. Users may choose either license,
at their option.

All new contributions must be made under both the GPL and BSD licenses.

See [LICENSE-GPL](https://github.com/llnl/ldms-plugins-llnl/blob/master/LICENSE-GPL),
[LICENSE-BSD](https://github.com/llnl/ldms-plugins-llnl/blob/master/LICENSE-BSD),
[COPYRIGHT](https://github.com/llnl/ldms-plugins-llnl/blob/master/COPYRIGHT), and
[NOTICE](https://github.com/llnl/ldms-plugins-llnl/blob/master/NOTICE) for details.

SPDX-License-Identifier: (GPL-2.0-or-later OR BSD-3-Clause)

LLNL-CODE-774582
