@echo off

gcc main.c -o ../bin/Engine.exe -lgdi32 -luser32 -lwinmm

pause

start ../bin/Engine.exe