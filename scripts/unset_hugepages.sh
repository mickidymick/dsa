#!/usr/bin/env bash

nr_pages_0=0 #0GB
nr_pages_1=0 #0GB
nr_pages_4=0 #0GB

# echo ${tot_pages} | sudo tee -a /proc/sys/vm/nr/hugepages

echo ${nr_pages_0} | sudo tee -a /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages
echo ${nr_pages_1} | sudo tee -a /sys/devices/system/node/node1/hugepages/hugepages-2048kB/nr_hugepages
echo ${nr_pages_4} | sudo tee -a /sys/devices/system/node/node4/hugepages/hugepages-2048kB/nr_hugepages

cat /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages
cat /sys/devices/system/node/node1/hugepages/hugepages-2048kB/nr_hugepages
cat /sys/devices/system/node/node4/hugepages/hugepages-2048kB/nr_hugepages

cat /proc/meminfo
