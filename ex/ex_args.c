/*      $OpenBSD: ex_args.c,v 1.12 2016/01/06 22:28:52 millert Exp $    */

/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * Copyright (c) 1991, 1993, 1994
 *      The Regents of the University of California.  All rights reserved.
 * Copyright (c) 1991, 1993, 1994, 1995, 1996
 *      Keith Bostic.  All rights reserved.
 * Copyright (c) 2022-2023 Jeffrey H. Johnson <trnsz@pobox.com>
 *
 * See the LICENSE.md file for redistribution information.
 */

#include <sys/types.h>
#include <sys/queue.h>
#include <sys/time.h>

#include <bitstring.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <bsd_stdlib.h>
#include <bsd_string.h>

#include "../common/common.h"
#include "../vi/vi.h"

static int ex_N_next(SCR *, EXCMD *);

/*
 * ex_next -- :next [+cmd] [files]
 *      Edit the next file, optionally setting the list of files.
 *
 * !!!
 * The :next command behaved differently from the :rewind command in
 * historic vi.  See nvi/docs/autowrite for details, but the basic
 * idea was that it ignored the force flag if the autowrite flag was
 * set.  This implementation handles them all identically.
 *
 * PUBLIC: int ex_next(SCR *, EXCMD *);
 */
int
ex_next(SCR *sp, EXCMD *cmdp)
{
        ARGS **argv;
        FREF *frp;
        int noargs;
        char **ap;

        /* Check for file to move to. */
        if (cmdp->argc == 0 && (sp->cargv == NULL || sp->cargv[1] == NULL)) {
                msgq(sp, M_ERR, "No more files to edit");
                return (1);
        }

        if (F_ISSET(cmdp, E_NEWSCREEN)) {
                /* By default, edit the next file in the old argument list. */
                if (cmdp->argc == 0) {
                        if (argv_exp0(sp,
                            cmdp, sp->cargv[1], strlen(sp->cargv[1])))
                                return (1);
                        return (ex_edit(sp, cmdp));
                }
                return (ex_N_next(sp, cmdp));
        }

        /* Check modification. */
        if (file_m1(sp,
            FL_ISSET(cmdp->iflags, E_C_FORCE), FS_ALL | FS_POSSIBLE))
                return (1);

        /* Any arguments are a replacement file list. */
        if (cmdp->argc) {
                /* Free the current list. */
                if (!F_ISSET(sp, SC_ARGNOFREE) && sp->argv != NULL) {
                        for (ap = sp->argv; *ap != NULL; ++ap)
                                free(*ap);
                        free(sp->argv);
                }
                F_CLR(sp, SC_ARGNOFREE | SC_ARGRECOVER);
                sp->cargv = NULL;

                /* Create a new list. */
                CALLOC_RET(sp,
                    sp->argv, cmdp->argc + 1, sizeof(char *));
                for (ap = sp->argv,
                    argv = cmdp->argv; argv[0]->len != 0; ++ap, ++argv)
                        if ((*ap =
                            v_strdup(sp, argv[0]->bp, argv[0]->len)) == NULL)
                                return (1);
                *ap = NULL;

                /* Switch to the first file. */
                sp->cargv = sp->argv;
                if ((frp = file_add(sp, *sp->cargv)) == NULL)
                        return (1);
                noargs = 0;

                /* Display a file count with the welcome message. */
                F_SET(sp, SC_STATUS_CNT);
        } else {
                if ((frp = file_add(sp, sp->cargv[1])) == NULL)
                        return (1);
                if (F_ISSET(sp, SC_ARGRECOVER))
                        F_SET(frp, FR_RECOVER);
                noargs = 1;
        }

        if (file_init(sp, frp, NULL, FS_SETALT |
            (FL_ISSET(cmdp->iflags, E_C_FORCE) ? FS_FORCE : 0)))
                return (1);
        if (noargs)
                ++sp->cargv;

        F_SET(sp, SC_FSWITCH);
        return (0);
}

/*
 * ex_N_next --
 *      New screen version of ex_next.
 */
static int
ex_N_next(SCR *sp, EXCMD *cmdp)
{
        SCR *new;
        FREF *frp;

        /* Get a new screen. */
        if (screen_init(sp->gp, sp, &new))
                return (1);
        if (vs_split(sp, new, 0)) {
                (void)screen_end(new);
                return (1);
        }

        /* Get a backing file. */
        if ((frp = file_add(new, cmdp->argv[0]->bp)) == NULL ||
            file_init(new, frp, NULL,
            (FL_ISSET(cmdp->iflags, E_C_FORCE) ? FS_FORCE : 0))) {
                (void)vs_discard(new, NULL);
                (void)screen_end(new);
                return (1);
        }

        /* The arguments are a replacement file list. */
        new->cargv = new->argv = ex_buildargv(sp, cmdp, NULL);

        /* Display a file count with the welcome message. */
        F_SET(new, SC_STATUS_CNT);

        /* Set up the switch. */
        sp->nextdisp = new;
        F_SET(sp, SC_SSWITCH);

        return (0);
}

