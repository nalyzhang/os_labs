#!/bin/bash
touch file.txt
g++ -c lab9a.cpp
g++ -o lab9a lab9a.o -lcap
g++ -c lab9b.cpp
g++ -o lab9b lab9b.o -lcap
