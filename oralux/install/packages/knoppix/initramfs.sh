#!/bin/sh
# # from http://www.knoppix.net/forum/viewtopic.php?t=22271
# # HOWTO: Converting your minirt.gz from initRD to initRAMFS

#[ $(id -u) != "0" ] && exec sudo "$0" "$@"

#set -vx
[ $(id -u) != "0" ] && su


dest=initRAMFS

if [ ! -e $dest ];
then
    src=minirt.d
    mkdir $src
    mkdir $dest

    gunzip minirt.gz
    mount -o loop minirt $src
    cp -Rp $src/* $dest/
    cd $dest
    cat > init <<EOF
#!/static/ash            
/linuxrc
exec /etc/init "$@" </dev/console >/dev/console 2>&1
EOF
    chmod +x init
    cd ..
    umount $src
    rmdir $src
fi

cd $dest
find . | cpio --quiet -o -H newc | gzip -9 > ../minirt.gz
cp ../minirt.gz /root/oralux/factory/newOralux/boot/isolinux




# cd miniroot-initramfs                                          
                                                      
# touch init                                                     
                                                      
# chmod +x init                                                  
                                                      
# vi init                                                        


# gzip -dc /boot/initrd.img-xxx > /tmp/initrd.img
# cd /tmp
# mkdir initrd
# cd initrd
# cpio -i -H newc -F /tmp/initrd.img
