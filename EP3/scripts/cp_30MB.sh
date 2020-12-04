#!/bin/bash

echo "mount $1" > cp_30MB.txt
echo "cp file30MB.txt /30MB.txt" >> cp_30MB.txt
echo "sai" >> cp_30MB.txt

ts=$(date +%s%N)
./../ep3 < cp_30MB.txt
tt=$((($(date +%s%N) - $ts)/1000000))

echo "$tt" >> cp_30MB_$1.txt
rm $1


