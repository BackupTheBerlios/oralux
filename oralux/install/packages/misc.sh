#! /bin/sh
# ----------------------------------------------------------------------------
# misc.sh
# $Id: misc.sh,v 1.13 2006/01/21 14:51:58 gcasse Exp $
# $Author: gcasse $
# Description: Miscellaneous packages
# $Date: 2006/01/21 14:51:58 $ |
# $Revision: 1.13 $ |
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

export LIST="alsa-utils angband-doc angband audacity aumix bsdgames catdoc \
aspell-de aspell-en aspell-es aspell-fr aspell-pt-br \
ecasound elinks erc gobjc lynx \
manpages manpages-de manpages-es manpages-fr manpages-pt manpages-ru \
nano ne nethack-console nethack pdftohtml \
pdmenu ppthtml python qalc tcsh toolame \
vim vsound workbone wv xlhtml \
xsltproc xpdf-utils zsh"

####
# Installing the package in the current tree
InstallPackage()
{
    apt-get install $LIST
    apt-get install cdtool; cd /usr/bin; ln -s ./cdtool cdstart

#    apt-get install -t unstable aspell-ru
    
    apt-get install console-cyrillic

    # Lirc
    # Answers during the installation
    # Create nodes: yes
    # Select device: Cancel
apt-get install lirc

  #    apt-get install sawfish
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    chroot $BUILD bash -c "apt-get install $LIST"
    chroot $BUILD bash -c "apt-get install cdtool; cd /usr/bin; ln -s ./cdtool cdstart"

#    chroot $BUILD bash -c "apt-get install -t unstable aspell-ru"

# default options: 
# - Russian keyboard, 
# - Scroll lock (switch latin/cyrillic), 
# - font: Terminus slave normal (font name: french translation)
# - font size: 16
# - The cyrillic mode is not started by default (console-data must be used to set up the console)
    chroot $BUILD apt-get install console-cyrillic

#    chroot $BUILD apt-get install sawfish

    # Lirc
    # Answers during the installation
    # Create nodes: yes
    # Select device: Cancel
    chroot $BUILD apt-get install lirc

    # Copyright
    mkdir $BUILD/usr/share/doc/fdimage
    cp -pR $INSTALL_PACKAGES/fdimage/fdimage.c $BUILD/usr/share/doc/fdimage/copyright
#    cp -pR $DOC/license/festival $BUILD/usr/share/doc/    
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
