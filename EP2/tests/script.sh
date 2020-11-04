#!/bin/bash

for TRACK in 350; do
	for CYCLISTS in 100; do
		for a in $(seq 21 30); do
			if [ $a -eq 30 ]; then
				./../ep2 $TRACK $CYCLISTS output_${TRACK}_${CYCLISTS}_${a}.txt
			else
				./../ep2 $TRACK $CYCLISTS output_${TRACK}_${CYCLISTS}_${a}.txt&
			fi
		done
	done
done

