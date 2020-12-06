#!/bin/bash

function cp_test () {
    echo "mount $1" > cp_$2MB.txt
    echo "cp file$2MB.txt /$2MB.txt" >> cp_$2MB.txt
    echo "sai" >> cp_$2MB.txt

    local ts=$(date +%s%N)
    ./../ep3 < cp_$2MB.txt
    local tt=$((($(date +%s%N) - $ts)/1000000))

    echo "$tt" >> cp_$2MB_$1.txt
}

function rm_test () {
    ./../ep3 < cp_$2MB.txt

    echo "mount $1" > rm_$2MB.txt
    echo "rm /$2MB.txt" >> rm_$2MB.txt
    echo "sai" >> rm_$2MB.txt

    local ts=$(date +%s%N)
    ./../ep3 < rm_$2MB.txt
    local tt=$((($(date +%s%N) - $ts)/1000000))

    echo "$tt" >> rm_$2MB_$1.txt
}

function rmdir_test () {
    echo "mount $1" > rmdir_30_$2.txt
    echo "rmdir /a" >> rmdir_30_$2.txt
    echo "sai" >> rmdir_30_$2.txt

    local ts=$(date +%s%N)
    ./../ep3 < rmdir_30_$2.txt
    local tt=$((($(date +%s%N) - $ts)/1000000))

    echo "$tt" >> rmdir_30_$2_$1.txt
}

function generate_disks () {
    if [ $1 = "empty.disk" ]; then
        echo "mount empty.disk" > empty_disk.txt
        echo "sai" >> empty_disk.txt
        ./../ep3 < empty_disk.txt
    elif [ $1 = "10MB.disk" ]; then
        echo "mount 10MB.disk" > 10MB_disk.txt
        for i in $(seq 1 100); do
            echo "cp lorem.txt /a${i}.txt" >> 10MB_disk.txt
        done
        echo "sai" >> 10MB_disk.txt
        ./../ep3 < 10MB_disk.txt
    else
        echo "mount 50MB.disk" > 50MB_disk.txt
        for i in $(seq 1 500); do
            echo "cp lorem.txt /a${i}.txt" >> 50MB_disk.txt
        done
        echo "sai" >> 50MB_disk.txt
        ./../ep3 < 50MB_disk.txt
    fi
}

function generate_mkdir () {
    local path="/a"
    echo "mount $1" > mkdir_30_$2.txt
    echo "mkdir $path" >> mkdir_30_$2.txt

    for i in $(seq 30); do
        local path="$path/a${i}"
        echo "mkdir $path" >> mkdir_30_$2.txt

        if [ $2 = "file" ]; then
            for j in $(seq 120); do
                echo "touch $path/file${j}" >> mkdir_30_$2.txt
            done
        fi
    done

    echo "sai" >> mkdir_30_$2.txt

    ./../ep3 < mkdir_30_$2.txt
}

cd ..
make
cd scripts

./generate_files.sh

for disk in empty.disk 10MB.disk 50MB.disk; do
    generate_disks $disk
    cp $disk copy_$disk

    for cmd in cp_ rm_; do
        for size in 1 10 30; do
            for i in $(seq 30); do
                cp copy_$disk $disk
                ${cmd}test $disk $size
            done
        done
    done

    generate_mkdir copy_$disk "nofile"

    for i in $(seq 30); do
        cp copy_$disk $disk
        rmdir_test $disk "nofile"
    done

    rm copy_$disk
    generate_disks $disk
    cp $disk copy_$disk

    generate_mkdir copy_$disk "file"

    for i in $(seq 30); do
        cp copy_$disk $disk
        rmdir_test $disk "file"
    done
done

rm -rf trash
mkdir trash
mv *.disk.txt trash/

./averages.sh

mv lorem.txt trash/
mv outputs.txt trash/
rm *.txt
rm *.disk
mv trash/lorem.txt .
mv trash/outputs.txt .
