/*      $OpenBSD: v_status.c,v 1.6 2014/11/12 04:28:41 bentley Exp $    */

/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * Copyright (c) 1992, 1993, 1994
 *      The Regents of the University of California.  All rights reserved.
 * Copyright (c) 1992, 1993, 1994, 1995, 1996
 *      Keith Bostic.  All rights reserved.
 * Copyright (c) 2022-2023 Jeffrey H. Johnson <trnsz@pobox.com>
 *
 * See the LICENSE.md file for redistribution information.
 */

#include <sys/types.h>
#include <sys/queue.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <bitstring.h>
#include <limits.h>
#include <stdio.h>

#include "../common/common.h"
#include "vi.h"

/*
 * v_status -- ^G
 *      Show the file status.
 *
 * PUBLIC: int v_status(SCR *, VICMD *);
 */
int
v_status(SCR *sp, VICMD *vp)
{
        (void)msgq_status(sp, vp->m_start.lno, MSTAT_SHOWLAST);
        return (0);
}
