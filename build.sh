#!/bin/sh -e

echo "build start"

g++ -o mqProcess/mqTask mqProcess/messageQueue.cpp
g++ -o tcpProcess/tcpTask tcpProcess/tcpSocket.cpp
g++ -o unixDomainProcess/unixDomainTask unixDomainProcess/unixDomainSocket.cpp
g++ -o udpProcess/udpTask udpProcess/udpProcess.cpp
g++ -o smProcess/sharedMemoryTask smProcess/sharedMemory.cpp
g++ -o output/output output/output.cpp

echo "build finish"
