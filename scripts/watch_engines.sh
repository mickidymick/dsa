#!/usr/bin/env bash

cd /sys/bus/dsa/drivers/user
for dir in wq*.0; do
    printf "[%s] clients:%s occupancy:%s\n" "$dir" $(cat "$dir"/clients) $(cat "$dir"/occupancy)
done
