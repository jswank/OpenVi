/*      $OpenBSD: args.h,v 1.5 2016/05/27 09:18:11 martijn Exp $        */

/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * Copyright (c) 1993, 1994
 *      The Regents of the University of California.  All rights reserved.
 * Copyright (c) 1993, 1994, 1995, 1996
 *      Keith Bostic.  All rights reserved.
 * Copyright (c) 2022-2023 Jeffrey H. Johnson <trnsz@pobox.com>
 *
 * See the LICENSE.md file for redistribution information.
 *
 *      @(#)args.h      10.2 (Berkeley) 3/6/96
 */

/*
 * Structure for building "argc/argv" vector of arguments.
 *
 * !!!
 * All arguments are NULL terminated as well as having an associated length.
 * The argument vector is NOT necessarily NULL terminated.  The proper way
 * to check the number of arguments is to use the argc value in the EXCMDARG
 * structure or to walk the array until an ARGS structure with a length of 0
 * is found.
 */
typedef struct _args {
        CHAR_T  *bp;            /* Argument. */
        size_t   blen;          /* Buffer length. */
        size_t   len;           /* Argument length. */

#define A_ALLOCATED     0x01    /* If allocated space. */
        u_int8_t flags;
} ARGS;
