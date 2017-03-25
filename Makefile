CC=g++
CFLAGS=-Wall -std=c++11 
INC=-ISOIL/ -ISDL/
LIB=-lGLU -lGL -lSOIL -lglut -lSDL2 -lSDL_mixer
BIN=
SRC=Terrain.cpp drawFunc.cpp Mesh.cpp Item.cpp Weapon.cpp Bullet.cpp Portal.cpp suppliedGlutFuncs.cpp 256main.cpp
OBJ=$(SRC:.cpp=.o)
EXEC=run
LDFLAGS=


all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CLFAGS) $(INC) -o $@ $^ $(LIB)
	
.cpp.o:
	$(CC) $(CFLAGS) $(INC) -c -o $@ $^

clean:
	rm -f $(OBJ) $(EXEC)