#!/bin/sh
echo "Oralux-installer 0.2 for 0.6 alpha"

cp knoppix-autoconfig /tmp

# TBD: To be removed
export PATH=".:$PATH"
cd install
knoppix-installer

