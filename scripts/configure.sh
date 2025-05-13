#!/usr/bin/env bash
set -v

./disable.sh

# sudo accel-config config-wq --group-id=0 --mode=shared --wq-size=32 --threshold=31 --type=user --priority=10 --name="app-0" --driver-name="user" --max-batch-size 128 --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa0/wq0.0
sudo accel-config config-wq --group-id=0 --mode=shared --wq-size=128 --threshold=127 --type=user --priority=10 --name="app-0" --driver-name="user" --max-batch-size 512 --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa0/wq0.0
# sudo accel-config config-group dsa0/group0.0 -t 96 -r 96 -d 3
sudo accel-config config-engine dsa0/engine0.0 --group-id=0
sudo accel-config config-engine dsa0/engine0.1 --group-id=0
sudo accel-config config-engine dsa0/engine0.2 --group-id=0
sudo accel-config config-engine dsa0/engine0.3 --group-id=0
sudo accel-config enable-device dsa0
sudo accel-config enable-wq dsa0/wq0.0

# sudo accel-config config-wq --group-id=0 --mode=shared --wq-size=32 --threshold=31 --type=user --priority=10 --name="app-0" --driver-name="user" --max-batch-size 128 --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa1/wq1.0
sudo accel-config config-wq --group-id=0 --mode=shared --wq-size=128 --threshold=127 --type=user --priority=10 --name="app-0" --driver-name="user" --max-batch-size 512 --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa1/wq1.0
sudo accel-config config-engine dsa1/engine1.0 --group-id=0
sudo accel-config config-engine dsa1/engine1.1 --group-id=0
sudo accel-config config-engine dsa1/engine1.2 --group-id=0
sudo accel-config config-engine dsa1/engine1.3 --group-id=0
sudo accel-config enable-device dsa1
sudo accel-config enable-wq dsa1/wq1.0

# sudo accel-config config-wq --group-id=0 --mode=shared --wq-size=32 --threshold=31 --type=user --priority=10 --name="app-0" --driver-name="user" --max-batch-size 128 --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa2/wq2.0
sudo accel-config config-wq --group-id=0 --mode=shared --wq-size=128 --threshold=127 --type=user --priority=10 --name="app-0" --driver-name="user" --max-batch-size 512 --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa2/wq2.0
sudo accel-config config-engine dsa2/engine2.0 --group-id=0
sudo accel-config config-engine dsa2/engine2.1 --group-id=0
sudo accel-config config-engine dsa2/engine2.2 --group-id=0
sudo accel-config config-engine dsa2/engine2.3 --group-id=0
sudo accel-config enable-device dsa2
sudo accel-config enable-wq dsa2/wq2.0

# sudo accel-config config-wq --group-id=0 --mode=shared --wq-size=32 --threshold=31 --type=user --priority=10 --name="app-0" --driver-name="user" --max-batch-size 128 --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa3/wq3.0
sudo accel-config config-wq --group-id=0 --mode=shared --wq-size=128 --threshold=127 --type=user --priority=10 --name="app-0" --driver-name="user" --max-batch-size 512 --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa3/wq3.0
sudo accel-config config-engine dsa3/engine3.0 --group-id=0
sudo accel-config config-engine dsa3/engine3.1 --group-id=0
sudo accel-config config-engine dsa3/engine3.2 --group-id=0
sudo accel-config config-engine dsa3/engine3.3 --group-id=0
sudo accel-config enable-device dsa3
sudo accel-config enable-wq dsa3/wq3.0

# sudo accel-config config-wq --group-id=0 --mode=shared --wq-size=32 --threshold=31 --type=user --priority=10 --name="app-0" --driver-name="user" --max-batch-size 128 --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa4/wq4.0
sudo accel-config config-wq --group-id=0 --mode=shared --wq-size=128 --threshold=127 --type=user --priority=10 --name="app-0" --driver-name="user" --max-batch-size 512 --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa4/wq4.0
sudo accel-config config-engine dsa4/engine4.0 --group-id=0
sudo accel-config config-engine dsa4/engine4.1 --group-id=0
sudo accel-config config-engine dsa4/engine4.2 --group-id=0
sudo accel-config config-engine dsa4/engine4.3 --group-id=0
sudo accel-config enable-device dsa4
sudo accel-config enable-wq dsa4/wq4.0

