#! /bin/sh
# ----------------------------------------------------------------------------
# listen-up.sh
# $Id: helisreal.sh,v 1.1 2004/09/27 20:30:00 gcasse Exp $
# $Author: gcasse $
# Description: Installing listen-up
# $Date: 2004/09/27 20:30:00 $ |
# $Revision: 1.1 $ |
# Copyright (C) 2003, 2004 Gilles Casse (gcasse@oralux.org)
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


mkdir HELIS
cd HELIX
wget https://helixcommunity.org/download.php/557/legacy-hxplay-1.0.0.298-linux-2.2-libc6-i386.bin
##https://helixcommunity.org/download.php/561/hxplay-1.0.0.298-linux-2.2-libc6-gcc32-i586.tar.bz2
##tar -jxvf ../hxplay-1.0.0.298-linux-2.2-libc6-gcc32-i586.tar.bz2 

chmod u+x legacy-hxplay-1.0.0.298-linux-2.2-libc6-i386.bin 
./legacy-hxplay-1.0.0.298-linux-2.2-libc6-i386.bin 


# RealPlayer10
cd achoisir
mkdir REALPLAYER
cd REALPLAYER/
wget https://helixcommunity.org/download.php/559/legacy-RealPlayer10GOLD.bin
chmod +x legacy-RealPlayer10GOLD.bin 
./legacy-RealPlayer10GOLD.bin 

# downloading the SDK (a form must be fullfilled: name, address,..)
http://forms.real.com/rnforms/resources/server/realsystemsdk/index.html 
tar -zxvf rsg2sdk_r4.tar.gz 
cp -pR rmasdk_6_0 /usr/local


# installing ralplayer 8 (for trplayer+sdk), downloaded from (+ RealPlayer 9 codec):
http://forms.real.com/real/player/blackjack.html

# realplayer8 asks for "a display" to be installed...
# installed under GUI...

# then under console:
cp -pR RealPlayer8 /usr/lib


et finalement
root # cd /home/gilles/.w3m/
root # !trpl
trplayer *smil
Failed to load rmacore.so.6.0: /usr/lib/RealPlayer8/Common/rmacore.so.6.0: undefined symbol: __pure_virtual

il manque _pure.o que j'ai viré précédemment.
option -E lors du link?









####
# Installing the package in the current tree
InstallPackage()
{
cd /tmp

# osalp is required
wget http://cesnet.dl.sourceforge.net/sourceforge/osalp/osalp-0.7.3.tar.gz 
tar -zxvf osalp-0.7.3.tar.gz 
cd osalp-0.7.3
./configure
make
make install

#
echo "password is please"
cvs -d:pserver:anonymous@linux-speakup.org:/usr/src/CVS login
cvs -d:pserver:anonymous@linux-speakup.org:/usr/src/CVS co listen-up
cd listen-up
make
make install
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
cd $BUILD/var/tmp

# # osalp is required
wget http://cesnet.dl.sourceforge.net/sourceforge/osalp/osalp-0.7.3.tar.gz 
tar -zxvf osalp-0.7.3.tar.gz 
chroot $BUILD bash -c "cd /var/tmp/osalp-0.7.3;./configure;make;make install"

#
echo "password is please"
cvs -d:pserver:anonymous@linux-speakup.org:/usr/src/CVS login
cvs -d:pserver:anonymous@linux-speakup.org:/usr/src/CVS co listen-up
chroot $BUILD bash -c "cd /var/tmp/listen-up;make;make install"
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
