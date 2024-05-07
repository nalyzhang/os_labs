#!/bin/bash
g++ -c lab4a.cpp
g++ -o lab4a lab4a.o -lpthread
g++ -c lab4b.cpp
g++ -o lab4b lab4b.o -lpthread
export PATH="$PWD:$PATH"
./lab4b "$@"
