#!/bin/bash

gcc -o averages averages.c -lm
echo "Testes: " > outputs.txt
for i in $(ls trash)
do
    echo -e "${i}: \n" >> outputs.txt
    ./averages trash/${i} >> outputs.txt
    echo -e "cabô\n" >> outputs.txt
done
