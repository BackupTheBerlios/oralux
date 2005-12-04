#! /bin/sh
# ----------------------------------------------------------------------------
# listen-up.sh
# $Id: listen-up.sh,v 1.3 2005/12/04 22:42:27 gcasse Exp $
# $Author: gcasse $
# Description: Installing listen-up
# $Date: 2005/12/04 22:42:27 $ |
# $Revision: 1.3 $ |
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

# Warning:
# libxml2 is required and supposed to be already installed (e.g. because of php.sh)

####
# Installing the package in the current tree
InstallPackage()
{
cd /tmp

apt-get install libncurses5-dev

# osalp is required
wget http://switch.dl.sourceforge.net/sourceforge/osalp/osalp-0.7.3.tar.gz 
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
chroot $BUILD apt-get install libncurses5-dev

#
cd $BUILD/tmp

# osalp is required
wget http://switch.dl.sourceforge.net/sourceforge/osalp/osalp-0.7.3.tar.gz 
tar -zxvf osalp-0.7.3.tar.gz 
chroot $BUILD bash -c "cd /tmp/osalp-0.7.3;./configure;make;make install"

#
echo "NOTICE: password is please"
cvs -d:pserver:anonymous@linux-speakup.org:/usr/src/CVS login
cvs -d:pserver:anonymous@linux-speakup.org:/usr/src/CVS co listen-up
chroot $BUILD bash -c "cd /tmp/listen-up;make;make install"
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
