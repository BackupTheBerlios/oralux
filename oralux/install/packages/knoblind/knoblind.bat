set cmdline=
@echo off
REM KNOBLIND - by Osvaldo La Rossa - http://www.brlspeak.net -
REM A BLINdfriendly KNOppix bootDisk builder for the KNOPPIX GNU/LINUX
REM distribution - 
REM First release: 04-12-2002 - Public 
REM Updated 07-01-2003: post-rawrite bug correction + keyboard parameter added
REM Updated 29-01-2003: knoblind.bat autolocation (runned from CD / from HD)
REM                     + W9x versus NT/W2K/XP or DOS autodiagnose
REM                     + some not relevant lay-out details
REM 02 Aug 2003, Gilles Casse (gcasse@oralux.org) 
REM Adding for Oralux, the cheatcodes "myconfig=scan home=scan"
REM so that the persistent home and the saved configuration are searched at boot time.
REM 28 Feb 2004: Adding a few comments in syslinux.cfg about the expected braille codes.

cls
echo This script is FREE software.
echo It may be copied and redistributed under the conditions of the GNU 
echo GENERAL PUBLIC LICENSE Version 2, see http://www.gnu.org/ 
pause > nul:

cls
echo KNOBLIND - by Osvaldo La Rossa - http://www.brlspeak.net/
echo A BLINdfriendly KNOppix bootDisk builder for the KNOPPIX GNU/LINUX
echo distribution - http://www.knopper.net/knoppix
echo -------------------------------------------------------------------------------
echo.
echo This preconfiger was especially realized to automize the boot sequence,
echo providing it with the apropriate braille support for blind users.
echo To do this, we will use both the CD + a special (knoblind) bootdisk:
echo by running this BATch, you will be able to build a disk with the right
echo Braille support for your specific display.
echo After that, you no longer need to enter any parameters at the "boot:" prompt:
echo just keep both the CD + bootdisk in their devices,
echo and press ENTER, or wait until the system boots-up.
echo.
echo The default KNOBLIND config will now be set to no-scroll mode (required for
echo hardware braille displays), it will also be set to normal VGA, lang fr,
echo keyboard be-latin1, and BrlTty support for ALVA, at serial port 1, with
echo German table.
echo You may modify this by hand before rebooting (see the APPEND line in the
echo a:\syslinux.cfg file).
echo.
echo If your configuration is correct, then just wait ... until you see the
echo "root@tty1[/]#" prompt.
pause > nul:

:bagain
cls
if not exist c:\windows\command\choice.com goto isitdos
goto menu
:isitdos
if not exist c:\dos\choice.com goto nt2kxp
:menu
choice /c:CEQ "C)reate a bootdisk  E)edit current a:\syslinux.cfg  Q)uit:  "/n
if errorlevel 3 goto quit
if errorlevel 2 goto current
if errorlevel 1 goto new

:new
cls
if exist \knoppix\knoblind.bat goto cdrun
choice /c:DEFGHQ "Where is your KNOPPIX CD?  D  E  F  G  H  q)uit:  "/n
if errorlevel 6 goto quit
if errorlevel 5 goto dh
if errorlevel 4 goto dg
if errorlevel 3 goto df
if errorlevel 2 goto de
if errorlevel 1 goto dd

:cdrun
echo.
echo Place a blank disk in drive A:, then press a key to proceed ...
pause
call callrw
set cmdline=
goto ok

:nt2kxp
echo Warning: you are probably running a different system than Win9x ...
echo Please insert a blank diskette in drive A:, then press a key:
echo the boot image will be directly written to A: without prompting for
echo a CHOICE-menu, since choice.com no longer exist on NT/W2K/XP ...
echo You're also supposed running this BATch from the CD, not from HD; if not,
echo do CTRL+C, else press a key to proceed ...
pause
cls
call callrw
set cmdline=
goto ok

:dd
cls
d:
cd \knoppix
call callrw
goto ok
:de
cls
e:
cd \knoppix
call callrw
goto ok
:df
cls
f:
cd \knoppix
call callrw
goto ok
:dg
cls
g:
cd \knoppix
call callrw
goto ok
:dh
cls
h:
cd \knoppix
call callrw
goto ok

