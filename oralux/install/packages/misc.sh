#! /bin/sh
# ----------------------------------------------------------------------------
# misc.sh
# $Id: misc.sh,v 1.19 2006/02/11 22:43:41 gcasse Exp $
# $Author: gcasse $
# Description: Miscellaneous packages
# $Date: 2006/02/11 22:43:41 $ |
# $Revision: 1.19 $ |
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

export ARCH_DOSEMU_COMMAND=xmsswap.zip
export DOSEMU_URL="http://freedos.sourceforge.net/freecom/packages/082pl3/$ARCH_DOSEMU_COMMAND"

cd $ARCH

if [ ! -e $ARCH_DOSEMU_COMMAND ]
    then
    echo "Downloading $ARCH_DOSEMU_COMMAND"
    wget $DOSEMU_URL
fi


export LIST="alsa-utils angband-doc angband audacity aumix bsdgames catdoc \
aspell-de aspell-en aspell-es aspell-fr aspell-pt-br aspell-pt \
dosemu-freedos ecasound elinks erc frotz gobjc lynx \
manpages manpages-de manpages-es manpages-fr manpages-pt manpages-ru \
moc nano ne nethack-console nethack pdftohtml \
ppthtml podracer python qalc tcsh toolame \
vim vsound workbone wv xlhtml \
xsltproc xpdf-utils zsh"

####
# Installing the package in the current tree
InstallPackage()
{
    apt-get install $LIST


    # Bug#327153: dosemu: fails to start
    # http://lists.debian.org/debian-qa-packages/2005/10/msg00324.html     
    cd /tmp
    mkdir FreeCom 
    cd FreeCom 
    unzip $ARCH/$ARCH_DOSEMU_COMMAND
    export DOC_FREECOM="/usr/share/doc/FreeCom"
    install -d $DOC_FREECOM
    install -m 644 *.txt *.html $DOC_FREECOM

    export LIB_FREECOM="/usr/lib/FreeCom"
    install -d $LIB_FREECOM
    install -m 755 command.com $LIB_FREECOM
    
    export LIB_DOSEMU="/usr/lib/dosemu/commands"
    cd $LIB_DOSEMU; rm comcom.com; ln -s $LIB_FREECOM/command.com comcom.com


    apt-get install cdtool; cd /usr/bin; ln -s ./cdtool cdstart

#    apt-get install -t unstable aspell-ru
    
    apt-get install console-cyrillic

    # Lirc
    # Answers during the installation
    # Create nodes: yes
    # Select device: Cancel
apt-get install lirc
update-rc.d -f exim4 remove

  #    apt-get install sawfish
}

####
# Adding the package to the new Oralux tree
Copy2Oralux()
{
    chroot $BUILD bash -c "apt-get install $LIST"

    # Bug#327153: dosemu: fails to start
    # http://lists.debian.org/debian-qa-packages/2005/10/msg00324.html     
    cd /tmp
    mkdir FreeCom 
    cd FreeCom 
    unzip $ARCH/$ARCH_DOSEMU_COMMAND
    export DOC_FREECOM="/usr/share/doc/FreeCom"
    install -d $BUILD$DOC_FREECOM
    install -m 644 *.txt *.html $BUILD$DOC_FREECOM

    export LIB_FREECOM="/usr/lib/FreeCom"
    install -d $BUILD$LIB_FREECOM
    install -m 755 command.com $BUILD$LIB_FREECOM
    
    export LIB_DOSEMU="/usr/lib/dosemu/commands"
    chroot $BUILD bash -c "cd $LIB_DOSEMU; rm comcom.com; ln -s $LIB_FREECOM/command.com comcom.com"

#    chroot $BUILD bash -c "apt-get install -t unstable aspell-ru"

# default options: 
# - Russian keyboard, 
# - Right Alt key (default)
# - Two Windows keys (default)
# - font: Terminus slave normal (font name: french translation)
# - font size: 16
# - koi8r
# - The cyrillic mode is not started by default (console-data must be used to set up the console)
    chroot $BUILD apt-get install console-cyrillic

#    chroot $BUILD apt-get install sawfish

    # Lirc
    # Answers during the installation
    # Create nodes: yes
    # Select device: Cancel
    chroot $BUILD bash -c "apt-get install lirc; cd /etc/rc5.d; update-rc.d -f lirc remove"

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
