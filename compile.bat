g++ -std=c++11 Camera.cpp Player.cpp Mesh.cpp Item.cpp Weapon.cpp Bullet.cpp Portal.cpp Terrain.cpp suppliedGlutFuncs.cpp DrawFunc.cpp 256main.cpp -o run -O3 -I"./include" -L"./lib"  -lSDL2 -lSDL2main -lSDL2_mixer -lfreeglut -lSOIL -lopengl32 -lglu32 -Wall -Wno-strict-aliasing -Wno-comment

@echo off
echo(

