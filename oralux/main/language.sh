#!/bin/sh
# From knoppix-autoconfig
# Set the variables related to keyboard/language
# Build the concerned files.
#
# usage
# language.sh en us

LANGUAGE=$1
ORALUXTTSLANG=$2

# The default language/keyboard to use. This CANNOT be autoprobed.
# Most of these variables will be used to generate the KDE defaults
# and will be inserted into /etc/sysconfig/* below.
case "$LANGUAGE" in
de)
# German version
COUNTRY="de"
LANG="de_DE@euro"
KEYTABLE="de-latin1-nodeadkeys"
XKEYBOARD="de"
KDEKEYBOARD="de"
CHARSET="iso8859-15"
# Additional KDE Keyboards
KDEKEYBOARDS="us,fr"
;;
# BE version
be)
LANGUAGE="be"
COUNTRY="be"
LANG="be"
KEYTABLE="be-latin1"
XKEYBOARD="be"
KDEKEYBOARD="be"
CHARSET="iso8859-15"
# Additional KDE Keyboards
KDEKEYBOARDS="us,de,fr"
;;
# BG version
bg)
LANGUAGE="bg"
COUNTRY="bg"
LANG="bg_BG"
KEYTABLE="bg"
XKEYBOARD="bg"
KDEKEYBOARD="bg"
CHARSET="microsoft-cp1251"
# Additional KDE Keyboards
KDEKEYBOARDS="us,de,fr"
;;
# Swiss version (basically de with some modifications)
ch)
LANGUAGE="de"
COUNTRY="ch"
LANG="de_CH"
KEYTABLE="sg-latin1"
XKEYBOARD="de_CH"
KDEKEYBOARD="de_CH"
CHARSET="iso8859-15"
# Additional KDE Keyboards
KDEKEYBOARDS="de,us,fr"
;;
cn)
# Simplified Chinese version
COUNTRY="cn"
LANG="zh_CN.GB2312"
KEYTABLE="us"
XKEYBOARD="us"
KDEKEYBOARD="us"
CHARSET="gb2312.1980-0"
# Additional KDE Keyboards
KDEKEYBOARDS="us,de,fr"
XMODIFIERS="@im=Chinput"
;;
# Czech version
cs|cz)
LANGUAGE="cs"
COUNTRY="cs"
LANG="cs_CZ"
KEYTABLE="cz-lat2"
XKEYBOARD="cs"
KDEKEYBOARD="cs"
CHARSET="iso8859-2"
# Additional KDE Keyboards
KDEKEYBOARDS="us,de,fr"
;;
dk|da)
# Dansk version
COUNTRY="dk"
LANG="da_DK"
# Workaround: "dk" broken in gettext, use da:da_DK
LANGUAGE="da:da_DK"
# Keytable "dk" is correct.
KEYTABLE="dk"
XKEYBOARD="dk"
KDEKEYBOARD="dk"
CHARSET="iso8859-15"
# Additional KDE Keyboards
KDEKEYBOARDS="dk,de,us,fr"
;;
es)
# Spanish version
COUNTRY="es"
LANG="es_ES@euro"
KEYTABLE="es"
XKEYBOARD="es"
KDEKEYBOARD="es"
CHARSET="iso8859-15"
# Additional KDE Keyboards
KDEKEYBOARDS="de,us,fr"
;;
fi)
# finnish version, though we may not have the kde-i18n files
COUNTRY="fi"
LANG="fi_FI@euro"
KEYTABLE="fi"
XKEYBOARD="fi"
KDEKEYBOARD="fi"
CHARSET="iso8859-15"
# Additional KDE Keyboards
KDEKEYBOARDS="us"
;;
fr)
# french version
COUNTRY="fr"
LANG="fr_FR@euro"
KEYTABLE="fr"
XKEYBOARD="fr"
KDEKEYBOARD="fr"
CHARSET="iso8859-15"
# Additional KDE Keyboards
KDEKEYBOARDS="de,us"
;;
he|il)
# Hebrew version
LANGUAGE="he"
COUNTRY="il"
LANG="he_IL"
KEYTABLE="us"
XKEYBOARD="us"
KDEKEYBOARD="il"
CHARSET="iso8859-8"
# Additional KDE Keyboards
KDEKEYBOARDS="us,fr,de"
;;
it)
# italian version
COUNTRY="it"
LANG="it_IT@euro"
KEYTABLE="it"
XKEYBOARD="it"
KDEKEYBOARD="it"
CHARSET="iso8859-15"
# Additional KDE Keyboards
KDEKEYBOARDS="fr,us,de"
;;
ja)
# (limited) japanese version
COUNTRY="jp"
LANG="ja_JP"
LANGUAGE="ja"
KEYTABLE="us"
XKEYBOARD="us"
KDEKEYBOARD="us"
CHARSET="iso8859-15"
# Additional KDE Keyboards
KDEKEYBOARDS="fr,us,de"
;;
nl)
# netherland version
COUNTRY="nl"
LANG="nl_NL@euro"
KEYTABLE="us"
XKEYBOARD="us"
KDEKEYBOARD="en_US"
CHARSET="iso8859-15"
# Additional KDE Keyboards
KDEKEYBOARDS="nl,de,fr"
;;
pl)
# Polish version
COUNTRY="pl"
LANG="pl_PL"
KEYTABLE="pl"
XKEYBOARD="pl"
KDEKEYBOARD="pl"
CHARSET="iso8859-2"
# Additional KDE Keyboards
KDEKEYBOARDS="de,us,fr"
;;
ru)
# Russian version
COUNTRY="ru"
LANG="ru_RU.KOI8-R"
KEYTABLE="ru"
XKEYBOARD="ru"
KDEKEYBOARD="ru"
CHARSET="koi8-r"
CONSOLEFONT="Cyr_a8x16"
# Additional KDE Keyboards
KDEKEYBOARDS="de,us,fr"
;;
sk)
# Slovak version (guessed)
COUNTRY="sk"
LANG="sk"
KEYTABLE="sk-qwerty"
XKEYBOARD="sk"
KDEKEYBOARD="sk"
CHARSET="iso8859-2"
# Additional KDE Keyboards
KDEKEYBOARDS="us,de,fr"
;;
tr)
# Turkish version (guessed)
COUNTRY="tr"
LANG="tr_TR"
KEYTABLE="tr_q-latin5"
XKEYBOARD="tr"
KDEKEYBOARD="tr"
CHARSET="iso8859-9"
# Additional KDE Keyboards
KDEKEYBOARDS="us,de,fr"
;;
tw)
# Traditional chinese version (thanks to Chung-Yen Chang)
COUNTRY="tw"
LANG="zh_TW.Big5"
LANGUAGE="zh_TW.Big5"
KEYTABLE="us"
XKEYBOARD="us"
KDEKEYBOARD="us"
# CHARSET="big5-0"
CHARSET="iso8859-1"
# Additional KDE Keyboards
KDEKEYBOARDS="us"
XMODIFIERS="@im=xcin"
;;
uk)
# british version
COUNTRY="uk"
LANG="en_GB"
KEYTABLE="uk"
XKEYBOARD="uk"
KDEKEYBOARD="uk"
CHARSET="iso8859-1"
# Additional KDE Keyboards
KDEKEYBOARDS="us"
;;
*)
# US version
LANGUAGE="us"
COUNTRY="us"
LANG="C"
KEYTABLE="us"
XKEYBOARD="us"
KDEKEYBOARD="us"
CHARSET="iso8859-1"
# Additional KDE Keyboards
KDEKEYBOARDS="de,fr"
;;
esac

