#!/bin/bash
g++ -c lab8_server.cpp
g++ -o lab8_server lab8_server.o -lpthread
g++ -c lab8_client.cpp
g++ -o lab8_client lab8_client.o -lpthread