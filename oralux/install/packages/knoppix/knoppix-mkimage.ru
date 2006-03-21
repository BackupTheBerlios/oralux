#=====================
#Knoppix_ru1
NTFSTITLE="Осторожно, NTFS"

NTFSWARNING1="В силу того, что изготовитель операционной системы Windows (ТМ) не предоставляет полной технической спецификации на файловую систему NTFS/XP, эта файловая система поддерживается операционной системой Linux лишь частично. Однако существует возможность записи содержимого файлов, которая является полезной для создания \"виртуальных дисков\" в разделе NTFS.

ЭТО НОВАЯ И ПОКА ЕЩЕ ЭКСПЕРИМЕНТАЛЬНАЯ функция. НЕТ НИКАКОЙ ГАРАНТИИ, ЧТО она БУДЕТ РАБОТАТЬ.
Вы все еще хотите продолжить?"

NTFSWARNING2="Предупреждение (2): Поведение NTFS/Windows (ТМ) может быть непредсказуемым, если Linux не был корректно выгружен после завершения работы в тех случаях, когда файловая система NTFS монтирована в режиме записи, или если возникают ошибки записи на диск. В этих случаях Windows запустит \"SCANDISK\" или \"CHKDSK\", чтобы проверить целостность разделов файловой системы . Обычно в ходе такой проверки никакие ошибки не обнаруживаются, но иногда может происходить потеря данных.

Вы все еще хотите продолжить?"

NTFSWARNING3="ПРЕДУПРЕЖДЕНИЕ (3): Убедитесь, что Вы сделали на внешнем носителе информации резервную копию всех важных данных из того раздела, который собираетесь использовать для образа KNOPPIX. Действительно ли Вы способны, при неблагоприятном стечении обстоятельств, самостоятельно восстановить разделы NTFS (или Windows)?"

TITLE1="Создание постоянного домашнего каталога KNOPPIX"

MESSAGE1="Этот скрипт создает образ виртуального диска для \"knoppix\" на Вашем жестком диске или на других съемных носителях (например, флэш-памяти или Zip-накопителях). Это позволит сохранять личные данные и файлы конфигурации при перезагрузке системы.

Вы хотите создать постоянный домашний каталог или архив системных файлов конфигурации для пользователя \"knoppix\"?"

MESSAGE2="Выберите раздел диска, на котором будет создан образ:"

MESSAGE3="В этом разделе уже существует образ Knoppix. Хотите удалить или переформатировать его?"

MESSAGE4="Это раздел файловой системы NTFS. Поскольку непосредственная запись в этой файловой системе для Linux не поддерживается, Вы должны из-под Windows(ТМ) создать пустой файл образа . Вы можете найти программу для этого в каталоге KNOPPIX на Вашем компакт-диске. ЭТО НОВАЯ И ПОКА ЕЩЕ ЭКСПЕРИМЕНТАЛЬНАЯ функция. НЕТ НИКАКОЙ ГАРАНТИИ, ЧТО она БУДЕТ РАБОТАТЬ."

MESSAGE5="Выберите файловую систему для образа KNOPPIX. ext3, reiserfs и xfs - это журналируемые файловые системы, очень устойчивые к ошибкам, но они требуют около 30 МБ дополнительного дискового пространства для журнала, который не может быть использован для хранения данных пользователя или программ."

MESSAGE6="Этот образ используется в настоящее время. Если Вы хотите удалить или переформатировать его, пожалуйста, перезагрузите Oralux, не активируя образ $IMAGE."

# =====================
# MESSAGE11="This harddisk partition cannot be mounted in read-write mode currently, sorry."
# E1="Personal configuration (desktop, programs)"
# E2="Network settings (LAN, Modem, ISDN, ADSL)"
# E3="Graphics subsystem settings (XF86Config)"
# E4="Other system configuration (printer etc.)"
# E5="All files on the Desktop (${DESKTOPKB}kB)"
# SUCCESS="SUCCESS!

# Creation of KNOPPIX configuration floppy was successful. Your configuration files will be reinstalled to the ramdisk on next KNOPPIX boot if you specify \"knoppix floppyconf\" (floppy disk), or \"knoppix myconfig=/mnt/directoryname\" at the boot prompt."
# ERROR="The KNOPPIX configuration could NOT be saved:"
# MESSAGE_NO_PARTS="No suitable partitions could be found."
# MESSAGE6="This image is currently in use. If you want to delete/reformat it, please reboot Oralux first, without activating the image $IMAGE."
# MESSAGE7="Do you want to save your home directory encrypted with AES256 (Advanced Encryption Standard, see http://csrc.nist.gov/encryption/aes/)? If yes, you will have to specify a very long password at homedir creation and boot time."
# MESSAGE8="Please enter the desired size of your persistent homedir in MB (currently used: $HOMEKB kB, available:"
# MESSAGE9="Formatting Knoppix-Image and copying data..."
# MESSAGE10="Preparing for Linux filesystem..."
# SUCCESS="The Knoppix-Image has been succeessfully formatted with the Linux ext2 filesystem, and your home directory and system configuration data has been transferred to it.

# You may now reboot your computer, and Oralux should find the image automatically. Alternatively, type \"knoppix home=$PARTITION\" or \"knoppix home=scan\" at the KNOPPIX boot: prompt."

# HEADER="AES256 encryption password (minimum 20 characters!)"
# ENTER="Enter:"
# AGAIN="Again, just to be sure:"

