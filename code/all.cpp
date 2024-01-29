
// ----------------
// INCLUDE FILES
//

// 3rd party. 
// @note: Avalible to everybody.

#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_opengl_glext.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include"stb_truetype.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "ogl.h"
#include "ogl.cpp"


// @note: Can't put this header on top of the headers, because we get a werid compilation error in font.cpp.
#include "windows.h" // for file stuff

// fh files. These have no namespace. They are like the language features I added to C++.
// @note: Avalible to everbody.
#include "fh_base.h"
#include "fh_alloc.h"
#include "fh_array.h"
#include "fh_str.h"
#include "fh_math.h"

// platform provided stuff
#include "window.h"
#include "input.h"
#include "file_io.h"

// game asset types
#include "texture.h"
#include "font.h"
#include "shader.h"
#include "mesh.h"

// game systems
#include "game_assets.h"
#include "game_memory.h"

// game stuff
#include "entity.h"
#include "level.h"

// rendering
#include "light.h"
#include "material.h"
#include "draw_3d.h"
#include "draw_3d_immediate.h"
#include "draw_2d_immediate.h"
#include "draw.h"

// other systems
#include "command_line.h"

// game, editor, engine
#include "game.h"
#include "editor.h"
#include "engine.h"


// -----------------
// CPP FILES
//
#include "fh_math.cpp"
#include "fh_str.cpp"
#include "fh_alloc.cpp"
#include "fh_array.cpp"

#include "file_io.cpp"

#include "texture.cpp"
#include "font.cpp"
#include "shader.cpp"
#include "mesh.cpp"

#include "game_assets.cpp"
#include "game_memory.cpp"

#include "entity.cpp"
#include "level.cpp"

#include "draw_3d.cpp"
#include "draw_3d_immediate.cpp"
#include "draw_2d_immediate.cpp"
#include "draw.cpp"

#include "command_line.cpp"

#include "game.cpp"
#include "editor.cpp"
#include "engine.cpp"
#include "entry.cpp"




