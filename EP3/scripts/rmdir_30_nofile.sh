#!/bin/bash

path="/a"
echo "mount $1" > mkdir_30_nofile.txt

echo "mkdir $path" >> mkdir_30_nofile.txt

for i in $(seq 30); do
	path="$path/a${i}"
	echo "mkdir $path" >> mkdir_30_nofile.txt
done

echo "sai" >> mkdir_30_nofile.txt

./../ep3 < mkdir_30_nofile.txt

echo "mount $1" > rmdir_30_nofile.txt
echo "rmdir /a" >> rmdir_30_nofile.txt
echo "sai" >> rmdir_30_nofile.txt

ts=$(date +%s%N)
./../ep3 < rmdir_30_nofile.txt
tt=$((($(date +%s%N) - $ts)/1000000))

echo "$tt" >> rmdir_30_nofile_$1.txt
rm $1


