#! /bin/sh

# mkmyknoppix
#
# (c) 3/2003 by Thomas Lange, lange@informatik.uni-koeln.de
# Universitaet zu Koeln
#
# This is free software; see the GNU General Public License version 2
# or later for copying conditions.  There is NO warranty.
# this script creates a new knoppix image with your local software
# You have to adjust the variable rmpacks if you want to remove more packages
# This script and the list for rmpacks is tested with knoppix version 3.2
#
# 4+/2003 (G.Casse): Customization for Oralux
#
source oralux.conf

####
#extractedKnoppix is the original iso coming from KNOPPIX/KNOPPIX
extractedKnoppix=$ISO/extractedKnoppix.iso # 2 GB, one big file

####
#knoppixCDiso is the original Knoppix CD iso
knoppixCDiso=$ISO/knoppixCD.iso
oraluxCDiso=$ISO/oraluxCD.iso

trap "umount_dirs" EXIT

####
add_soft() {
    cd $INSTALL_PACKAGES
    next_script general.sh "General"
    next_script tcl.sh "Tcl"
    next_script flite.sh "flite"
    next_script eflite.sh "eflite"
    next_script emacs.sh "emacs"
    next_script emacspeak.sh "emacspeak"
    next_script emacspeak-ss.sh "emacspeak-ss"
    next_script ses.sh "ses"
    next_script php.sh "php"
    next_script listen-up.sh "listen-up"
    next_script mail.sh "mail"
    next_script misc.sh "misc"
    next_script mbrola.sh "mbrola"
    next_script efm.sh "efm"
    next_script lliaphonParlemax.sh "lliaphonParlemax"
    next_script multispeech.sh "multispeech"
    next_script links.sh "links"
    next_script net.sh "net"
    next_script menu.sh "menu"
    next_script yasr.sh "yasr"
    next_script doc.sh "doc"

# The following script must be the last one
    next_script end.sh "end"
    cd ..
}

####
knoppixCD2iso() {

# Create an iso file ($knoppixCDiso) from the original CD
mount /dev/cdrom
cd $CDROM

if [ ! -f $CDROM/KNOPPIX/KNOPPIX ]; then 
  echo "I can't find KNOPPIX/KNOPPIX in $CDROM. Is it really the directory where the CDROM is mounted?"
  exit 0
fi

if [ ! -f KNOPPIX/boot.img ]; then 
  echo "I can't find KNOPPIX/boot.img in $CDROM."
  exit 0
fi

if [ ! -f KNOPPIX/boot.cat ]; then 
  echo "I can't find KNOPPIX/boot.cat in $CDROM."
  exit 0
fi

rm -f $knoppixCDiso 2>/dev/null
mkisofs -l -r -J -V "Oralux" -hide-rr-moved -v -b KNOPPIX/boot.img -c KNOPPIX/boot.cat -o $knoppixCDiso .

}

####
knoppixCDiso2iso() {

# extract the iso from the knoppix image
    set -e   # very important!

    [ -f $knoppixCDiso ] || {
	echo "$knoppixCDiso not found\n. Copy your original Knoppix iso file in the expected directory ($ISO) or create the iso from the cdrom."
	exit 99
    }

    mount -r -o loop $knoppixCDiso $MNT_ORALUX
    mkdir -p $NEW_ORALUX
    rm -f $extractedKnoppix 2>/dev/null
    tar --exclude KNOPPIX/KNOPPIX -C $MNT_ORALUX -cf - KNOPPIX | tar -C $NEW_ORALUX -xvf -
    cp $MNT_ORALUX/KNOPPIX/KNOPPIX $extractedKnoppix.old
    umount $MNT_ORALUX
    extract_compressed_fs $extractedKnoppix.old > $extractedKnoppix
}

####
knoppixiso2dir() {

    cd $BUILD
    echo "First, Do you want to remove or update the files under $PWD"
    read a
    if [ "$a" == "y" ] ; then
        echo "Removing files"
	rm -rf *
    fi

# create a directory tree containing the files of the knoppix image
    echo "mount -r -o loop $extractedKnoppix $MNT_ORALUX"

    mount -r -o loop $extractedKnoppix $MNT_ORALUX
    cd $BUILD
    echo "Creating directory $BUILD"
    tar -C $MNT_ORALUX -cf - . | tar -C $BUILD -xf -
    umount $MNT_ORALUX
}

####
rm_extractedKnoppix() {

    rm -f $extractedKnoppix
}

