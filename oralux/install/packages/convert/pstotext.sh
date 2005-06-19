#!/bin/bash
#set -vx

mode=$1
src=$2

ext=txt
dir=/tmp
cmd=pstotext

dest=$(basename "$src")"_"$$.$ext
fulldest="$dir/$dest"
touch "$fulldest"
chmod 600 "$fulldest"

case "$mode" in
    --edit) # editor
	$cmd "$src" > "$fulldest"
	echo "$fulldest";;
    *)  # pager
	$cmd "$src";;
esac
