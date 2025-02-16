/*      $OpenBSD: cl.h,v 1.12 2021/09/02 11:19:02 schwarze Exp $        */

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
 *      @(#)cl.h        10.19 (Berkeley) 9/24/96
 */

extern  volatile sig_atomic_t cl_sigint;
extern  volatile sig_atomic_t cl_sigterm;
extern  volatile sig_atomic_t cl_sigwinch;

typedef struct _cl_private {
        CHAR_T   ibuf[512];     /* Input keys. */

        int      eof_count;     /* EOF count. */

        struct termios orig;    /* Original terminal values. */
        struct termios ex_enter;/* Terminal values to enter ex. */
        struct termios vi_enter;/* Terminal values to enter vi. */

        char    *el;            /* Clear to EOL terminal string. */
        char    *cup;           /* Cursor movement terminal string. */
        char    *cuu1;          /* Cursor up terminal string. */
        char    *rmso, *smso;   /* Inverse video terminal strings. */
        char    *smcup, *rmcup; /* Terminal start/stop strings. */

#define INDX_HUP        0
#define INDX_INT        1
#define INDX_TERM       2
#define INDX_WINCH      3
#define INDX_MAX        4       /* Original signal information. */
        struct sigaction oact[INDX_MAX];

        enum {                  /* Tty group write mode. */
            TGW_UNKNOWN=0, TGW_SET, TGW_UNSET } tgw;

        enum {                  /* Terminal initialization strings. */
            TE_SENT=0, TI_SENT } ti_te;

#define CL_IN_EX        0x0001  /* Currently running ex. */
#define CL_RENAME       0x0002  /* X11 xterm icon/window renamed. */
#define CL_RENAME_OK    0x0004  /* User wants the windows renamed. */
#define CL_SCR_EX_INIT  0x0008  /* Ex screen initialized. */
#define CL_SCR_VI_INIT  0x0010  /* Vi screen initialized. */
#define CL_STDIN_TTY    0x0020  /* Talking to a terminal. */
        u_int32_t flags;
} CL_PRIVATE;

#define CLP(sp)         ((CL_PRIVATE *)((sp)->gp->cl_private))
#define GCLP(gp)        ((CL_PRIVATE *)(gp)->cl_private)

/* Return possibilities from the keyboard read routine. */
typedef enum { INP_OK=0, INP_EOF, INP_ERR, INP_INTR, INP_TIMEOUT } input_t;

/* The screen line relative to a specific window. */
#define RLNO(sp, lno)   (sp)->woff + (lno)

/* X11 xterm escape sequence to rename the icon/window. */
#define XTERM_RENAME    "\033]0;%s\007"

#include "cl_extern.h"
