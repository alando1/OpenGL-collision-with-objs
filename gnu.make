main:   Terrain.o drawFunc.o suppliedGlutFuncs.o
	g++ -std=c++11 Terrain.o drawFunc.o suppliedGlutFuncs.o -o main

# Vector.o:	Vector.h Vector.cpp
# 	g++ -c Vector.cpp

Terrain.o:  Terrain.h Terrain.cpp
	g++ -std=c++11 -c Terrain.cpp 

drawFunc.o:   drawFunc.h drawFunc.cpp
	g++ -std=c++11 -c drawFunc.cpp

suppliedGlutFuncs.o: suppliedGlutFuncs.cpp suppliedGlutFuncs.h
	g++ -std=c++11 -c suppliedGlutFuncs.cpp

