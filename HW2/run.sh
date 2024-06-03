#!/bin/bash

file_name=("1" "2" "3" "4" "5" "6" "7")

for file_number in "${file_name[@]}"
do
    ./main.exe ./sample_data/sample${file_number}.txt > ./sample_res/sample${file_number}_res.txt
done

test_file_name=("1" "2" "3" "4")
for file_number in "${test_file_name[@]}"
do
    ./main.exe ./test_data/test${file_number}.txt > ./test_res/test${file_number}_res.txt
done