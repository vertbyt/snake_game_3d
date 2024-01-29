
@vertex
#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 uv;
layout (location = 3) in uint color;

uniform mat4 model, view, proj;

out vec3 frag_pos;
out vec3 frag_norm;
out vec2 frag_uv;
out vec4 frag_color;

void main(){
	gl_Position = proj*view*model*vec4(pos, 1.0f);
	
	uint a32 = (color >> 24) & 0xFFu;
	uint r32 = (color >> 16) & 0xFFu;
	uint g32 = (color >> 8)  & 0xFFu;
	uint b32 = (color >> 0)  & 0xFFu;
	
	frag_pos   = vec3(view*model*vec4(pos, 1.0f));
	frag_norm  = normalize( mat3(transpose(inverse(view*model)))*norm );
	frag_uv    = uv;
	frag_color = vec4(float(r32)/255.0f, float(g32)/255.0f, float(b32)/255.0f, float(a32)/255.0f);
}

@fragment
#version 330

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shine;
};

struct Light {
	vec3 dir;
	vec3 ambient, diffuse, specular;
};

uniform Material material;
uniform Light light;

uniform mat4 view;

in vec3 frag_pos;
in vec3 frag_norm;
in vec2 frag_uv;
in vec4 frag_color;

out vec4 final_color;

void main(){
	vec3 view_dir = normalize(frag_pos);
	vec3 light_dir = normalize(mat3(view)*light.dir); // @Slow ???
	
	vec4 diffuse_sample  = texture(material.diffuse, frag_uv);
	vec4 specular_sample = texture(material.specular, frag_uv);
	float shine = material.shine;
	
	// diffuse factor
	float df = max(dot(-light_dir, frag_norm), 0.0f);
	float sf = max(dot(-view_dir, reflect(light_dir, frag_norm)), 0.0f);
	sf = sf*pow(sf, shine);
	
	vec3 ambient  = light.ambient*vec3(diffuse_sample*frag_color);
	vec3 diffuse  = light.diffuse*vec3(diffuse_sample*frag_color*df);
	vec3 specular = light.specular*vec3(specular_sample*frag_color*sf);
	
	vec3 phong_color = ambient + specular + diffuse;
	
	final_color = vec4(phong_color, diffuse_sample.a);
}
