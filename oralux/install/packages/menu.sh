#! /bin/sh
# ----------------------------------------------------------------------------
# menu.sh
# $Id: menu.sh,v 1.10 2006/01/28 23:09:21 gcasse Exp $
# $Author: gcasse $
# Description: Installing the audio menu
# $Date: 2006/01/28 23:09:21 $ |
# $Revision: 1.10 $ |
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

####
# Installing in the current tree
InstallPackage()
{
   cd /usr/bin; 
   ln -s /usr/share/oralux/dialog/dialog.php dialog-oralux
   install -m 755 /usr/share/oralux/dialog/oralux-dialog.sh /usr/bin
   install -m 755 /usr/share/oralux/main/oralux-yasr.sh /usr/bin
   apt-get install pdmenu
}

####
# Adding the information to the new Oralux tree
Copy2Oralux()
{
   # WARNING
   # We can't embedded any DECtalk file in the CD.
   # As getvoice is built from dtk (dtk/ttsapi.h)
   # we have to delete the related objects
   cd $MAIN
   make oralux-target
   make clean.getvoice 2>/dev/null
   rm -f *.o

   # Installing the oralux runtime
   rm -rf $BUILD/usr/share/oralux

   cd $ORALUX/..
   tar --dereference --exclude factory --exclude arch --exclude CVS --exclude "*~" -cf - oralux | tar -C $BUILD/usr/share -xf -
   chroot $BUILD bash -c "cd /usr/bin; rm -f oralux.sh; ln -s /usr/share/oralux/main/oralux.sh ."

   # Installing the text based dialog
   chroot $BUILD bash -c "cd /usr/bin; ln -s /usr/share/oralux/dialog/dialog.php dialog-oralux;install -m 755 /usr/share/oralux/dialog/oralux-dialog.sh /usr/bin;install -m 755 /usr/share/oralux/main/oralux-yasr.sh /usr/bin"

   chroot $BUILD apt-get install pdmenu
   install -m 444 $INSTALL_PACKAGES/menu/pdmenurc $BUILD/etc
}

# debianizeBase()
# {
#     RELEASE=$1
#     PACKAGE_NAME=$2
#     DIR_ORIG=$3

#     PACKAGE_VERSION="${PACKAGE_NAME}-$ORALUX_VERSION.$RELEASE"

#     DIR=$DEBIANDIR/$PACKAGE_NAME

#     [ -n "$DIR" ] && [ -n "$PACKAGE_VERSION" ] && (rm -r "$DIR"/"$PACKAGE_VERSION";rm "$DIR"/"$PACKAGE_NAME"*.orig.tar.gz)

#     mkdir -p $DIR/$PACKAGE_VERSION

# #    cd $ORALUX
#     tar -C $ORALUX/$DIR_ORIG --exclude CVS --exclude "*~" -cf - . | tar -C $DIR/$PACKAGE_VERSION -xvf -
#     cd $DIR
#     tar -zcf $PACKAGE_VERSION.tar.gz $PACKAGE_VERSION
#     cd $PACKAGE_VERSION
#     dh_make -e $MAINTMAIL -f ../$PACKAGE_VERSION.tar.gz
# }

# debianizeAudio()
# {
#     RELEASE=$1
#     TYPE=$2

#     PACKAGE_NAME="oralux-audio-$TYPE"
#     PACKAGE_VERSION="${PACKAGE_NAME}-$ORALUX_VERSION.$RELEASE"

#     DIR=$DEBIANDIR/$PACKAGE_NAME

#     [ -n "$DIR" ] && [ -n "$PACKAGE_VERSION" ] && (rm -r "$DIR"/"$PACKAGE_VERSION";rm "$DIR"/"$PACKAGE_NAME"*.orig.tar.gz)

#     mkdir -p $DIR/${PACKAGE_VERSION}

#     tar -C $ORALUX/audio/$TYPE --exclude CVS --exclude "*~" -cf - . | tar -C $DIR/${PACKAGE_VERSION} -xvf -

#     cd $DIR
#     tar -zcf $PACKAGE_VERSION.tar.gz $PACKAGE_VERSION
#     cd $PACKAGE_VERSION
#     dh_make -e $MAINTMAIL -f ../$PACKAGE_VERSION.tar.gz
# }


# debianizeBase $MAIN_RELEASE "oralux-main" "main"
# exit 0

# debianizeAudio $AUDIO_RELEASE_de "de" 
# debianizeAudio $AUDIO_RELEASE_us "en"
# debianizeAudio $AUDIO_RELEASE_es "es" 
# debianizeAudio $AUDIO_RELEASE_fr "fr" 
# debianizeAudio $AUDIO_RELEASE_ru "ru" 
# debianizeAudio $AUDIO_RELEASE_theme "theme"

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
