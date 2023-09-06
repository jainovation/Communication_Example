#!/bin/sh -e

echo "build start"

g++ -o server server.cpp
g++ -o client client.cpp

echo "build finish"