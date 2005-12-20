#! /bin/sh
# ----------------------------------------------------------------------------
# jabber.sh
# $Id: jabber.sh,v 1.1 2005/12/20 22:00:45 gcasse Exp $
# $Author: gcasse $
# Description: Installing jabber
# $Date: 2005/12/20 22:00:45 $ |
# $Revision: 1.1 $ |
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

export EMACS_JABBER=emacs-jabber_0.6.1_all.deb
export DIR=/usr/share/emacs21/site-lisp/emacs-jabber
cd $ARCH

wget http://ovh.dl.sourceforge.net/sourceforge/emacs-jabber/$EMACS_JABBER

####
# Installing the package in the current tree
InstallPackage()
{
    # jabber server
    apt-get install jabber jabber-aim jabber-irc jabber-jit jabber-jud jabber-msn jabber-muc jabber-yahoo adduser-plugin-notifyjabber

    # command line client
    apt-get install imcom

    # emacs client
    apt-get install flim
    dpkg -i $ARCH/$EMACS_JABBER
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    # jabber server
    chroot $BUILD apt-get install jabber jabber-aim jabber-irc jabber-jit jabber-jud jabber-msn jabber-muc jabber-yahoo adduser-plugin-notifyjabber

    # command line client
    chroot $BUILD apt-get install imcom

    chroot $BUILD apt-get install flim

    # emacs client
    cp $ARCH/$EMACS_JABBER $BUILD/tmp
    chroot $BUILD dpkg -i /tmp/$EMACS_JABBER
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
