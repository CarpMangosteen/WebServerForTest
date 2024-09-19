#!/bin/bash

# Step 1: 使用 make 编译项目
make

# Step 2: 运行 ./Server
./Server &  # 将 Server 放入后台运行

# Step 3: 在新终端窗口中运行 ./Client
gnome-terminal -- bash -c "./Client; exec bash"
