#!/bin/sh

type=$1

cd src

if [ "$type" = "--server" ]
then
	make clean
	make all || exit
	echo "Starting server..."
	./server
else
	make all || exit
	echo "Starting client..."
	./client
fi
