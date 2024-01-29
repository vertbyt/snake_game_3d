@echo off

set ADD_INC=/I ../sdl2/include/ /I ../stb/ /I ../assimp/include/
set CL_FLAGS=-Zi -Od -nologo -c  %ADD_INC% -F4194304

set LINK_FLAGS=-nologo -debug -out:"game_3d.exe" -subsystem:console -STACK:4194304

set LIBS_X64= ../assimp/assimp-vc143-mtd.lib
set LIBS_X64=%LIBS_X64% ../sdl2/lib/x64/SDL2main.lib ../sdl2/lib/x64/SDL2.lib shell32.lib opengl32.lib

cd ../build

REM **********
REM Compiling

set SRC_FILES=
set SRC_FILES=%SRC_FILES% ../code/fh_base.cpp ../code/fh_math.cpp ../code/fh_alloc.cpp
set SRC_FILES=%SRC_FILES% ../code/fh_str.cpp ../code/fh_array.cpp


set SRC_FILEs=%SRC_FILES% ../code/file_io.cpp
set SRC_FILEs=%SRC_FILES% ../code/font.cpp ../code/mesh.cpp ../code/shader.cpp ../code/texture.cpp 
set SRC_FILES=%SRC_FILES% ../code/command_line.cpp

set SRC_FILEs=%SRC_FILES% ../code/ogl.cpp ../code/draw_3d_immediate.cpp ../code/draw_2d_immediate.cpp
set SRC_FILEs=%SRC_FILES% ../code/draw_3d.cpp ../code/draw.cpp

set SRC_FILES=%SRC_FILES% ../code/game_memory.cpp  ../code/game_assets.cpp
set SRC_FILES=%SRC_FILES% ../code/game.cpp 

set SRC_FILES=%SRC_FILES% ../code/entity.cpp ../code/level.cpp ../code/editor.cpp
set SRC_FILES=%SRC_FILES% ../code/engine.cpp ../code/entry.cpp

cl %CL_FLAGS% %SRC_FILES%


REM **********
REM Linking

set OBJ_FILES=
set OBJ_FILES=%OBJ_FILES% fh_base.obj fh_math.obj fh_alloc.obj fh_str.obj fh_array.obj

set OBJ_FILES=%OBJ_FILES% file_io.obj

set OBJ_FILES=%OBJ_FILES% game_memory.obj game_assets.obj font.obj
set OBJ_FILES=%OBJ_FILES% ogl.obj shader.obj texture.obj mesh.obj 
set OBJ_FILES=%OBJ_FILES% draw_3d.obj draw_3d_immediate.obj draw_2d_immediate.obj draw.obj

set OBJ_FILES=%OBJ_FILES% entity.obj level.obj editor.obj command_line.obj

set OBJ_FILES=%OBJ_FILES% game.obj 

set OBJ_FILES=%OBJ_FILES% engine.obj entry.obj

link %LINK_FLAGS% %OBJ_FILES% %LIBS_X64% 

cd ../code
  