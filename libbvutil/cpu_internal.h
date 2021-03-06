/*
 * This file is part of BVBase.
 *
 * BVBase is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * BVBase is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with BVBase; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef BVUTIL_CPU_INTERNAL_H
#define BVUTIL_CPU_INTERNAL_H

#include "cpu.h"

#define CPUEXT_SUFFIX(flags, suffix, cpuext)                            \
    (BV_HAVE_ ## cpuext ## suffix && ((flags) & BV_CPU_FLAG_ ## cpuext))

#define CPUEXT(flags, cpuext) CPUEXT_SUFFIX(flags, , cpuext)

int bb_get_cpu_flags_aarch64(void);
int bb_get_cpu_flags_arm(void);
int bb_get_cpu_flags_ppc(void);
int bb_get_cpu_flags_x86(void);

#endif /* BVUTIL_CPU_INTERNAL_H */
