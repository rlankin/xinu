/**
 * @file ttyRead.c
 *
 * Modified by Ryan Lankin
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <ctype.h>
#include <device.h>
#include <tty.h>
#include <thread.h>

static void ttyEcho(device *, char);
void ttyPS(void);

/**
 * @ingroup tty
 *
 * Read characters from a tty.
 * @param devptr pointer to tty device
 * @param buf buffer for read characters
 * @param len size of the buffer
 * @return number of characters read, EOF if end of file was reached
 */
devcall ttyRead(device *devptr, void *buf, uint len)
{
    struct tty *ttyptr;
    device *phw;
    int ch = 0;
    int count = 0;
    char *buffer = buf;

    /* Setup and error check pointers to structures */
    ttyptr = &ttytab[devptr->minor];
    phw = ttyptr->phw;
    if (NULL == phw)
    {
        return SYSERR;
    }

    /* If the eof flag is set, clear the flag and return EOF */
    if (ttyptr->ieof)
    {
        ttyptr->ieof = FALSE;
        return EOF;
    }

    /* In raw mode, no line buffering and no cooking */
    if (ttyptr->iflags & TTY_IRAW)
    {
        /* Fill user buffer first from input buffer */
        while ((0 < ttyptr->icount) && (count < len))
        {
            *buffer++ = ttyptr->in[ttyptr->istart];
            ttyptr->icount--;
            ttyptr->istart = (ttyptr->istart + 1) % TTY_IBLEN;
            count++;
        }

        /* Fill rest of user buffer by reading input */
        while (count < len)
        {
            ch = (*phw->getc) (phw);
            if (SYSERR == ch)
            {
                return SYSERR;
            }
            *buffer++ = ch;
            count++;

            /* Echo character if TTY_ECHO flag is set */
            if (ttyptr->iflags & TTY_ECHO)
            {
                ttyEcho(devptr, ch);
            }
        }

        return count;
    }

    /* In non-raw mode, read characters into the TTY input buffer     */
    /* until a line delimiter is read or the TTY input buffer is full */
    while ((ttyptr->icount < TTY_IBLEN) && !ttyptr->idelim)
    {
        /* Read character */
        ch = (*phw->getc) (phw);
        if (SYSERR == ch)
        {
            return SYSERR;
        }

        /* Cook special characters */
        switch (ch)
        {
            /* Backspace or delete */
        case '\b':
        case 0x7F:
            if (ttyptr->icount < 1)
            {
                continue;
            }
            ttyptr->icount--;
            break;
            /* Newline */
        case '\n':
            /* Translate NL to CR if INLCR flag is set */
            if (ttyptr->iflags & TTY_INLCR)
            {
                ch = '\r';
            }
            /* Place character in TTY input buffer */
            ttyptr->in[(ttyptr->istart + ttyptr->icount) % TTY_IBLEN] =
                ch;
            ttyptr->icount++;
            ttyptr->idelim = TRUE;
            break;
            /* Carriage return */
        case '\r':
            /* Ignore carriage return if IGNCR flag is set */
            if (ttyptr->iflags & TTY_IGNCR)
            {
                continue;
            }
            /* Translate CR to NL if ICRNL flag is set */
            if (ttyptr->iflags & TTY_ICRNL)
            {
                ch = '\n';
            }
            /* Place character in TTY input buffer */
            ttyptr->in[(ttyptr->istart + ttyptr->icount) % TTY_IBLEN] =
                ch;
            ttyptr->icount++;
            ttyptr->idelim = TRUE;
            break;
            /* End of file */
        case 0x04:
            ttyptr->ieof = TRUE;
            ttyptr->idelim = TRUE;
            break;
        /* ctrl+t */
        case TY_PS:
            ttyPS();
            ttyptr->icount = 0;
            ttyptr->idelim = TRUE;
            break;
        /* All other characters */
        default:
            /* Ignore unprintable characters */
            if (!isprint(ch))
            {
                continue;
            }
            /* Place character in TTY input buffer */
            ttyptr->in[(ttyptr->istart + ttyptr->icount) % TTY_IBLEN] =
                ch;
            ttyptr->icount++;
            break;
        }

        /* Echo character if TTY_ECHO flag is set */
        if (ttyptr->iflags & TTY_ECHO)
        {
            ttyEcho(devptr, ch);
        }

        if (ttyptr->iflags & TTY_IRAW)
        {
            break;
        }
    }

    /* Fill user buffer from input buffer */
    while ((0 < ttyptr->icount) && (count < len))
    {
        *buffer++ = ttyptr->in[ttyptr->istart];
        ttyptr->icount--;
        ttyptr->istart = (ttyptr->istart + 1) % TTY_IBLEN;
        count++;
    }

    /* If TTY input buffer is empty, clear idelimiter flag */
    if (0 == ttyptr->icount)
    {
        ttyptr->idelim = FALSE;
    }

    /* If nothing was read, and the eof flag was set, return EOF */
    if ((0 == count) && (ttyptr->ieof))
    {
        ttyptr->ieof = FALSE;
        return EOF;
    }

    return count;
}

/**
 * Echo a single character on a TTY.
 * @param devptr TTY device table entry
 * @param ch character to echo
 */
static void ttyEcho(device *devptr, char ch)
{
    /* Backspace or delete */
    if (('\b' == ch) || (0x7F == ch))
    {
        ttyPutc(devptr, '\b');
        ttyPutc(devptr, ' ');
        ttyPutc(devptr, '\b');
        return;
    }

    /* Do not echo unprintable characters */
    if (!isprint(ch))
    {
        return;
    }

    /* Echo character */
    ttyPutc(devptr, ch);
}

/*
 * This is an exact copy of the xsh_ps() function that uses kprintf() instead of
 * printf() and has the parameter handling removed.
 *
 * NOTE: After this function runs, the shell prompt that the user is returned to
 * does not properly have its color handled. Normally, the user is returned to a
 * prompt that looks like:
 *
 * xsh$
 *
 * and is colored red. Instead, the kprintf calls cause the prompt to look like:
 *
 * [1;31mxsh$
 *
 * The prompt functions properly, but is not colored red, as can be seen by the
 * [1;31m not correctly being captured as caused to render red text. I believe
 * this may have something to do with the fact that kprintf() ultimately writes
 * directly to a serial device instead of calling the normal putc() function.
 *
 * This is fixed by replacing the calls to kprintf() in this function with
 * calls to printf().
 */
void ttyPS() {
    struct thrent *thrptr;
    int i;

    /* readable names for PR* status in thread.h */
    static const char * const pstnams[] = {
        "curr", "free", "ready", "recv",
        "sleep", "susp", "wait", "rtim"
    };

    // Print header
    kprintf("\r\n%3s %-16s %5s %4s %4s %10s %-10s %10s\r\n",
           "TID", "NAME", "STATE", "PRIO", "PPID", "STACK BASE",
           "STACK PTR", "STACK LEN");
    kprintf("%3s %-16s %5s %4s %4s %10s %-10s %10s\r\n",
           "---", "----------------", "-----", "----", "----",
           "----------", "----------", " ---------");

    /* Output information for each thread */
    for (i = 0; i < NTHREAD; i++) {
        thrptr = &thrtab[i];
        if (thrptr->state == THRFREE) {
            continue;
        }

        kprintf("%3d %-16s %5s %4d %4d 0x%08lX 0x%08lX %10lu\r\n",
               i,
               thrptr->name,
               pstnams[(int)thrptr->state - 1],
               thrptr->prio,
               thrptr->parent,
               (ulong)thrptr->stkbase,
               (ulong)thrptr->stkptr,
               thrptr->stklen);
    }
}
