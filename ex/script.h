/*      $OpenBSD: script.h,v 1.4 2014/11/12 16:29:04 millert Exp $      */

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
 *      @(#)script.h    10.2 (Berkeley) 3/6/96
 */

struct _script {
        pid_t    sh_pid;                /* Shell pid.            */
        int      sh_master;             /* Master pty fd.        */
        int      sh_slave;              /* Slave pty fd.         */
        char    *sh_prompt;             /* Prompt.               */
        size_t   sh_prompt_len;         /* Prompt length.        */
        char     sh_name[64];           /* Pty name              */
        struct   winsize sh_win;        /* Window size.          */
        struct   termios sh_term;       /* Terminal information. */
};
