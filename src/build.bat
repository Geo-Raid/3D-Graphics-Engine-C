@echo off

g++ main.cpp -s -O -o ../bin/Engine.exe -lgdi32 -luser32 -lwinmm -Wno-deprecated

pause