# sudo accel-config config-wq --group-id=0 --mode=shared --wq-size=32 --threshold=31 --type=user --priority=10 --name="app-0" --driver-name="user" --max-batch-size 128 --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa5/wq5.0
sudo accel-config config-wq --group-id=0 --mode=shared --wq-size=128 --threshold=127 --type=user --priority=10 --name="app-0" --driver-name="user" --max-batch-size 512 --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa5/wq5.0
sudo accel-config config-engine dsa5/engine5.0 --group-id=0
sudo accel-config config-engine dsa5/engine5.1 --group-id=0
sudo accel-config config-engine dsa5/engine5.2 --group-id=0
sudo accel-config config-engine dsa5/engine5.3 --group-id=0
sudo accel-config enable-device dsa5
sudo accel-config enable-wq dsa5/wq5.0

# sudo accel-config config-wq --group-id=0 --mode=shared --wq-size=32 --threshold=31 --type=user --priority=10 --name="app-0" --driver-name="user" --max-batch-size 128 --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa6/wq6.0
sudo accel-config config-wq --group-id=0 --mode=shared --wq-size=128 --threshold=127 --type=user --priority=10 --name="app-0" --driver-name="user" --max-batch-size 512 --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa6/wq6.0
sudo accel-config config-engine dsa6/engine6.0 --group-id=0
sudo accel-config config-engine dsa6/engine6.1 --group-id=0
sudo accel-config config-engine dsa6/engine6.2 --group-id=0
sudo accel-config config-engine dsa6/engine6.3 --group-id=0
sudo accel-config enable-device dsa6
sudo accel-config enable-wq dsa6/wq6.0

# sudo accel-config config-wq --group-id=0 --mode=shared --wq-size=32 --threshold=31 --type=user --priority=10 --name="app-0" --driver-name="user" --max-batch-size 128 --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa7/wq7.0
sudo accel-config config-wq --group-id=0 --mode=shared --wq-size=128 --threshold=127 --type=user --priority=10 --name="app-0" --driver-name="user" --max-batch-size 512 --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa7/wq7.0
sudo accel-config config-engine dsa7/engine7.0 --group-id=0
sudo accel-config config-engine dsa7/engine7.1 --group-id=0
sudo accel-config config-engine dsa7/engine7.2 --group-id=0
sudo accel-config config-engine dsa7/engine7.3 --group-id=0
sudo accel-config enable-device dsa7
sudo accel-config enable-wq dsa7/wq7.0

# sudo accel-config config-wq --group-id=0 --mode=shared --wq-size=32 --threshold=31 --type=user --priority=10 --name="app-1" --driver-name="user" --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa0/wq0.0
# sudo accel-config config-wq --group-id=1 --mode=shared --wq-size=32 --threshold=31 --type=user --priority=10 --name="app-1" --driver-name="user" --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa0/wq0.1
# sudo accel-config config-wq --group-id=2 --mode=shared --wq-size=32 --threshold=31 --type=user --priority=10 --name="app-1" --driver-name="user" --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa0/wq0.2
# sudo accel-config config-wq --group-id=3 --mode=shared --wq-size=32 --threshold=31 --type=user --priority=10 --name="app-1" --driver-name="user" --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa0/wq0.3
# sudo accel-config config-engine dsa0/engine0.0 --group-id=0
# sudo accel-config config-engine dsa0/engine0.1 --group-id=1
# sudo accel-config config-engine dsa0/engine0.2 --group-id=2
# sudo accel-config config-engine dsa0/engine0.3 --group-id=3
# sudo accel-config enable-device dsa0
# sudo accel-config enable-wq dsa0/wq0.0
# sudo accel-config enable-wq dsa0/wq0.1
# sudo accel-config enable-wq dsa0/wq0.2
# sudo accel-config enable-wq dsa0/wq0.3

