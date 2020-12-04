#!/bin/bash

echo "mount $1" > cp_1MB.txt
echo "cp file1MB.txt /1MB.txt" >> cp_1MB.txt
echo "sai" >> cp_1MB.txt

ts=$(date +%s%N)
./../ep3 < cp_1MB.txt
tt=$((($(date +%s%N) - $ts)/1000000))

echo "$tt" >> cp_1MB_$1.txt
rm $1

