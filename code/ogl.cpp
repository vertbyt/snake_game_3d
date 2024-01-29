
#define GL_LOAD_PROC(n, load_proc) n = (n##_func_ptr)load_proc(#n)

// *******************
// Opengl proc loading

void load_gl_procs(void* (*load)(const char* str)) {

#define X(n) GL_LOAD_PROC(n, load)

  // shader program
  X(glShaderSource);
  X(glCompileShader);
  X(glGetShaderInfoLog);
  X(glGetShaderiv);
  
  X(glAttachShader);
  X(glLinkProgram);
  X(glGetProgramiv);
  X(glGetProgramInfoLog);
      
  X(glCreateShader);
  X(glDeleteShader);
  X(glCreateProgram);
  X(glDeleteProgram);
  X(glUseProgram);
  
  X(glGetUniformLocation);
  X(glUniform1i);
  X(glUniform1iv);
  X(glUniform1f);
  X(glUniform1fv);
  X(glUniform2fv);
  
  X(glUniform3f);
  
  X(glUniform4fv);
  X(glUniform4f);
  
  X(glUniformMatrix4fv);
  
  // gl buffers
  X(glGenVertexArrays);
  X(glBindVertexArray);
  X(glVertexAttribPointer);
  X(glVertexAttribIPointer);
  X(glEnableVertexAttribArray);
  X(glGenBuffers);
  X(glBindBuffer);
  X(glBufferData);
  X(glBufferSubData);
  
  X(glGenerateMipmap);

  // texture
  glActiveTextureBRUH = (PFNGLACTIVETEXTUREPROC)load("glActiveTexture");
  
#undef X
}