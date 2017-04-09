COMPILEFLAGS = -std=c++11 -I"./include" -O3
COMPILECMD = g++ -c $(COMPILEFLAGS)
LINKFLAGS =   -L"./lib"  
LINKCMD = g++ $(LINKFLAGS) $(OBJS)  $(LIBRARIES) -o $(TARGET)
LIBRARIES = -lSDL2 -lSDL2main -lGLU -lGL -lSOIL -lglut -lSDL2_mixer
#LIBRARIES = -lSDL2 -lSDL2main -lSDL2_mixer -lfreeglut -lSOIL -lopengl32 -lglu32 
OBJS = Terrain.o drawFunc.o suppliedGlutFuncs.o
TARGET = run.exe

main:   $(OBJS)
		$(LINKCMD)

Terrain.o:  Terrain.h Terrain.cpp
	$(COMPILECMD) Terrain.cpp 

drawFunc.o: drawFunc.cpp
	$(COMPILECMD) drawFunc.cpp

suppliedGlutFuncs.o: suppliedGlutFuncs.cpp suppliedGlutFuncs.h
	$(COMPILECMD) suppliedGlutFuncs.cpp

Mesh.o: Mesh.cpp Mesh.h
	$(COMPILECMD) Mesh.cpp

Item.o: Item.h Item.cpp
	$(COMPILECMD) Item.cpp 

Weapon.o : Weapon.h Weapon.cpp
	$(COMPILECMD) Weapon.cpp 

Bullet.o : Bullet.h Bullet.cpp
	$(COMPILECMD) Bullet.cpp 

Portal.o: Portal.h Portal.cpp
	$(COMPILECMD) Portal.cpp 