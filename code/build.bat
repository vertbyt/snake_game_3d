@echo off

REM BUILD YO

set ADD_INC=/I ../sdl2/include/ /I ../stb/ /I ../assimp/include/
set CL_FLAGS=-Zi -Od -nologo -c  %ADD_INC% -F4194304 -std:c++14

set LINK_FLAGS=-nologo -debug -out:"game_3d.exe" -subsystem:console -STACK:4194304

set LIBS_X64= ../assimp/assimp-vc143-mtd.lib
set LIBS_X64=%LIBS_X64% ../sdl2/lib/x64/SDL2main.lib ../sdl2/lib/x64/SDL2.lib shell32.lib opengl32.lib

cd ../build
cl %CL_FLAGS% ../code/all.cpp
link %LINK_FLAGS% all.obj %LIBS_X64% 
cd ../code
  