#!/bin/bash

./../ep3 < cp_10MB.txt

echo "mount $1" > rm_10MB.txt
echo "rm /10MB.txt" >> rm_10MB.txt
echo "sai" >> rm_10MB.txt

ts=$(date +%s%N)
./../ep3 < rm_10MB.txt
tt=$((($(date +%s%N) - $ts)/1000000))

echo "$tt" >> rm_10MB_$1.txt
rm $1



