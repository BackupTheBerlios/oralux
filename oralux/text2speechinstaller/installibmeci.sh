#!/bin/bash

#set -v

LI="$1"
PA="$2"
IS="$3"

ID=/mnt/I$$
LD=/mnt/L$$
mkdir $ID 
mkdir $LD 

TM=/ramdisk/ibmeci
mkdir -p $TM

CO=/tmp/ibmeci.conf
rm -f $CO

bailout()
{
    umount $LD
    umount $ID
    rmdir $LD $ID
    exit 0
}

mount -t auto -o loop $IS $ID
[ "$?" != "0" ] && bailout
mount -p0 -t auto -o loop,encryption=AES256 $ID/$LI $LD<<EOT
$PA
EOT

[ "$?" != "0" ] && bailout

touch $CO

cp $LD/* $TM 2>/dev/null

bailout