# Export it now, so error messages get translated, too
export LANGUAGE LANG COUNTRY CHARSET


# Delete obsolete links and files
rm -f /etc/sysconfig/i18n /etc/sysconfig/keyboard /etc/sysconfig/desktop /etc/sysconfig/knoppix 2>/dev/null

# Write KNOPPIX config files for other scripts to parse
# Standard variables/files + ORALUXTTSLANG
echo "LANG=\"$LANG\""                  > /etc/sysconfig/i18n
echo "COUNTRY=\"$COUNTRY\""           >> /etc/sysconfig/i18n
echo "LANG=\"$LANG\""                 >> /etc/sysconfig/i18n
echo "LANGUAGE=\"$LANGUAGE\""         >> /etc/sysconfig/i18n
echo "CHARSET=\"$CHARSET\""           >> /etc/sysconfig/i18n
echo "XMODIFIERS=\"$XMODIFIERS\""     >> /etc/sysconfig/i18n
echo "ORALUXTTSLANG=\"$ORALUXTTSLANG\""           >> /etc/sysconfig/knoppix

# Default Keyboard layout for console and X
echo "KEYTABLE=\"$KEYTABLE\""          > /etc/sysconfig/keyboard
echo "XKEYBOARD=\"$XKEYBOARD\""       >> /etc/sysconfig/keyboard
echo "KDEKEYBOARD=\"$KDEKEYBOARD\""   >> /etc/sysconfig/keyboard
echo "KDEKEYBOARDS=\"$KDEKEYBOARDS\"" >> /etc/sysconfig/keyboard

# Desired desktop
echo "DESKTOP=\"$DESKTOP\""            > /etc/sysconfig/desktop

# Write all, including non-standard variables, to /etc/sysconfig/knoppix
echo "LANG=\"$LANG\""                  > /etc/sysconfig/knoppix
echo "COUNTRY=\"$COUNTRY\""           >> /etc/sysconfig/knoppix
echo "LANGUAGE=\"$LANGUAGE\""         >> /etc/sysconfig/knoppix
echo "CHARSET=\"$CHARSET\""           >> /etc/sysconfig/knoppix
echo "KEYTABLE=\"$KEYTABLE\""         >> /etc/sysconfig/knoppix
echo "XKEYBOARD=\"$XKEYBOARD\""       >> /etc/sysconfig/knoppix
echo "KDEKEYBOARD=\"$KDEKEYBOARD\""   >> /etc/sysconfig/knoppix
echo "KDEKEYBOARDS=\"$KDEKEYBOARDS\"" >> /etc/sysconfig/knoppix
echo "DESKTOP=\"$DESKTOP\""           >> /etc/sysconfig/knoppix
echo "ORALUXTTSLANG=\"$ORALUXTTSLANG\"" >> /etc/sysconfig/knoppix

loadkeys $KEYTABLE