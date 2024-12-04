#!/usr/bin/env bash

sudo accel-config disable-wq dsa0/wq0.0
sudo accel-config disable-device dsa0

sudo accel-config disable-wq dsa2/wq2.0
sudo accel-config disable-device dsa2

sudo accel-config disable-wq dsa4/wq4.0
sudo accel-config disable-device dsa4

sudo accel-config disable-wq dsa6/wq6.0
sudo accel-config disable-device dsa6
