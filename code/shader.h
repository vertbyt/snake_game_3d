
#ifndef SHADER_H
#define SHADER_H


namespace shader {

enum Shader_Type {
  Shader_Type_None, 
  
  Shader_Type_Vertex,
  Shader_Type_Fragment,
  Shader_Type_Program,
  
  Shader_Type_Count
};

struct Shader {
  u32 id;
  Shader_Type type;
};

Shader vs_fs(Str8 vs, Str8 fs);
Shader load(Str8 path);
void unload(Shader shader);

void bind(Shader shader);
void unbind(void);
b32 is_current_bound(Shader shader);

s32 uloc(Shader shader, char* name);

void set_int(s32 loc, s32 value);
void set_ints(s32 loc, s32 count, s32* ints);

void set_float(s32 loc, f32 value);
void set_floats(s32 loc, s32 count, f32* floats);

void set_vec3(s32 loc, f32 x, f32 y, f32 z);
void set_vec3(s32 loc, Vec3 v);

void set_vec4(s32 loc, f32 x, f32 y, f32 z, f32 w);

void set_mat4(s32 loc, f32* m);

}

#endif