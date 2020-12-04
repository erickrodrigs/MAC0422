#!/bin/bash

cd ..
make
cd scripts

./generate_files.sh

for disk in scripts/empty.disk scripts/10MB.disk scripts/50MB.disk; do
	for size in 1 10 30; do
		for cmd in cp_ rm_; do
			for i in $(seq 30); do
				./generate_disks.sh $disk
				./${cmd}${size}MB.sh $disk
			done
		done
	done

	for cmd in rmdir_30_nofile rmdir_30_file; do
		for i in $(seq 30); do
			./generate_disks.sh $disk
			./${cmd}.sh $disk
		done
	done
done

