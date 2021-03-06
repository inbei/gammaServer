#!/usr/bin/env bash

killall gamma

rm -rf node*

# set the running environment
path=$(pwd)
export LD_LIBRARY_PATH=$path

sleep 1

nohup ./gamma config1.ini -print >> node1.log 2>&1 &

sleep 1

nohup ./gamma config2.ini -print >> node2.log 2>&1 &
