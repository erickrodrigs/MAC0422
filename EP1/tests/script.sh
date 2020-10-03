#!/bin/bash
make input_generator
gcc averages.c -o averages -lm
make tester
cd ..
make
cd tests

a=1
b=2
c=3

for CASES in 1 2 3
do
    echo CASE $CASE
    ./input_generator input$CASES.txt $CASES
    cd ..
    for SCHEDULER in 1 2 3
    do
        echo SCHEDULER $SCHEDULER
        for SAMPLE in $(seq 1 30)
        do
            echo SAMPLE $SAMPLE
            ./ep1 $SCHEDULER tests/input$CASES.txt output$CASES.txt
            cd tests
            ./tester input$CASES.txt ../output$CASES.txt >> result.txt
            cd ..
        done
        echo rodando averages
        cd tests
        ./averages $CASES $SCHEDULER < result.txt >> averages.txt
        rm result.txt
        cd ..
    done
    cd tests
    rm input$CASES.txt
    cd ..
    rm output$CASES.txt
    cd tests
done