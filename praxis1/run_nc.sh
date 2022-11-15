#!/bin/bash

echo "SCRIPT::CMAKE"
cmake -S . -B build
cd ./build
echo "SCRIPT::Make"
make
cd ..
echo

echo "SCRIPT::Testing"
./build/webserver $1 > out.txt 2>&1 &
echo "Payload:"
echo "$(cat $2)"
cat $2 | nc -N -C localhost $1
echo "SCRIPT::Done"
cat ./out.txt
