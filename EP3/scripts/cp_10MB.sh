#!/bin/bash

echo "mount $1" > cp_10MB.txt
echo "cp file10MB.txt /10MB.txt" >> cp_10MB.txt
echo "sai" >> cp_10MB.txt

ts=$(date +%s%N)
./../ep3 < cp_10MB.txt
tt=$((($(date +%s%N) - $ts)/1000000))

echo "$tt" >> cp_10MB_$1.txt
rm $1


