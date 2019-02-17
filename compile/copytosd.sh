#!/bin/bash

SD=/run/media/rlankin/XINU

rm $SD/kernel.img
cp xinu.boot $SD/kernel.img

udisks --unmount /dev/mmcblk0p1
#udisks --detach /dev/mmcblk0p1