# sudo accel-config config-wq --group-id=0 --mode=shared --wq-size=32 --threshold=31 --type=user --priority=10 --name="app-1" --driver-name="user" --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa1/wq1.0
# sudo accel-config config-wq --group-id=1 --mode=shared --wq-size=32 --threshold=31 --type=user --priority=10 --name="app-1" --driver-name="user" --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa1/wq1.1
# sudo accel-config config-wq --group-id=2 --mode=shared --wq-size=32 --threshold=31 --type=user --priority=10 --name="app-1" --driver-name="user" --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa1/wq1.2
# sudo accel-config config-wq --group-id=3 --mode=shared --wq-size=32 --threshold=31 --type=user --priority=10 --name="app-1" --driver-name="user" --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa1/wq1.3
# sudo accel-config config-engine dsa1/engine1.0 --group-id=0
# sudo accel-config config-engine dsa1/engine1.1 --group-id=1
# sudo accel-config config-engine dsa1/engine1.2 --group-id=2
# sudo accel-config config-engine dsa1/engine1.3 --group-id=3
# sudo accel-config enable-device dsa1
# sudo accel-config enable-wq dsa1/wq1.0
# sudo accel-config enable-wq dsa1/wq1.1
# sudo accel-config enable-wq dsa1/wq1.2
# sudo accel-config enable-wq dsa1/wq1.3

# sudo accel-config config-wq --group-id=0 --mode=shared --wq-size=32 --threshold=31 --type=user --priority=10 --name="app-1" --driver-name="user" --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa2/wq2.0
# sudo accel-config config-wq --group-id=1 --mode=shared --wq-size=32 --threshold=31 --type=user --priority=10 --name="app-1" --driver-name="user" --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa2/wq2.1
# sudo accel-config config-wq --group-id=2 --mode=shared --wq-size=32 --threshold=31 --type=user --priority=10 --name="app-1" --driver-name="user" --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa2/wq2.2
# sudo accel-config config-wq --group-id=3 --mode=shared --wq-size=32 --threshold=31 --type=user --priority=10 --name="app-1" --driver-name="user" --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa2/wq2.3
# sudo accel-config config-engine dsa2/engine2.0 --group-id=0
# sudo accel-config config-engine dsa2/engine2.1 --group-id=1
# sudo accel-config config-engine dsa2/engine2.2 --group-id=2
# sudo accel-config config-engine dsa2/engine2.3 --group-id=3
# sudo accel-config enable-device dsa2
# sudo accel-config enable-wq dsa2/wq2.0
# sudo accel-config enable-wq dsa2/wq2.1
# sudo accel-config enable-wq dsa2/wq2.2
# sudo accel-config enable-wq dsa2/wq2.3

# sudo accel-config config-wq --group-id=0 --mode=shared --wq-size=32 --threshold=31 --type=user --priority=10 --name="app-1" --driver-name="user" --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa3/wq3.0
# sudo accel-config config-wq --group-id=1 --mode=shared --wq-size=32 --threshold=31 --type=user --priority=10 --name="app-1" --driver-name="user" --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa3/wq3.1
# sudo accel-config config-wq --group-id=2 --mode=shared --wq-size=32 --threshold=31 --type=user --priority=10 --name="app-1" --driver-name="user" --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa3/wq3.2
# sudo accel-config config-wq --group-id=3 --mode=shared --wq-size=32 --threshold=31 --type=user --priority=10 --name="app-1" --driver-name="user" --max-transfer-size $((2*1024*1024*1024)) --block-on-fault=1 dsa3/wq3.3
# sudo accel-config config-engine dsa3/engine3.0 --group-id=0
# sudo accel-config config-engine dsa3/engine3.1 --group-id=1
# sudo accel-config config-engine dsa3/engine3.2 --group-id=2
# sudo accel-config config-engine dsa3/engine3.3 --group-id=3
# sudo accel-config enable-device dsa3
# sudo accel-config enable-wq dsa3/wq3.0
# sudo accel-config enable-wq dsa3/wq3.1
# sudo accel-config enable-wq dsa3/wq3.2
# sudo accel-config enable-wq dsa3/wq3.3

./set_group_perms.sh

sudo rdmsr 0xc8b
