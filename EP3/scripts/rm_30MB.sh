#!/bin/bash

./../ep3 < cp_30MB.txt

echo "mount $1" > rm_30MB.txt
echo "rm /30MB.txt" >> rm_30MB.txt
echo "sai" >> rm_30MB.txt

ts=$(date +%s%N)
./../ep3 < rm_30MB.txt
tt=$((($(date +%s%N) - $ts)/1000000))

echo "$tt" >> rm_30MB_$1.txt
rm $1



