#!/bin/bash
echo 'rootcling -f Dict.cc  DetInfo.h LinkDef.h'
rootcling -f Dict.cc  DetInfo.h LinkDef.h
echo 'g++ `root-config --libs --cflags` -O3 -fPIC -Wall -o Dict.o -c Dict.cc'
g++ `root-config --libs --cflags` -O3 -fPIC -Wall -o Dict.o -c Dict.cc
echo 'g++ `root-config --libs --cflags` -Wall -g -o deInfo.exe  deInfo.C Dict.o'
g++ `root-config --libs --cflags`  -Wall -g -o deInfo.exe  deInfo.C Dict.o

