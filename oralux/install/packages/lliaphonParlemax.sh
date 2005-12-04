#! /bin/sh
# ----------------------------------------------------------------------------
# lliaphonParlemax.sh
# $Id: lliaphonParlemax.sh,v 1.3 2005/12/04 22:42:27 gcasse Exp $
# $Author: gcasse $
# Description: Installing LLiaPhon and ParleMax
# $Date: 2005/12/04 22:42:27 $ |
# $Revision: 1.3 $ |
# Copyright (C) 2004, 2005 Nath (nath.ml at free.fr)
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

# Version of ParleMax, and its url:
export VPM=0.1
export URLPM="http://philsfree.free.fr/bigdown/parlemax-$VPM.tgz"
export TARBALLPM="parlemax-$VPM.tgz"

####
# Installing the package in the current tree
InstallPackage()
{
# Installing LLiaPhon
cd /tmp
wget http://nathx.free.fr/biglux/lliaphon-0.3.2.tar.gz
tar xvzf lliaphon-0.3.2.tar.gz
cd lliaphon
make
make ressource
make install
echo "export LLIAPHON=/usr/local/lliaphon" >>/etc/profile
rm -rf /tmp/lliaphon*

# Installing ParleMax (I copy the sources in /usr/local/src since it does not exist an make install command)
cd /usr/local/src
rm -rf ParleMax
mkdir ParleMax
cd ParleMax
wget $URLPM
tar xvzf $TARBALLPM
cd ParleMax-$VPM
make
cd /usr/local/bin
rm -f parlemax maxlect
ln -s /usr/local/src/ParleMax/ParleMax-$VPM/parlemax parlemax
ln -s /usr/local/src/ParleMax/ParleMax-$VPM/maxlect maxlect

# Initializing the PM_FR_VOICE environment variable to indicate where the mbrola voice is located 
echo "export PM_FR_VOICE=/usr/local/share/mbrola/voices/fr3" >>/etc/profile

rm -rf /usr/local/src/ParleMax/TARBALLPM
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
# Installing LLiaPhon
cd $BUILD/tmp
wget http://nathx.free.fr/biglux/lliaphon-0.3.2.tar.gz
tar xvzf lliaphon-0.3.2.tar.gz
chroot $BUILD bash -c "cd /tmp/lliaphon; make; make ressource; make install"

echo "export LLIAPHON=/usr/local/lliaphon" >>$BUILD/etc/profile
rm -rf $BUILD/tmp/lliaphon*

# Installing ParleMax (I copy the sources in /usr/local/src since it does not exist a make install command)
cd $BUILD/usr/local/src
rm -rf ParleMax
mkdir ParleMax
cd ParleMax
wget $URLPM
tar xvzf $TARBALLPM
chroot $BUILD bash -c "cd /usr/local/src/ParleMax/ParleMax-$VPM; make; cd /usr/local/bin; rm -f parlemax maxlect; ln -s /usr/local/src/ParleMax/ParleMax-$VPM/parlemax parlemax; ln -s /usr/local/src/ParleMax/ParleMax-$VPM/maxlect maxlect"

# Initializing the PM_FR_VOICE environment variable to indicate where the mbrola voice is located 
echo "export PM_FR_VOICE=/usr/local/share/mbrola/voices/fr3" >>$BUILD/etc/profile

rm -rf $BUILD/usr/local/src/ParleMax/TARBALLPM
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
