/**
 * @file    stackdepth.c
 * @author  Ryan Lankin
 */

#include <xinu.h>

void stackdump(unsigned long *sp, unsigned long *fp);

/*
 * Print information about the current call stack.
 *
 * This is designed specifically for ARM processors.
 */
int stackdepth() {
    unsigned long *sp, *fp;
    int depth = 0;

    asm("mov %0, %%sp":"=r"(sp)::);
    asm("mov %0, %%fp":"=r"(fp)::);
    kprintf("sp = %08X (%08X), fp = %08X (%08X)\r\n", sp, *sp, fp, *fp); // TODO: Debug
    struct thrent *thread = &thrtab[gettid()];
    kprintf("stkbase: 0x%08X\r\n", thread->stkbase);

    // stackdump(sp, fp);

    do {
        // TODO: Print depth, SP, and FP
        // kprintf("Depth: %d, SP: 0x%08x, FP: 0x%08x, Prev FP: 0x%08x\r\n", ++depth, 0, fp, *fp);
        // fp = *fp;
        kprintf("Depth: %d, SP: 0x%08x, FP: 0x%08x, Prev FP: 0x%08x\r\n", ++depth, 0, fp, fp[-1]);
        fp = fp[-1];
    } while (fp != 0);

    return depth;
}

/*
 * Print the contents of the stack surrounding fp.
 *
 * This is intended for debugging purposes.
 */
void stackdump(unsigned long *sp, unsigned long *fp) {
    int i;
    for (i = 20; i > -20; i--) {
        kprintf("fp[%3d] @ 0x%08X = 0x%08X", i, &(fp[i]), fp[i]);
        if (&(fp[i]) == fp) {
            kprintf(" <--- FP");
        }
        if (&(fp[i]) == sp) {
            kprintf(" <--- SP");
        }
        kprintf("\r\n");
    }
}

void sdtest1(void) {
    kprintf("In sdtest1, stack depth = %d\r\n", stackdepth());
    sdtest2();
}

void sdtest2(void) {
    kprintf("In sdtest2, stack depth = %d\r\n", stackdepth());
    sdtest3();
}

void sdtest3(void) {
    kprintf("In sdtest3, stack depth = %d\r\n\r\n", stackdepth());
}
