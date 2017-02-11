@echo off
SET mingwdir="C:\Program Files (x86)\CodeBlocks\MinGW\bin\"
%mingwdir%g++ main.c -o WaterBuckets.exe
%mingwdir%g++ --version