COMPILEFLAGS = -std=c++11 -I"./include" -O3
COMPILECMD = g++ -c $(COMPILEFLAGS)
LINKFLAGS =   
LINKCMD = g++ $(LINKFLAGS) $(OBJS)  $(LIBRARIES) -o $(TARGET)
LIBRARIES = -lSDL2main -lSDL2 -lGLU -lGL -lSOIL -lglut -lSDL2_mixer
#LIBRARIES = -lSDL2 -lSDL2main -lSDL2_mixer -lfreeglut -lSOIL -lopengl32 -lglu32 
OBJS = Camera.o Player.o Mesh.o Item.o Weapon.o Bullet.o Portal.o Terrain.o SuppliedGlutFuncs.o DrawFunc.o 256main.o
TARGET = run

main:   $(OBJS)
		$(LINKCMD)

clean:
	rm *.o

Camera.o: Camera.h Camera.cpp
	$(COMPILECMD) Camera.cpp

Player.o: Player.h Player.cpp
	$(COMPILECMD) Player.cpp

Mesh.o: Mesh.h Mesh.cpp
	$(COMPILECMD) Mesh.cpp

Item.o: Item.h Item.cpp
	$(COMPILECMD) Item.cpp

Weapon.o: Weapon.h Weapon.cpp
	$(COMPILECMD) Weapon.cpp

Bullet.o: Bullet.h Bullet.cpp
	$(COMPILECMD) Bullet.cpp

Portal.o: Portal.h Portal.cpp
	$(COMPILECMD) Portal.cpp

Terrain.o:  Terrain.h Terrain.cpp
	$(COMPILECMD) Terrain.cpp 

SuppliedGlutFuncs.o: SuppliedGlutFuncs.cpp SuppliedGlutFuncs.h
	$(COMPILECMD) SuppliedGlutFuncs.cpp

DrawFunc.o: DrawFunc.cpp
	$(COMPILECMD) DrawFunc.cpp

256main.o: 256main.cpp
	$(COMPILECMD) 256main.cpp