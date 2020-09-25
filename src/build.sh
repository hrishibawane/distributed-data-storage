#!/bin/sh

type=$1

echo "Running $0..."

if [ "$type" = "server" ]; then
	echo "Compiling server..."
	make server
	echo "Running server..."
	../bin/server
else
	echo "Compiling peer..."
	make peer
	echo "Running peer..."
	../bin/peer
fi
