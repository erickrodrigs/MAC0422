#!/bin/bash
make input_generator
make average
make tester
cd ..
make
cd tests

a=1
b=2
c=3

for CASES in a b c
do
    ./input_generator input$CASES.txt $CASES
    cd ..
    for SCHEDULER in 1 2 3
    do
        for SAMPLE in $(seq 1 30)
        do
            ./ep1 $SCHEDULER tests/input$CASES.txt output$CASES.txt
            cd tests
            ./tester input$CASES.txt ../output$CASES.txt >> result.txt
        done
        ./average $CASES $SCHEDULER < result.txt >> averages.txt
        rm result.txt
    done
    rm input$CASES.txt
    cd ..
    rm output$CASES.txt
    cd tests
done