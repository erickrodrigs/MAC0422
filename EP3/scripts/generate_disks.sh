#!/bin/bash

if [ $1 = "empty.disk" ]; then
	echo "mount empty.disk" >> empty_disk.txt
	echo "sai"
	./../ep3 < empty_disk.txt
elif [ $1 = "10MB.disk" ]; then
	echo "mount 10MB.disk" >> 10MB_disk.txt
	for i in $(seq 1 100); do
		echo "cp lorem.txt /prefile/a${i}.txt" >> 10MB_disk.txt
	done
	echo "sai" >> 10MB_disk.txt
	./../ep3 < 10MB_disk.txt
else
	echo "mount 50MB.disk" >> 50MB_disk.txt
	for i in $(seq 1 500); do
		echo "cp lorem.txt /prefile/a${i}.txt" >> 50MB_disk.txt
	done
	echo "sai" >> 50MB_disk.txt
	./../ep3 < 50MB_disk.txt
fi