:ok
set cmdline=
echo.
echo Purifying disk ...
ren a:\logo.* *.txt
echo Graphical logo renamed to *.txt ...
echo.
ren a:\syslinux.* *.ori
echo syslinux.cfg renamed to *.ori ...
echo.
echo Building new syslinux.cfg with integrated braille support ...
echo # Update the line beginning with APPEND according to your braille display:
echo 
echo # 1. Codes for the braille displays supported by BRLTTY
echo # Alva: al, BrailleLite: bl, BrailleNote: bn, CombiBraille: cb, EcoBraille: ec,
echo # EuroBraille: eu, HandyTech: ht, LogText: lt, MultiBraille: mb, Say_MDV: md,
echo # MiniBraille: mn, Papenmeier: pm, Say_TSI: ts, Vario: va, VideoBraille: vd,
echo # VarioHT: vh, Voyager: vo, VisioBraille: vs,
echo 
echo # 2. Codes for the braille tables supplied by BRLTTY
echo # American: text.us.tbl, NABCC: text.nabcc.tbl, British: text.uk.tbl,
echo # Dansk: text.da.tbl, German: text.de.tbl,
echo # French: text.fr.tbl, French CBIFS: text.fr-cbifs.tbl, Italian: text.it.tbl,
echo # Norwegian and German: text.no-h.tbl, Norwegian: text.no-p.tbl, 
echo # Polish: text.pl.tbl, Russian: text.ru.tbl, Spanish: text.es.tbl
echo # Swedish: text.se1.tbl, Swedish2: text.se2.tbl, Vietnamese: text.vni.tbl
echo
echo DEFAULT vmlinuz >a:\syslinux.cfg
echo APPEND no-scroll brltty=al,/dev/ttyS0,text.us.tbl myconfig=scan lang=us keyboard=us home=scan ramdisk_size=100000 init=/etc/init noapic apm=power-off hda=scsi hdb=scsi hdc=scsi hdd=scsi hde=scsi hdf=scsi vga=normal initrd=miniroot.gz quiet BOOT_IMAGE=knoppix >>a:\syslinux.cfg
echo TIMEOUT 30 >>a:\syslinux.cfg
echo. >>a:\syslinux.cfg
echo PROMPT 1 >>a:\syslinux.cfg
echo DISPLAY boot.msg >>a:\syslinux.cfg
echo F1 boot.msg >>a:\syslinux.cfg
echo F2 f2 >>a:\syslinux.cfg
echo Edit now a:\syslinux.cfg for your personal display:
echo see brltty=al,/dev/ttyS0,text.german.tbl at the end of the APPEND line ...
pause
if exist c:\dos\edit.* goto current
if not exist c:\windows\command\choice.com goto noedsyslx
:current
set cmdline=
edit a:\syslinux.cfg
goto noxp
:noedsyslx
echo (If you are running this BATch under NT/W2K/XP's pseudo-DOS box, you must
echo edit a:\syslinux.cfg by hand to select the appropriate BrlTty driver for
echo your specific config ...)
echo.
:noxp
if exist a:\logo.txt goto ready
cls
echo WARNING! disk error: can't locate a:\logo.txt:
echo this file is used to control the bootdisk integrity, please try to build
echo the KnoBlind disk again ...
echo KNOWN BUG / KNOWN SOLUTION:
echo if the problem occurs once again, try to reboot the pc under DOS/DOSbox,
echo then run immediately the knoblind BATch.
echo.
echo Press a key to try again, or use CTRL+C to abort now ...
pause
goto bagain
:ready
echo Done!
echo.
echo ThanX for using KNOBLIND! You may now restart the machine ...
echo Report problems to: info@brlspeak.net with Subject: KnoBlind Disk
goto done

:quit
cls
echo This script is FREE software.
echo It may be copied and redistributed under the conditions of the GNU 
echo GENERAL PUBLIC LICENSE Version 2, see http://www.gnu.org/ 
echo.
echo Report problems to: info@brlspeak.net with Subject: KnoBlind Disk
set cmdline=

:done
set cmdline=
