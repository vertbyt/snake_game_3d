
#ifndef MESH_H
#define MESH_H

namespace mesh {

struct Mesh_Vertex {
	Vec3 position;
	Vec3 normal;
	Vec2 uv;
	u32 color;
};

struct Mesh_Partition {
	u32 vertex_at, index_at;
	u32 vertices_count, indices_count;
};

struct Mesh {
	u32 vao, vbo, ebo;
	
	u32 vertices_count;
	Mesh_Vertex* vertices;
	
	u32 indices_count;
	u16 *indices;
	
	u32 partitions_count;
	Mesh_Partition* partitions;
	
	Vec3 min_bounds;
	Vec3 max_bounds;
};

Mesh load(Str8 path);
void scale_and_center(Mesh mesh, f32 scale, f32* scale_out, Vec3* adj_out);

}

#endif


