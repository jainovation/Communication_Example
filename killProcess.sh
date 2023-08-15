#!/bin/bash

var1=$(ps -ef | grep 'tcpTask')
var2=$(ps -ef | grep 'unixDomainTask')
var3=$(ps -ef | grep 'udpTask')
var4=$(ps -ef | grep 'sharedMemoryTask')
var5=$(ps -ef | grep 'output')

echo process info: ${var1}
echo process info: ${var2}
echo process info: ${var3}
echo process info: ${var4}
echo process info: ${var5}

get_pid=$(echo $var1} | cut -d " " -f2)

if [ -n "${get_pid}" ]
then
    result=$(kill -9 ${get_pid})
    echo process is killed.
else
    echo running process not found.
fi

get_pid=$(echo $var2} | cut -d " " -f2)

if [ -n "${get_pid}" ]
then
    result=$(kill -9 ${get_pid})
    echo process is killed.
else
    echo running process not found.
fi

get_pid=$(echo $var3} | cut -d " " -f2)

if [ -n "${get_pid}" ]
then
    result=$(kill -9 ${get_pid})
    echo process is killed.
else
    echo running process not found.
fi

get_pid=$(echo $var4} | cut -d " " -f2)

if [ -n "${get_pid}" ]
then
    result=$(kill -9 ${get_pid})
    echo process is killed.
else
    echo running process not found.
fi

get_pid=$(echo $var5} | cut -d " " -f2)

if [ -n "${get_pid}" ]
then
    result=$(kill -9 ${get_pid})
    echo process is killed.
else
    echo running process not found.
fi

cd /tmp
rm my_unix_socket