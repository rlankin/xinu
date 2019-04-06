/*
 * @file ssignal.c
 * @author Ryan Lankin
 */

#include <xinu.h>

syscall ssignal(
        sid32   semA,
        sid32   semB
        )
{
    extern int resdefer;
    struct sement *semAEntry;
    struct sement *semBEntry;
    irqmask mask = disable();

    if (isbadsem(semA) || isbadsem(semB) || semA == semB) {
        restore(mask);
        return SYSERR;
    }

    semAEntry = &semtab[semA];
    semBEntry = &semtab[semB];

    semtab_acquire(semA);
    semtab_acquire(semB);

    // Ensure semaphores are valid
    if (semAEntry->state == SFREE || semBEntry->state == SFREE) {
        semtab_release(semA);
        semtab_release(semB);
        restore(mask);
        return SYSERR;
    }

    // Defer rescheduling, signal both semaphores, then reschedule
    resdefer = 1;
    signal(semA);
    signal(semB);
    if (--resdefer > 0) {
        resdefer = 0;
        semtab_release(semA);
        semtab_release(semB);
        resched();
    }

    semtab_release(semA);
    semtab_release(semB);

    restore(mask);

    return OK;
}
