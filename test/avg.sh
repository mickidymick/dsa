#!/usr/bin/env bash

avg=0
size=10
for i in $(seq 1 $size);
do
    str=$(make test)
    echo "Run "$i" -- "$str
    avg=$(echo "$str + $avg" | bc)
done
avg=$(echo "scale=2; $avg / $size" | bc)
echo $avg
