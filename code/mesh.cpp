
namespace mesh {

Mesh load(Str8 file_path) {
	
	u32 import_flags = aiProcess_Triangulate|aiProcess_JoinIdenticalVertices;
	import_flags    |= aiProcess_FlipUVs|aiProcess_MakeLeftHanded;
	
	char file_path_cstr[512];
	str8_copy_to_cstr(file_path, file_path_cstr, sizeof(file_path_cstr));
	const aiScene* scene = aiImportFile(file_path_cstr, import_flags);
	Assert(scene);
	
	u32 vertices_count = 0;
	u32 indices_count = 0;
	u32 meshes_count = scene->mNumMeshes;
	
	// Mesh vertices and indcies count
	Loop(i, meshes_count) {
		aiMesh* mesh = scene->mMeshes[i];
		vertices_count += mesh->mNumVertices;
		indices_count += mesh->mNumFaces*3; // @Note: We only deal with triangular faces.
	}
	
	Mesh_Vertex*    vertices   = (Mesh_Vertex*)m_alloc(sizeof(Mesh_Vertex)*vertices_count);
	u16*            indices    = (u16*)m_alloc(sizeof(u16)*indices_count);
	Mesh_Partition* partitions = (Mesh_Partition*)m_alloc(sizeof(Mesh_Partition)*meshes_count);
	
	u32 vertex_at = 0;
	u32 index_at = 0;
	
	Vec3 min_bounds = vec3(1);
	Vec3 max_bounds = vec3(-1);
	
	// Individual meshes
	Loop(i, meshes_count) {
		aiMesh* mesh = scene->mMeshes[i];
		partitions[i] = {vertex_at, index_at, mesh->mNumVertices, mesh->mNumFaces*3};
		
		// Color
		aiColor4D color = {};
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		aiGetMaterialColor(material,AI_MATKEY_COLOR_DIFFUSE,&color);
		u32 color_u32 = pack_color_argb(color.r, color.g, color.b, color.a);
		
		// Vertices
		Loop(vi, mesh->mNumVertices) {
			aiVector3D p = mesh->mVertices[vi];
			aiVector3D n = mesh->mNormals[vi];
			
			aiVector3D uv = {1.0f, 1.0f, 1.0f};
			if(mesh->mTextureCoords[0]) uv = mesh->mTextureCoords[0][vi];
			
			Mesh_Vertex* v = &vertices[vertex_at];
			vertex_at += 1;
			
			v->position = {p.x, p.y, p.z};
			v->normal   = {n.x, n.y, n.z};
			v->uv       = {uv.x, uv.y};
			v->color    = color_u32;
			
			min_bounds = {Min(min_bounds.x, p.x), Min(min_bounds.y, p.y), Min(min_bounds.z, p.z)};
			max_bounds = {Max(max_bounds.x, p.x), Max(max_bounds.y, p.y), Max(max_bounds.z, p.z)};
		}
		
		// Faces
		Loop(fi, mesh->mNumFaces) {
			aiFace face = mesh->mFaces[fi];
			Loop(fii, face.mNumIndices) {
				indices[index_at] = face.mIndices[fii];
				
				index_at += 1;
			}
		}
	}
	
	u16* indices_relative_to_mesh = (u16*)m_alloc(sizeof(u16)*indices_count);
	s32 indices_relative_at = 0;
	Loop(pi, meshes_count) {
		u16* indices_region = &indices[partitions[pi].index_at];
		Loop(ii, partitions[pi].indices_count) {
			indices_relative_to_mesh[indices_relative_at] = partitions[pi].vertex_at + indices_region[ii];
			indices_relative_at += 1;
		}
	}
	
	aiReleaseImport(scene);
	
	// ogl buffers
	u32 vao, vbo, ebo;
	
    glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	
	// data
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	
	glBufferData(GL_ARRAY_BUFFER, vertices_count*sizeof(Mesh_Vertex), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count*sizeof(u16), indices_relative_to_mesh, GL_STATIC_DRAW);
	
	// vertex attrib
	u32 stride = sizeof(Mesh_Vertex);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)&_Member(Mesh_Vertex, position));
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)&_Member(Mesh_Vertex, normal));
	glEnableVertexAttribArray(1);
	
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)&_Member(Mesh_Vertex, uv));
	glEnableVertexAttribArray(2);
	
	glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, stride, (void*)&_Member(Mesh_Vertex, color));
	glEnableVertexAttribArray(3);
	
	// unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	Mesh loaded_mesh = {};
	
	loaded_mesh.vao = vao;
	loaded_mesh.vbo = vbo;
	loaded_mesh.ebo = ebo;
	
	loaded_mesh.vertices_count   = vertices_count;
	loaded_mesh.indices_count    = indices_count;
	loaded_mesh.partitions_count = meshes_count;
	loaded_mesh.vertices         = vertices;
	loaded_mesh.indices          = indices;
	loaded_mesh.partitions       = partitions;
	loaded_mesh.min_bounds       = min_bounds;
	loaded_mesh.max_bounds       = max_bounds;
	
	return loaded_mesh;
}

void scale_and_center(Mesh mesh, f32 scale, f32* scale_out, Vec3* adj_out) {
	Vec3 dim = mesh.max_bounds - mesh.min_bounds;
	f32 max_dim = Max(dim.x, Max(dim.y, dim.z));
	
	f32 s = (2.0f/max_dim)*scale;
	
	Vec3 adj = -(mesh.min_bounds + dim*0.5f)*s;
	
	*scale_out = s;
	*adj_out = adj;
}

} // namespace mesh

