/*
 * @file swait.c
 * @author Ryan Lankin
 */

#include <xinu.h>
#include <thread.h>

// TODO: Make compatible with Huang's version?

syscall swait(
        sid32 semA,
        sid32 semB
        )
{
    struct thrent *threadEntry;
    struct sement *semAEntry;
    struct sement *semBEntry;
    struct sement *semWait;
    sid32 semWaitId;
    int cpuid;
    irqmask mask = disable();

    if (isbadsem(semA) || isbadsem(semB) || semA == semB) {
        restore(mask);
        return SYSERR;
    }

    semAEntry = &semtab[semA];
    semBEntry = &semtab[semB];

    cpuid = getcpuid();
    threadEntry = &thrtab[thrcurrent[cpuid]];

    // Wait for both semaphores to become available
    while (1) {
        semtab_acquire(semA);
        semtab_acquire(semB);

        // If either semaphore has been deleted, exit with error
        if (semAEntry->state == SFREE || semBEntry->state == SFREE) {
            semtab_release(semA);
            semtab_release(semB);
            restore(mask);
            return SYSERR;
        }

        // Stop waiting if both semaphores are available
        if (semAEntry->count <= 0 || semBEntry->count <= 0) {
            semtab_release(semA);
            semtab_release(semB);
            break;
        }

        // Wait
        if (semAEntry->count <= 0) {
            semWait = semAEntry;
            semWaitId = semA;
        } else {
            semWait = semBEntry;
            semWaitId = semB;
        }

        (semWait->count)--;
        enqueue(thrcurrent[cpuid], semWait->queue);

        thrtab_acquire(thrcurrent[cpuid]);
        threadEntry->state = THRWAIT;
        threadEntry->sem = semWaitId;
		thrtab_release(thrcurrent[cpuid]);

        semtab_release(semA);
        semtab_release(semB);
        resched();

        semtab_acquire(semWaitId);
        (semWait->count)++;
        semtab_release(semWaitId);
    }

    wait(semA);
    wait(semB);

    restore(mask);

    return OK;
}
