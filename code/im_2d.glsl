
@vertex
#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
layout (location = 2) in uint color;

uniform mat4 proj;

out vec2 frag_uv;
out vec4 frag_color;

void main(){
	gl_Position = proj*vec4(pos, 1.0f);
	
	uint a32 = (color >> 24) & 0xFFu;
	uint r32 = (color >> 16) & 0xFFu;
	uint g32 = (color >> 8)  & 0xFFu;
	uint b32 = (color >> 0)  & 0xFFu;
	
	frag_uv    = uv;
	frag_color = vec4(float(r32)/255.0f, float(g32)/255.0f, float(b32)/255.0f, float(a32)/255.0f);
}

@fragment
#version 330

uniform sampler2D the_texture;

in vec2 frag_uv;
in vec4 frag_color;

out vec4 final_color;

void main(){
	final_color = texture(the_texture, frag_uv)*frag_color;
}