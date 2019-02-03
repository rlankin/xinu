/**
 * @file    printsegaddress.c
 * @author  Ryan Lankin
 */

#include <xinu.h>

void printsegaddress() {
	kprintf("Segment addresses:\r\n");

    kprintf("%10d bytes text\r\n", (ulong)&etext - (ulong)&text);
    kprintf("           Range: [0x%08x to 0x%08x]\r\n", (ulong)&text, (ulong)&etext - 1);
    kprintf("           Bytes: [%d, %d, %d, %d, ...]\r\n", text, *(&text + 1), *(&text + 2), *(&text + 3));

    kprintf("%10d bytes data\r\n", (ulong)&edata - (ulong)&data);
    kprintf("           Range: [0x%08x to 0x%08x]\r\n", (ulong)&data, (ulong)&edata - 1);
    kprintf("           Bytes: [%d, %d, %d, %d, ...]\r\n", data, *(&data + 1), *(&data + 2), *(&data + 3));

    kprintf("%10d bytes bss\r\n", (ulong)&ebss - (ulong)&bss);
    kprintf("           Range: [0x%08x to 0x%08x]\r\n", (ulong)&bss, (ulong)&ebss - 1);
    kprintf("           Bytes: [%d, %d, %d, %d, ...]\r\n", bss, *(&bss + 1), *(&bss + 2), *(&bss + 3));

    kprintf("\r\n");
}
