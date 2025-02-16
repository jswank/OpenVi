/*      $OpenBSD: mark.h,v 1.5 2016/05/27 09:18:11 martijn Exp $        */

/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * Copyright (c) 1992, 1993, 1994
 *      The Regents of the University of California.  All rights reserved.
 * Copyright (c) 1992, 1993, 1994, 1995, 1996
 *      Keith Bostic.  All rights reserved.
 * Copyright (c) 2022-2023 Jeffrey H. Johnson <trnsz@pobox.com>
 *
 * See the LICENSE.md file for redistribution information.
 *
 *      @(#)mark.h      10.3 (Berkeley) 3/6/96
 */

/*
 * The MARK and LMARK structures define positions in the file.  There are
 * two structures because the mark subroutines are the only places where
 * anything cares about something other than line and column.
 *
 * Because of the different interfaces used by the db(3) package, curses,
 * and users, the line number is 1 based and the column number is 0 based.
 * Additionally, it is known that the out-of-band line number is less than
 * any legal line number.  The line number is of type recno_t, as that's
 * the underlying type of the database.  The column number is of type size_t,
 * guaranteeing that we can malloc a line.
 */

struct _mark {
#define OOBLNO          0               /* Out-of-band line number. */
        recno_t  lno;                   /* Line number.             */
        size_t   cno;                   /* Column number.           */
};

struct _lmark {
        LIST_ENTRY(_lmark) q;           /* Linked list of marks. */
        recno_t  lno;                   /* Line number.          */
        size_t   cno;                   /* Column number.        */
        CHAR_T   name;                  /* Mark name.            */

#define MARK_DELETED    0x01            /* Mark was deleted.   */
#define MARK_USERSET    0x02            /* User set this mark. */
        u_int8_t flags;
};

#define ABSMARK1        '\''            /* Absolute mark name. */
#define ABSMARK2        '`'             /* Absolute mark name. */