####
rm_tmp() {
    rm -rf $BUILD/var/tmp/*
    echo "$BUILD/var/tmp is cleaned"
}



####
remove_unused_packages() {

if [ -f $BUILD/etc/resolv.conf ]; then
    cp -L $BUILD/etc/resolv.conf $BUILD/etc/resolv.conf.sav
    rm $BUILD/etc/resolv.conf
fi

cp -L /etc/resolv.conf $BUILD/etc/resolv.conf
echo "if this script exits, just run it once again and skip this stage" 

chroot $BUILD apt-get update

# list of packages, that can be removed in $BUILD
# about 1.1 Gbytes will be removed

    for i in de fr da es it tr ru pl ja nl cs ; do
	kde="$l kde-i18n-$i"
    done
    rmpacks="$kde openoffice-de-en glibc-doc ttf-openoffice abiword-gtk lyx trans-de-en libgimp1.2 kdoc kdelibs4-dev kdelibs4 kdebase-data kdelibs-data gnome-games-locale libopenh323-1.9.10 libqt3-dev libqt3-mt libqt3 mozilla-browser"

    chroot $BUILD apt-get -y --force-yes --purge --ignore-hold remove $rmpacks
    games=`chroot $BUILD dpkg -S games/|grep -v , |egrep -v bsdmainutils | awk '{print $1}'|sort|uniq |sed -e 's/://'`
    chroot $BUILD apt-get -y --purge remove $games

    cd $ORALUX/install
    liste=`cat list_remove.txt|egrep -v "#"`;chroot $BUILD bash -c "dpkg -P `echo $liste`"
    liste=`cat remove_graphicstmp.txt|egrep -v "#"`;chroot $BUILD bash -c "dpkg -P `echo $liste`"
    liste=`cat list_top200.txt|egrep -v "#"`;chroot $BUILD bash -c "dpkg -P `echo $liste`"
    liste=`cat list_remove_2.txt|egrep -v "#"`;chroot $BUILD bash -c "dpkg -P `echo $liste`"

    chroot $BUILD bash -c "COLUMNS=200 dpkg -l | grep ^rc | awk '{print $2} ' | xargs dpkg -P"
    chroot $BUILD bash -c "deborphan | xargs dpkg -P"
    chroot $BUILD apt-get clean

    mv $BUILD/etc/resolv.conf.sav $BUILD/etc/resolv.conf
}

####
update_index() {

    chroot $BUILD bash -c "depmod -a"
    chroot $BUILD bash -c "dpkg --configure -a"
    chroot $BUILD bash -c "updatedb; update-dlocatedb"
}

####
next_step() {
# wait for input to start next step

    task=$1
    shift
    echo -n "Next step $task: $@: "
    read inp
    if [ "$inp" != "y" ] ; then
	echo "Skipping task $task"
	return
    fi
    $task
}

####
next_script() {

    task=$1
    shift
    echo -n "Next step $task: "
    read inp
    if [ "$inp" != "y" ] ; then
	echo "Skipping task $task"
	return
    fi
    chmod +x $task 
    ./$task b
}

####
create_new_iso() {

set +e

if [ ! -f $NEW_ORALUX/KNOPPIX/boot.img ]; then 
  echo "I can't find $NEW_ORALUX/KNOPPIX/boot.img."
  exit 0
fi

if [ ! -f $NEW_ORALUX/KNOPPIX/boot.cat ]; then 
  echo "I can't find $NEW_ORALUX/KNOPPIX/boot.cat."
  exit 0
fi

rm -f $oraluxCDiso
rm -f $NEW_ORALUX/KNOPPIX/KNOPPIX
mkisofs -L -R -l -V "Oralux ISO9660" -v -allow-multidot $BUILD | create_compressed_fs - 65536 > $NEW_ORALUX/KNOPPIX/KNOPPIX

#Computing the md5sums
cd $NEW_ORALUX

[ -f KNOPPIX/md5sums ] && rm KNOPPIX/md5sums
find . | grep -v boot.cat | grep -v md5sums | sed "s#./##" | sort | xargs md5sum 2>/dev/null >> KNOPPIX/md5sums

cd $NEW_ORALUX
mkisofs -l -r -J -V "Oralux" -hide-rr-moved -v -b KNOPPIX/boot.img -c KNOPPIX/boot.cat -o $oraluxCDiso $NEW_ORALUX
}

####
umount_dirs() {

    umount $MNT_ORALUX || true
    [ -f $BUILD/proc/cpuinfo ] && umount $BUILD/proc
    exit 0
}

####
cleartmp() {

    rm -rf $extractedKnoppix $NEW_ORALUX
}

####
cd_info() {

    cdrecord -scanbus
}

####
knoppixonhd() {

    knx-hdinstall
}

####
cd_erase() {

    if [ "$CDBUS" = "0" ] ; then
	echo "Please update the CDBUS variable in oralux.conf!"
	return
    fi
    cdrecord blank=fast dev=$CDBUS
}

####
cd_burn() {

    if [ "$CDBUS" = "0" ] ; then
	echo "Please update the CDBUS variable in oralux.conf!"
	return
    fi
    cdrecord -v -eject speed=8 -pad dev=$CDBUS $oraluxCDiso
}

###
# Start
#
echo "Return skips a task. To execute the task type 'y' and return."

umount $BUILD 2>/dev/null

# Check that the expected directory are present 
[ -d $ARCH ] || mkdir $ARCH
[ -d $FACTORY ] || mkdir $FACTORY
[ -d $MNT_ORALUX ] || mkdir $MNT_ORALUX
[ -d $BUILD ] || mkdir $BUILD
[ -d $NEW_ORALUX ] || mkdir $NEW_ORALUX
[ -d $ISO ] || mkdir $ISO

echo "mount $DEV_BUILD $BUILD"
mount $DEV_BUILD $BUILD

chmod +x $INSTALL_PACKAGES/*.sh

set -e
next_step cleartmp "Clear the $NEW_ORALUX directory and temporary files ?"
next_step knoppixCD2iso "Make a KNOPPIX CD ISO image from the KNOPPIX CDROM?"
next_step knoppixCDiso2iso "Extract and Make the ISO from the KNOPPIX CD ISO image?"
next_step knoppixiso2dir "Make a directory tree from the uncompressed ISO?"
next_step rm_extractedKnoppix "Remove $extractedKnoppix?"
mkdir -p $BUILD/proc 2>/dev/null
mount -t proc /proc $BUILD/proc
next_step remove_unused_packages "Remove unused packages?"
next_step add_soft "Add softwares?"
umount $BUILD/proc

next_step rm_tmp "Remove $BUILD/var/tmp/"

next_step update_index "Updating modules dependencies and indexes?"
next_step create_new_iso "Creating ISO image for new KNOPPIX?"
next_step cd_info "Scan bus about your CD recorder?"
next_step cd_erase "Erase CD?"
next_step cd_burn "Burn CD?"

echo "You can save disk space by removeing big directories:"
echo "rm -rf $NEW_ORALUX"

exit 0
