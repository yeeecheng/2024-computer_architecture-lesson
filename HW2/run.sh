#!/bin/bash

file_name=("1" "2" "3" "4" "5" "6" "7")

for file_number in "${file_name[@]}"
do
    ./main.exe ./sample_data/sample${file_number}.txt > ./sample${file_number}_res.txt
done