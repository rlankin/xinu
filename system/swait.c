/*
 * @file swait.c
 * @author Ryan Lankin
 */

#include <xinu.h>

// TODO: Make compatible with Huang's version?

syscall swait(
        sid32 semA,
        sid32 semB
        )
{
    irqmask mask = disable();
    struct thrent *threadEntry;
    struct sement *semAEntry;
    struct sement *semBEntry;
    struct sement *semWait;

    if (isbadsem(semA) || isbadsem(semB) || semA == semB) {
        restore(mask);
        return SYSERR;
    }

    semAEntry = &semtab[semA];
    semBEntry = &semtab[semB];

    if (semAEntry->state == SFREE || semBEntry->state == SFREE) {
        restore(mask);
        return SYSERR;
    }

    threadEntry = &thrtab[thrcurrent[cpuid]];

    // Wait for both semaphores to become available
    while (semAEntry->count <= 0 || semBEntry->count <= 0) {
        // If either semaphore has been deleted, exit with error
        if (semAEntry->state == SFREE || semBEntry->state == SFREE) {
            restore(mask);
            return SYSERR;
        }

        // Wait
        if (semAEntry->count <= 0) {
            semWait = semAEntry;
        } else {
            semWait = semBEntry;
        }

        (semWait->count)--;
        enqueue(thrcurrent[cpuid], semWait->queue);

        thrtab_acquire(thrcurrent[cpuid]);
        threadEntry->state = THRWAIT;
        threadEntry->sem = semWait;
		thrtab_release(thrcurrent[cpuid]);
        resched();

        (semWait->count)++;
    }

    wait(semA);
    wait(semB);

    restore(mask);

    return OK;
}