/*
 * ex_prev -- :prev
 *      Edit the previous file.
 *
 * PUBLIC: int ex_prev(SCR *, EXCMD *);
 */
int
ex_prev(SCR *sp, EXCMD *cmdp)
{
        FREF *frp;

        if (sp->cargv == sp->argv) {
                msgq(sp, M_ERR, "No previous files to edit");
                return (1);
        }

        if (F_ISSET(cmdp, E_NEWSCREEN)) {
                if (argv_exp0(sp, cmdp, sp->cargv[-1], strlen(sp->cargv[-1])))
                        return (1);
                return (ex_edit(sp, cmdp));
        }

        if (file_m1(sp,
            FL_ISSET(cmdp->iflags, E_C_FORCE), FS_ALL | FS_POSSIBLE))
                return (1);

        if ((frp = file_add(sp, sp->cargv[-1])) == NULL)
                return (1);

        if (file_init(sp, frp, NULL, FS_SETALT |
            (FL_ISSET(cmdp->iflags, E_C_FORCE) ? FS_FORCE : 0)))
                return (1);
        --sp->cargv;

        F_SET(sp, SC_FSWITCH);
        return (0);
}

/*
 * ex_rew -- :rew
 *      Re-edit the list of files.
 *
 * !!!
 * Historic practice was that all files would start editing at the beginning
 * of the file.  We don't get this right because we may have multiple screens
 * and we can't clear the FR_CURSORSET bit for a single screen.  I don't see
 * anyone noticing, but if they do, we'll have to put information into the SCR
 * structure so we can keep track of it.
 *
 * PUBLIC: int ex_rew(SCR *, EXCMD *);
 */
int
ex_rew(SCR *sp, EXCMD *cmdp)
{
        FREF *frp;

        /*
         * !!!
         * Historic practice -- you can rewind to the current file.
         */
        if (sp->argv == NULL) {
                msgq(sp, M_ERR, "No previous files to rewind");
                return (1);
        }

        if (file_m1(sp,
            FL_ISSET(cmdp->iflags, E_C_FORCE), FS_ALL | FS_POSSIBLE))
                return (1);

        /* Switch to the first one. */
        sp->cargv = sp->argv;
        if ((frp = file_add(sp, *sp->cargv)) == NULL)
                return (1);
        if (file_init(sp, frp, NULL, FS_SETALT |
            (FL_ISSET(cmdp->iflags, E_C_FORCE) ? FS_FORCE : 0)))
                return (1);

        /* Switch and display a file count with the welcome message. */
        F_SET(sp, SC_FSWITCH | SC_STATUS_CNT);

        return (0);
}

/*
 * ex_args -- :args
 *      Display the list of files.
 *
 * PUBLIC: int ex_args(SCR *, EXCMD *);
 */
int
ex_args(SCR *sp, EXCMD *cmdp)
{
        int cnt, col, len, sep;
        char **ap;

        if (sp->argv == NULL) {
                (void)msgq(sp, M_ERR, "No file list to display");
                return (0);
        }

        col = len = sep = 0;
        for (cnt = 1, ap = sp->argv; *ap != NULL; ++ap) {
                col += len = strlen(*ap) + sep + (ap == sp->cargv ? 2 : 0);
                if (col >= sp->cols - 1) {
                        col = len;
                        sep = 0;
                        (void)ex_puts(sp, "\n");
                } else if (cnt != 1) {
                        sep = 1;
                        (void)ex_puts(sp, " ");
                }
                ++cnt;

                (void)ex_printf(sp, "%s%s%s", ap == sp->cargv ? "[" : "",
                    *ap, ap == sp->cargv ? "]" : "");
                if (INTERRUPTED(sp))
                        break;
        }
        (void)ex_puts(sp, "\n");
        return (0);
}

/*
 * ex_buildargv --
 *      Build a new file argument list.
 *
 * PUBLIC: char **ex_buildargv(SCR *, EXCMD *, char *);
 */
char **
ex_buildargv(SCR *sp, EXCMD *cmdp, char *name)
{
        ARGS **argv;
        int argc;
        char **ap, **s_argv;

        argc = cmdp == NULL ? 1 : cmdp->argc;
        CALLOC(sp, s_argv, argc + 1, sizeof(char *));
        if ((ap = s_argv) == NULL)
                return (NULL);

        if (cmdp == NULL) {
                if ((*ap = v_strdup(sp, name, strlen(name))) == NULL) {
                        free(s_argv);
                        return (NULL);
                }
                ++ap;
        } else
                for (argv = cmdp->argv; argv[0]->len != 0; ++ap, ++argv)
                        if ((*ap =
                            v_strdup(sp, argv[0]->bp, argv[0]->len)) == NULL) {
                                while (--ap >= s_argv)
                                        free(*ap);
                                free(s_argv);
                                return (NULL);
                        }
        *ap = NULL;
        return (s_argv);
}
