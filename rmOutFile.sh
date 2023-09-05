#!/bin/sh -e

FILE=mqProcess/received_data.txt
if [ -f "$FILE" ]; then
    echo "$FILE exists."
    rm mqProcess/received_data.txt
else
    echo "$FILE does not exist."
fi

FILE=tcpProcess/received_data.txt
if [ -f "$FILE" ]; then
    echo "$FILE exists."
    rm tcpProcess/received_data.txt
else
    echo "$FILE does not exist."
fi

FILE=unixDomainProcess/received_data.txt
if [ -f "$FILE" ]; then
    echo "$FILE exists."
    rm unixDomainProcess/received_data.txt
else
    echo "$FILE does not exist."
fi

FILE=udpProcess/received_data.txt
if [ -f "$FILE" ]; then
    echo "$FILE exists."
    rm udpProcess/received_data.txt
else
    echo "$FILE does not exist."
fi

FILE=smProcess/received_data.txt
if [ -f "$FILE" ]; then
    echo "$FILE exists."
    rm smProcess/received_data.txt
else
    echo "$FILE does not exist."
fi

FILE=output/received_data.txt
if [ -f "$FILE" ]; then
    echo "$FILE exists."
    rm output/received_data.txt
else
    echo "$FILE does not exist."
fi
