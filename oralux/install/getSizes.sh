#!/bin/sh
# From:
# http://knoppix.net/docs/index.php/KnoppixRemasteringHowto

# size of the packages

dpkg-query -W --showformat='${Installed-Size} ${Package}\n' | sort -n                                                  
