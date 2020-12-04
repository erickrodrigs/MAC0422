#!/bin/bash

./../ep3 < cp_1MB.txt

echo "mount $1" > rm_1MB.txt
echo "rm /1MB.txt" >> rm_1MB.txt
echo "sai" >> rm_1MB.txt

ts=$(date +%s%N)
./../ep3 < rm_1MB.txt
tt=$((($(date +%s%N) - $ts)/1000000))

echo "$tt" >> rm_1MB_$1.txt
rm $1


