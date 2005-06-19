#!/bin/bash
set -vx

mode=$1
src=$2

ext=txt
dir=/tmp

# Info about o3read from Willem van der Walt

dest=$(basename "$src")"_"$$.$ext
fulldest="$dir/$dest"
touch "$fulldest"
chmod 600 "$fulldest"

case "$mode" in
    --edit) # editor
	unzip -p "$src" content.xml | o3totxt  > "$fulldest"
	echo "$fulldest";;
    *)  # pager
	unzip -p "$src" content.xml | o3totxt;;
esac
