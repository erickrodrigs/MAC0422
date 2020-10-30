#!/bin/bash

A=1

while [ $A -eq 1 ]
do
	./ep2 10 26 > log.txt

	cat log.txt

	if [ $? -eq 139 ]; then
		echo "OVO MATA O SEGFAULTO KKKKKKKKK"
		exit 1
	fi
done

