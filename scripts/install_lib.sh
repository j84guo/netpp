#!/bin/bash

# Compile position independent code
mkdir -p bin/
g++ -c -Wall -std=c++17 -fpic src/*.cpp

# Build share library and copy to system location with correct access
# Then delete unneeded object files
g++ -shared -o libnetpp.so *.o
rm *.o
sudo mv libnetpp.so /usr/local/lib
sudo chmod 0755 /usr/local/lib/libnetpp.so

# Refresh ldconfig cache
sudo ldconfig
ldconfig -p | grep netpp

# Put headers in system location
sudo mkdir -p /usr/local/include/netpp
sudo cp src/*.h /usr/local/include/netpp

