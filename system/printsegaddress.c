/**
 * @file    printsegaddress.c
 * @author  Ryan Lankin
 */

#include <xinu.h>

/*
 * Print information about the text, data, and bss memory segments.
 *
 * The values produced by this function may not line up to Xinu's start up
 * output exactly due to the fact that the segments are aligned on 64-byte
 * boundaries. If the alignments in ld.script are removed, the values line up
 * exactly.
 */
void printseginfo(char *name, int *start, int *end) {
    kprintf("%10d bytes %s\r\n", end - start, name);
    kprintf("           Range: [0x%08X to 0x%08X]\r\n", start, end - 1);
    kprintf("           Bytes: [0x%08X, 0x%08X, 0x%08X, 0x%08X, ...]\r\n", start[0], start[1], start[2], start[3]);
}

void printsegaddress() {
	kprintf("Segment addresses:\r\n");

    printseginfo("text", &text, &etext);
    printseginfo("data", &data, &edata);
    printseginfo("bss", &bss, &ebss);

    kprintf("\r\n");
}
