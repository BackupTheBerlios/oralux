#!/bin/sh
# menu.lst is in /tmp

TARGET_MNT_POINT="/mnt/hdinstall"
mkdir -p $TARGET_MNT_POINT
mkdosfs /dev/fd0
mount /dev/fd0 $TARGET_MNT_POINT
grub-install --recheck --root-directory=$TARGET_MNT_POINT "(fd0)"
cp /tmp/menu.lst $TARGET_MNT_POINT/boot/grub/
umount $TARGET_MNT_POINT
rmdir $TARGET_MNT_POINT
