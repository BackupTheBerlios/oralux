#! /bin/sh
# ----------------------------------------------------------------------------
# kernel.sh
# $Id: kernel.sh,v 1.2 2006/08/05 20:28:30 gcasse Exp $
# $Author: gcasse $
# Description: Linux kernel patches.
# $Date: 2006/08/05 20:28:30 $ |
# $Revision: 1.2 $ |
# Copyright (C) 2003, 2004, 2005 Gilles Casse (gcasse@oralux.org)
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# ----------------------------------------------------------------------------
####
source ../oralux.conf
unset ARCH
unset RAMDSIK

# Kernel
export KERNEL_EXT=2.6.17
export KERNEL=linux-$KERNEL_EXT
export KERNEL_TARBALL=$KERNEL.tar.gz
export KERNEL_URL="http://www.fr.kernel.org/pub/linux/kernel/v2.6/$Tarball"

# Unionfs snapshot
export UNIONFS=unionfs-1.3
export UNIONFS_TARBALL=$UNIONFS.tar.gz
export UNIONFS_URL=ftp://ftp.fsl.cs.sunysb.edu/pub/unionfs/$UNIONFS

# Cloop
export CLOOP=cloop-2.04
export CLOOP_TARBALL=cloop_2.04-1.tar.gz
export CLOOP_URL=http://debian-knoppix.alioth.debian.org/sources/$CLOOP_TARBALL


cd $ARCHDIR

if [ ! -e $KERNEL_TARBALL ]
    then
    echo "Downloading $Kernel"
    wget $KERNEL_URL
fi

if [ ! -e $CLOOP_TARBALL ]
    then
    echo "Downloading $CLOOP_TARBALL"
    wget $CLOOP_URL
fi

if [ ! -e $UNIONFS_TARBALL ]
    then
    echo "Downloading $UNIONFS_TARBALL"
    wget $UNIONFS_URL
fi

####
# Installing the package in the current tree
InstallPackage()
{
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    # kernel: sources
    rm $BUILD/usr/src/linux
    rm -rf $BUILD/usr/src/$KERNEL
    cd $ARCHDIR
    tar -zxvf $KERNEL_TARBALL -C $BUILD/usr/src
    
    cd $BUILD/usr/src/$KERNEL

    # config et conf.vars (cloop)
    cp $INSTALL_PACKAGES/kernel/config .config
    cp $INSTALL_PACKAGES/kernel/conf.vars .

    # patch
    for i in `ls $INSTALL_PACKAGES/kernel/*.patch`; do patch -p1 < $i; done

    # kernel: links
    chroot $BUILD bash -c "\
cd /usr/src;\
ln -s $KERNEL linux;\
cd linux/include;\
ln -s asm-i386 asm;\
cd /usr/src/linux;\
make oldconfig;\
make;\
"
    # Unionfs
    cd $ARCHDIR
    tar -zxvf $UNIONFS_TARBALL -C $BUILD/usr/src/modules
    chroot $BUILD bash -c "\
apt-get install ctags uuid-dev;\
mkdir /usr/src/modules 2>/dev/null;\
cd /usr/src/modules;\
cd $UNIONFS;\
echo "EXTRACFLAGS=-DUNIONFS_NDEBUG" >> fistdev.mk; \
make; \
make install;\
"
    cp $BUILD/lib/modules/$KERNEL_EXT/kernel/fs/unionfs/unionfs.ko /root/oralux/factory/newOralux/KNOPPIX/modules

# Unionfs: fistdev.mk with
# EXTRACFLAGS=-DUNIONFS_NDEBUG


    # cloop
    tar -zxvf $CLOOP_TARBALL -C $BUILD/usr/src/modules
    chroot $BUILD bash -c "\
cd /usr/src/modules;\
cd $CLOOP;\
make;\
cp cloop.ko /lib/modules/*/kernel/drivers/block;\
"
    cp $BUILD/lib/modules/$KERNEL_EXT/kernel/drivers/block/cloop.ko /root/oralux/factory/newOralux/KNOPPIX/modules

    # kernel: build
    chroot $BUILD bash -c "\
cd /usr/src/linux;\
make all;\
cp arch/i386/boot/bzImage /boot/vmlinuz-$KERNEL_EXT
cp System.map /boot/System.map-$KERNEL_EXT
cp .config /boot/config-$KERNEL_EXT
"
}

case $1 in
    i|I)
        InstallPackage
        ;;
    b|B)
        Copy2Oralux
        ;;
    *)
        echo "I (install) or B(new tree) is expected"
        ;;
esac

exit 0
