/*      $OpenBSD: msg.h,v 1.3 2001/01/29 01:58:31 niklas Exp $  */

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
 *      @(#)msg.h       10.10 (Berkeley) 5/10/96
 */

/*
 * Common messages (continuation or confirmation).
 */

typedef enum {
        CMSG_CONF, CMSG_CONT, CMSG_CONT_EX,
        CMSG_CONT_R, CMSG_CONT_S, CMSG_CONT_Q } cmsg_t;

/*
 * Message types.
 *
 * !!!
 * In historical vi, O_VERBOSE didn't exist, and O_TERSE made the error
 * messages shorter.  In this implementation, O_TERSE has no effect and
 * O_VERBOSE results in informational displays about common errors, for
 * naive users.
 *
 * M_NONE       Display to the user, no reformatting, no nothing.
 *
 * M_BERR       Error: M_ERR if O_VERBOSE, else bell.
 * M_ERR        Error: Display in inverse video.
 * M_INFO        Info: Display in normal video.
 * M_SYSERR     Error: M_ERR, using strerror(3) message.
 * M_VINFO       Info: M_INFO if O_VERBOSE, else ignore.
 *
 * The underlying message display routines only need to know about M_NONE,
 * M_ERR and M_INFO -- all the other message types are converted into one
 * of them by the message routines.
 */

typedef enum {
        M_NONE = 1, M_BERR, M_ERR, M_INFO, M_SYSERR, M_VINFO } mtype_t;

/*
 * There are major problems with error messages being generated by routines
 * preparing the screen to display error messages.  It's possible for the
 * editor to generate messages before we have a screen in which to display
 * them, or during the transition between ex (and vi startup) and a true vi.
 * There's a queue in the global area to hold them.
 *
 * If SC_EX/SC_VI is set, that's the mode that the editor is in.  If the flag
 * S_SCREEN_READY is set, that means that the screen is prepared to display
 * messages.
 */

typedef struct _msgh MSGH;      /* MSGS list head structure. */
LIST_HEAD(_msgh, _msg);
struct _msg {
        LIST_ENTRY(_msg) q;     /* Linked list of messages. */
        mtype_t  mtype;         /* Message type: M_NONE, M_ERR, M_INFO. */
        char    *buf;           /* Message buffer. */
        size_t   len;           /* Message length. */
};

/* Flags to msgq_status(). */
#define MSTAT_SHOWLAST  0x01    /* Show the line number of the last line. */
#define MSTAT_TRUNCATE  0x02    /* Truncate the file name if it's too long. */
