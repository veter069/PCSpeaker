# compiler:
CC=g++
# flags:
FLG=-std=c++11

nothing:

cpuports:
	$(CC) $(FLG) CPUports/CPUports.h -o CPUports/CPUports.o

pitctl:
	$(CC) $(FLG) -I CPUports PITctl/PITctl.h -o PITctl/PITctl.o

benchmark: 
	$(CC) $(FLG) -I PITctl -I CPUports CPUbenchmark/benchmark.cpp -o build/benchmark

pcsplayer: 
	$(CC) $(FLG) PCSplayer/PCSplayer.cpp -o PCSplayer/build/pcsplayer
