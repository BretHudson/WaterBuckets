@echo off
SET mingwdir="C:\Program Files (x86)\CodeBlocks\MinGW\bin\"
%mingwdir%g++ main.c -o WaterBucket.exe
%mingwdir%g++ --version