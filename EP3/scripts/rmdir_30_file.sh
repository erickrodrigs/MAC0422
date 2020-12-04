#!/bin/bash

path="/a"
echo "mount $1" > mkdir_30_file.txt

echo "mkdir $path" >> mkdir_30_file.txt

for i in $(seq 30); do
	path="$path/a${i}"
	echo "mkdir $path" >> mkdir_30_file.txt

	for j in $(seq 120); do
		echo "touch $path/file${j}" >> mkdir_30_file.txt
	done
done

echo "sai" >> mkdir_30_file.txt

./../ep3 < mkdir_30_file.txt

echo "mount $1" > rmdir_30_file.txt
echo "rmdir /a" >> rmdir_30_file.txt
echo "sai" >> rmdir_30_file.txt

ts=$(date +%s%N)
./../ep3 < rmdir_30_file.txt
tt=$((($(date +%s%N) - $ts)/1000000))

echo "$tt" >> rmdir_30_file_$1.txt
rm $1


