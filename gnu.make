main:   Terrain.o drawFunc.o suppliedGlutFuncs.o
	g++ Terrain.o drawFunc.o suppliedGlutFuncs.o -o main

# Vector.o:	Vector.h Vector.cpp
# 	g++ -c Vector.cpp

Terrain.o:  Terrain.h Terrain.cpp
	g++ -c Terrain.cpp 

drawFunc.o:   drawFunc.h drawFunc.cpp
	g++ -c drawFunc.cpp

suppliedGlutFuncs.o: suppliedGlutFuncs.cpp suppliedGlutFuncs.h
	g++ -c suppliedGlutFuncs.cpp

