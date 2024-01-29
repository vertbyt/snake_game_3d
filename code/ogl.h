#ifndef OGL_H
#define OGL_H

#define GL_DEF_PROC(n, nc) typedef PFN##nc##PROC n##_func_ptr;  n##_func_ptr n = NULL

//
// Opengl procs definition
//
#define X(n, fp) GL_DEF_PROC(n, fp)

// shader 
X(glCreateShader,      GLCREATESHADER);
X(glDeleteShader,      GLDELETESHADER);
X(glShaderSource,      GLSHADERSOURCE);
X(glCompileShader,     GLCOMPILESHADER);
X(glGetShaderInfoLog,  GLGETSHADERINFOLOG);
X(glGetShaderiv,       GLGETSHADERIV);

X(glCreateProgram, GLCREATEPROGRAM);
X(glDeleteProgram, GLDELETEPROGRAM);
X(glUseProgram,    GLDELETEPROGRAM);

X(glAttachShader,      GLATTACHSHADER);
X(glLinkProgram,       GLLINKPROGRAM);
X(glGetProgramiv,      GLGETPROGRAMIV);
X(glGetProgramInfoLog, GLGETPROGRAMINFOLOG);

X(glGetUniformLocation, GLGETUNIFORMLOCATION);
X(glUniform1i, GLUNIFORM1I);
X(glUniform1iv, GLUNIFORM1IV);

X(glUniform1f, GLUNIFORM1F);
X(glUniform1fv, GLUNIFORM1FV);


X(glUniform2fv, GLUNIFORM2FV);

X(glUniform3f, GLUNIFORM3F);

X(glUniform4fv, GLUNIFORM4FV);
X(glUniform4f, GLUNIFORM4F);
X(glUniformMatrix4fv, GLUNIFORMMATRIX4FV);

//gl buffers
X(glGenVertexArrays, GLGENVERTEXARRAYS);
X(glBindVertexArray, GLBINDVERTEXARRAY);
X(glVertexAttribPointer, GLVERTEXATTRIBPOINTER);
X(glVertexAttribIPointer, GLVERTEXATTRIBIPOINTER);
X(glEnableVertexAttribArray, GLENABLEVERTEXATTRIBARRAY);

X(glGenBuffers, GLGENBUFFERS);
X(glBindBuffer, GLBINDBUFFER);
X(glBufferData, GLBUFFERDATA);
X(glBufferSubData, GLBUFFERSUBDATA);

X(glGenerateMipmap, GLGENERATEMIPMAP);

// texture
// Super weird hack, for some reason glActiveTexture is unresolved, and this is the fix.
PFNGLACTIVETEXTUREPROC glActiveTextureBRUH = NULL;
#define glActiveTexture glActiveTextureBRUH
#undef X

//
// Opengl proc loading
//
void load_gl_procs(void* (*load)(const char* str));


#endif

