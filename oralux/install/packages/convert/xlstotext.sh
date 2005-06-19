#!/bin/bash
#set -vx

mode=$1
src=$2

ext=txt
dir=/tmp
cmd=xlhtml

dest=$(basename "$src")"_"$$.$ext
fulldest="$dir/$dest"
touch "$fulldest"
chmod 600 "$fulldest"

case "$mode" in
    --edit) # editor
	touch "$fulldest.tmp"
	chmod 600 "$fulldest.tmp"
	$cmd "$src" > "$fulldest.tmp"
	lynx -force_html -dump "$fulldest.tmp" > "$fulldest"
	rm "$fulldest.tmp"
	echo "$fulldest";;
    *)  # pager
	$cmd "$src" > "$fulldest"
	lynx -force_html -dump "$fulldest"
	rm "$fulldest";;
esac

