#!/bin/sh

set -e

type=$1
pType=$2

echo "Running $0..."

if [ $type = "server" ]; then
	echo "\nCompiling server..."
	make server
	echo "\nRunning server..."
	../bin/server
else
	echo "\nCompiling peer..."
	make peer
	echo "\nRunning peer..."
	../bin/peer $pType
fi
