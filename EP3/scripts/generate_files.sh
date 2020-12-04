#!/bin/bash

for i in $(seq 1 10); do
	cat lorem.txt >> file1MB.txt
done

for i in $(seq 1 10); do
        cat file1MB.txt >> file10MB.txt
done

for i in $(seq 1 3); do
        cat file10MB.txt >> file30MB.txt
done


