#!/usr/bin/env bash
set -v

# sudo accel-config config-wq dsa0/wq0.0 --group-id=0
# sudo accel-config config-wq dsa0/wq0.0 --priority=5
# sudo accel-config config-wq dsa0/wq0.0 --wq-size=8
# sudo accel-config config-engine dsa0/engine0.0 --group-id=0
# sudo accel-config config-wq dsa0/wq0.0 --type=user
# sudo accel-config config-wq dsa0/wq0.0 --name="dsa-test"
# sudo accel-config config-wq dsa0/wq0.0 --mode=dedicated
# sudo accel-config enable-device dsa0
# sudo accel-config enable-wq dsa0/wq0.0

# sudo accel-config load-config -c build_dirs/libaccel/contrib/configs/app_profile.conf -e

sudo accel-config config-wq --group-id=0 --mode=shared --wq-size=128 --threshold=127 --type=user --priority=10 --name="app-0" --driver-name="user" --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa0/wq0.0
sudo accel-config config-engine dsa0/engine0.0 --group-id=0
# sudo accel-config config-engine dsa0/engine0.1 --group-id=0
# sudo accel-config config-engine dsa0/engine0.2 --group-id=0
# sudo accel-config config-engine dsa0/engine0.3 --group-id=0
sudo accel-config enable-device dsa0
sudo accel-config enable-wq dsa0/wq0.0

# sudo accel-config config-wq --group-id=0 --mode=shared --wq-size=128 --threshold=127 --type=user --priority=10 --name="app-1" --driver-name="user" --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa2/wq2.0
# sudo accel-config config-engine dsa2/engine2.0 --group-id=0
# sudo accel-config config-engine dsa2/engine2.1 --group-id=0
# sudo accel-config config-engine dsa2/engine2.2 --group-id=0
# sudo accel-config config-engine dsa2/engine2.3 --group-id=0
# sudo accel-config enable-device dsa2
# sudo accel-config enable-wq dsa2/wq2.0

# sudo accel-config config-wq --group-id=0 --mode=shared --wq-size=128 --threshold=127 --type=user --priority=10 --name="app-2" --driver-name="user" --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa4/wq4.0
# sudo accel-config config-engine dsa4/engine4.0 --group-id=0
# sudo accel-config config-engine dsa4/engine4.1 --group-id=0
# sudo accel-config config-engine dsa4/engine4.2 --group-id=0
# sudo accel-config config-engine dsa4/engine4.3 --group-id=0
# sudo accel-config enable-device dsa4
# sudo accel-config enable-wq dsa4/wq4.0

# sudo accel-config config-wq --group-id=0 --mode=shared --wq-size=128 --threshold=127 --type=user --priority=10 --name="app-3" --driver-name="user" --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa6/wq6.0
# sudo accel-config config-engine dsa6/engine6.0 --group-id=0
# sudo accel-config config-engine dsa6/engine6.1 --group-id=0
# sudo accel-config config-engine dsa6/engine6.2 --group-id=0
# sudo accel-config config-engine dsa6/engine6.3 --group-id=0
# sudo accel-config enable-device dsa6
# sudo accel-config enable-wq dsa6/wq6.0

./set_group_perms.sh

sudo rdmsr 0xc8b
