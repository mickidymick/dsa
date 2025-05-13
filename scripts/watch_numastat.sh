#!/usr/bin/env bash

watch -n 0.5 "numastat -m | tail -n +35"
