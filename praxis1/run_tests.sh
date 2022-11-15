#!/bin/bash

echo "SCRIPT::CMAKE"
cmake -S . -B build
cd ./build
echo "SCRIPT::Make"
make
cd ..
echo

echo "SCRIPT::Testing"
for file in ./test/*.py; do
	echo "SCRIPT::Running \"$file\""
	python3 $file --port $1 ./build/webserver
	echo
done
