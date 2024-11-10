#!/bin/sh

# Update and install dependencies
sudo apt-get update
sudo apt-get -y upgrade
sudo apt-get -y dist-upgrade
sudo apt-get install -y python3-lldb-18 libncurses5-dev libncursesw5-dev libspdlog-dev

# Create symbolic link for lldb
sudo ln -s /usr/lib/llvm-*/lib/python3.11/dist-packages/lldb/* /usr/lib/python3/dist-packages/lldb