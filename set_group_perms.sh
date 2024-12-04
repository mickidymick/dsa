#!/usr/bin/env bash

# sudo chown -R root:DSA /dev/dsa
# sudo chmod -R g+rw /dev/dsa
# sudo chmod -R a+r+w+x /dev/dsa

sudo chown -R root:DSA /dev/dsa/wq0.0
sudo chown -R root:DSA /dev/dsa/wq2.0
sudo chown -R root:DSA /dev/dsa/wq4.0
sudo chown -R root:DSA /dev/dsa/wq6.0

sudo chmod -R a+r+w+x /dev/dsa/wq0.0
sudo chmod -R a+r+w+x /dev/dsa/wq2.0
sudo chmod -R a+r+w+x /dev/dsa/wq4.0
sudo chmod -R a+r+w+x /dev/dsa/wq6.0
