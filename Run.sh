#!/bin/bash
g++ main.cpp -std=c++17 -o output `pkg-config --cflags --libs opencv`
echo "Running Final Work"
./output