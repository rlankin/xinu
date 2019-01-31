/**
 * @file    printsegaddress.c
 * @author  Ryan Lankin
 */

#include <xinu.h>

void printsegaddress() {
	kprintf("Segment addresses:\r\n");

    kprintf("%10d bytes text\r\n", (ulong)&_data - (ulong)&_start);
    kprintf("           [0x%08x to 0x%08x]\r\n", (ulong)&_start, (ulong)&_data - 1);
    // TODO: Print first 4 bytes

    kprintf("%10d bytes data\r\n", (ulong)&_bss - (ulong)&_data);
    kprintf("           [0x%08x to 0x%08x]\r\n", (ulong)&_data, (ulong)&_bss - 1);
    // TODO: Print first 4 bytes

    kprintf("%10d bytes bss\r\n", (ulong)&_end - (ulong)&_bss);
    kprintf("           [0x%08x to 0x%08x]\r\n", (ulong)&_bss, (ulong)&_end - 1);
    // TODO: Print first 4 bytes

    kprintf("\r\n");
}
