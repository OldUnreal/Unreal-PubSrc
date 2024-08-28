/*=============================================================================
	OpenGL3Funcs.h: OpenGL 3.2 function-declaration macros.

	Created 2010.

	Revision history:

=============================================================================*/

//OpenGL 3.2 functions beyond subset present in OpenGL 1.x
GL3_PROC(void,glMultiDrawArrays,(GLenum, GLint *, GLsizei *, GLsizei))
GL3_PROC(void,glMultiDrawElements,(GLenum, const GLsizei *, GLenum, const GLvoid* *, GLsizei))

GL3_PROC(void,glActiveTexture,(GLenum))
GL3_PROC(void,glSampleCoverage,(GLclampf, GLboolean))
GL3_PROC(void,glCompressedTexImage3D,(GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *))
GL3_PROC(void,glCompressedTexImage2D,(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *))
GL3_PROC(void,glCompressedTexImage1D,(GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const GLvoid *))
GL3_PROC(void,glCompressedTexSubImage3D,(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *))
GL3_PROC(void,glCompressedTexSubImage2D,(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *))
GL3_PROC(void,glCompressedTexSubImage1D,(GLenum, GLint, GLint, GLsizei, GLenum, GLsizei, const GLvoid *))
GL3_PROC(void,glGetCompressedTexImage,(GLenum, GLint, GLvoid *))

GL3_PROC(void,glBlendEquationSeparate,(GLenum, GLenum))
GL3_PROC(void,glDrawBuffers,(GLsizei, const GLenum *))
GL3_PROC(void,glStencilOpSeparate,(GLenum, GLenum, GLenum, GLenum))
GL3_PROC(void,glStencilFuncSeparate,(GLenum, GLenum, GLint, GLuint))
GL3_PROC(void,glStencilMaskSeparate,(GLenum, GLuint))
GL3_PROC(void,glAttachShader,(GLuint, GLuint))
GL3_PROC(void,glBindAttribLocation,(GLuint, GLuint, const GLchar *))
GL3_PROC(void,glCompileShader,(GLuint))
GL3_PROC(GLuint,glCreateProgram,(void))
GL3_PROC(GLuint,glCreateShader,(GLenum))
GL3_PROC(void,glDeleteProgram,(GLuint))
GL3_PROC(void,glDeleteShader,(GLuint))
GL3_PROC(void,glDetachShader,(GLuint, GLuint))
GL3_PROC(void,glDisableVertexAttribArray,(GLuint))
GL3_PROC(void,glEnableVertexAttribArray,(GLuint))
GL3_PROC(void,glGetActiveAttrib,(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *))
GL3_PROC(void,glGetActiveUniform,(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *))
GL3_PROC(void,glGetAttachedShaders,(GLuint, GLsizei, GLsizei *, GLuint *))
GL3_PROC(GLint,glGetAttribLocation,(GLuint, const GLchar *))
GL3_PROC(void,glGetProgramiv,(GLuint, GLenum, GLint *))
GL3_PROC(void,glGetProgramInfoLog,(GLuint, GLsizei, GLsizei *, GLchar *))
GL3_PROC(void,glGetShaderiv,(GLuint, GLenum, GLint *))
GL3_PROC(void,glGetShaderInfoLog,(GLuint, GLsizei, GLsizei *, GLchar *))
GL3_PROC(void,glGetShaderSource,(GLuint, GLsizei, GLsizei *, GLchar *))
GL3_PROC(GLint,glGetUniformLocation,(GLuint, const GLchar *))
GL3_PROC(void,glGetUniformfv,(GLuint, GLint, GLfloat *))
GL3_PROC(void,glGetUniformiv,(GLuint, GLint, GLint *))
GL3_PROC(void,glGetVertexAttribdv,(GLuint, GLenum, GLdouble *))
GL3_PROC(void,glGetVertexAttribfv,(GLuint, GLenum, GLfloat *))
GL3_PROC(void,glGetVertexAttribiv,(GLuint, GLenum, GLint *))
GL3_PROC(void,glGetVertexAttribPointerv,(GLuint, GLenum, GLvoid* *))
GL3_PROC(GLboolean,glIsProgram,(GLuint))
GL3_PROC(GLboolean,glIsShader,(GLuint))
GL3_PROC(void,glLinkProgram,(GLuint))
GL3_PROC(void,glShaderSource,(GLuint, GLsizei, const GLchar* *, const GLint *))
GL3_PROC(void,glUseProgram,(GLuint))
GL3_PROC(void,glUniform1f,(GLint, GLfloat))
GL3_PROC(void,glUniform2f,(GLint, GLfloat, GLfloat))
GL3_PROC(void,glUniform3f,(GLint, GLfloat, GLfloat, GLfloat))
GL3_PROC(void,glUniform4f,(GLint, GLfloat, GLfloat, GLfloat, GLfloat))
GL3_PROC(void,glUniform1i,(GLint, GLint))
GL3_PROC(void,glUniform2i,(GLint, GLint, GLint))
GL3_PROC(void,glUniform3i,(GLint, GLint, GLint, GLint))
GL3_PROC(void,glUniform4i,(GLint, GLint, GLint, GLint, GLint))
GL3_PROC(void,glUniform1fv,(GLint, GLsizei, const GLfloat *))
GL3_PROC(void,glUniform2fv,(GLint, GLsizei, const GLfloat *))
GL3_PROC(void,glUniform3fv,(GLint, GLsizei, const GLfloat *))
GL3_PROC(void,glUniform4fv,(GLint, GLsizei, const GLfloat *))
GL3_PROC(void,glUniform1iv,(GLint, GLsizei, const GLint *))
GL3_PROC(void,glUniform2iv,(GLint, GLsizei, const GLint *))
GL3_PROC(void,glUniform3iv,(GLint, GLsizei, const GLint *))
GL3_PROC(void,glUniform4iv,(GLint, GLsizei, const GLint *))
GL3_PROC(void,glUniformMatrix2fv,(GLint, GLsizei, GLboolean, const GLfloat *))
GL3_PROC(void,glUniformMatrix3fv,(GLint, GLsizei, GLboolean, const GLfloat *))
GL3_PROC(void,glUniformMatrix4fv,(GLint, GLsizei, GLboolean, const GLfloat *))
GL3_PROC(void,glValidateProgram,(GLuint))
GL3_PROC(void,glVertexAttrib1d,(GLuint, GLdouble))
GL3_PROC(void,glVertexAttrib1dv,(GLuint, const GLdouble *))
GL3_PROC(void,glVertexAttrib1f,(GLuint, GLfloat))
GL3_PROC(void,glVertexAttrib1fv,(GLuint, const GLfloat *))
GL3_PROC(void,glVertexAttrib1s,(GLuint, GLshort))
GL3_PROC(void,glVertexAttrib1sv,(GLuint, const GLshort *))
GL3_PROC(void,glVertexAttrib2d,(GLuint, GLdouble, GLdouble))
GL3_PROC(void,glVertexAttrib2dv,(GLuint, const GLdouble *))
GL3_PROC(void,glVertexAttrib2f,(GLuint, GLfloat, GLfloat))
GL3_PROC(void,glVertexAttrib2fv,(GLuint, const GLfloat *))
GL3_PROC(void,glVertexAttrib2s,(GLuint, GLshort, GLshort))
GL3_PROC(void,glVertexAttrib2sv,(GLuint, const GLshort *))
GL3_PROC(void,glVertexAttrib3d,(GLuint, GLdouble, GLdouble, GLdouble))
GL3_PROC(void,glVertexAttrib3dv,(GLuint, const GLdouble *))
GL3_PROC(void,glVertexAttrib3f,(GLuint, GLfloat, GLfloat, GLfloat))
GL3_PROC(void,glVertexAttrib3fv,(GLuint, const GLfloat *))
GL3_PROC(void,glVertexAttrib3s,(GLuint, GLshort, GLshort, GLshort))
GL3_PROC(void,glVertexAttrib3sv,(GLuint, const GLshort *))
GL3_PROC(void,glVertexAttrib4Nbv,(GLuint, const GLbyte *))
GL3_PROC(void,glVertexAttrib4Niv,(GLuint, const GLint *))
GL3_PROC(void,glVertexAttrib4Nsv,(GLuint, const GLshort *))
GL3_PROC(void,glVertexAttrib4Nub,(GLuint, GLubyte, GLubyte, GLubyte, GLubyte))
GL3_PROC(void,glVertexAttrib4Nubv,(GLuint, const GLubyte *))
GL3_PROC(void,glVertexAttrib4Nuiv,(GLuint, const GLuint *))
GL3_PROC(void,glVertexAttrib4Nusv,(GLuint, const GLushort *))
GL3_PROC(void,glVertexAttrib4bv,(GLuint, const GLbyte *))
GL3_PROC(void,glVertexAttrib4d,(GLuint, GLdouble, GLdouble, GLdouble, GLdouble))
GL3_PROC(void,glVertexAttrib4dv,(GLuint, const GLdouble *))
GL3_PROC(void,glVertexAttrib4f,(GLuint, GLfloat, GLfloat, GLfloat, GLfloat))
GL3_PROC(void,glVertexAttrib4fv,(GLuint, const GLfloat *))
GL3_PROC(void,glVertexAttrib4iv,(GLuint, const GLint *))
GL3_PROC(void,glVertexAttrib4s,(GLuint, GLshort, GLshort, GLshort, GLshort))
GL3_PROC(void,glVertexAttrib4sv,(GLuint, const GLshort *))
GL3_PROC(void,glVertexAttrib4ubv,(GLuint, const GLubyte *))
GL3_PROC(void,glVertexAttrib4uiv,(GLuint, const GLuint *))
GL3_PROC(void,glVertexAttrib4usv,(GLuint, const GLushort *))
GL3_PROC(void,glVertexAttribPointer,(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *))

GL3_PROC(void,glGenQueries,(GLsizei, GLuint *))
GL3_PROC(void,glDeleteQueries,(GLsizei, const GLuint *))
GL3_PROC(GLboolean,glIsQuery,(GLuint))
GL3_PROC(void,glBeginQuery,(GLenum, GLuint))
GL3_PROC(void,glEndQuery,(GLenum))
GL3_PROC(void,glGetQueryiv,(GLenum, GLenum, GLint *))
GL3_PROC(void,glGetQueryObjectiv,(GLuint, GLenum, GLint *))
GL3_PROC(void,glGetQueryObjectuiv,(GLuint, GLenum, GLuint *))
GL3_PROC(void,glBindBuffer,(GLenum, GLuint))
GL3_PROC(void,glDeleteBuffers,(GLsizei, const GLuint *))
GL3_PROC(void,glGenBuffers,(GLsizei, GLuint *))
GL3_PROC(GLboolean,glIsBuffer,(GLuint))
GL3_PROC(void,glBufferData,(GLenum, GLsizeiptr, const GLvoid *, GLenum))
GL3_PROC(void,glBufferSubData,(GLenum, GLintptr, GLsizeiptr, const GLvoid *))
GL3_PROC(void,glGetBufferSubData,(GLenum, GLintptr, GLsizeiptr, GLvoid *))
GL3_PROC(GLvoid*,glMapBuffer,(GLenum, GLenum))
GL3_PROC(GLboolean,glUnmapBuffer,(GLenum))
GL3_PROC(void,glGetBufferParameteriv,(GLenum, GLenum, GLint *))
GL3_PROC(void,glGetBufferPointerv,(GLenum, GLenum, GLvoid* *))

GL3_PROC(void,glBindVertexArray,(GLuint))
GL3_PROC(void,glDeleteVertexArrays,(GLsizei, const GLuint *))
GL3_PROC(void,glGenVertexArrays,(GLsizei, GLuint *))
GL3_PROC(GLboolean,glIsVertexArray,(GLuint))

GL3_PROC(void,glGetUniformIndices,(GLuint, GLsizei, const GLchar* *, GLuint *))
GL3_PROC(void,glGetActiveUniformsiv,(GLuint, GLsizei, const GLuint *, GLenum, GLint *))
GL3_PROC(void,glGetActiveUniformName,(GLuint, GLuint, GLsizei, GLsizei *, GLchar *))
GL3_PROC(GLuint,glGetUniformBlockIndex,(GLuint, const GLchar *))
GL3_PROC(void,glGetActiveUniformBlockiv,(GLuint, GLuint, GLenum, GLint *))
GL3_PROC(void,glGetActiveUniformBlockName,(GLuint, GLuint, GLsizei, GLsizei *, GLchar *))
GL3_PROC(void,glUniformBlockBinding,(GLuint, GLuint, GLuint))

GL3_PROC(void,glColorMaski,(GLuint, GLboolean, GLboolean, GLboolean, GLboolean))
GL3_PROC(void,glGetBooleani_v,(GLenum, GLuint, GLboolean *))
GL3_PROC(void,glGetIntegeri_v,(GLenum, GLuint, GLint *))
GL3_PROC(void,glEnablei,(GLenum, GLuint))
GL3_PROC(void,glDisablei,(GLenum, GLuint))
GL3_PROC(GLboolean,glIsEnabledi,(GLenum, GLuint))
GL3_PROC(void,glBeginTransformFeedback,(GLenum))
GL3_PROC(void,glEndTransformFeedback,(void))
GL3_PROC(void,glBindBufferRange,(GLenum, GLuint, GLuint, GLintptr, GLsizeiptr))
GL3_PROC(void,glBindBufferBase,(GLenum, GLuint, GLuint))
GL3_PROC(void,glTransformFeedbackVaryings,(GLuint, GLsizei, const GLchar* *, GLenum))
GL3_PROC(void,glGetTransformFeedbackVarying,(GLuint, GLuint, GLsizei, GLsizei *, GLsizei *, GLenum *, GLchar *))
GL3_PROC(void,glClampColor,(GLenum, GLenum))
GL3_PROC(void,glBeginConditionalRender,(GLuint, GLenum))
GL3_PROC(void,glEndConditionalRender,(void))
GL3_PROC(void,glVertexAttribIPointer,(GLuint, GLint, GLenum, GLsizei, const GLvoid *))
GL3_PROC(void,glGetVertexAttribIiv,(GLuint, GLenum, GLint *))
GL3_PROC(void,glGetVertexAttribIuiv,(GLuint, GLenum, GLuint *))
GL3_PROC(void,glVertexAttribI1i,(GLuint, GLint))
GL3_PROC(void,glVertexAttribI2i,(GLuint, GLint, GLint))
GL3_PROC(void,glVertexAttribI3i,(GLuint, GLint, GLint, GLint))
GL3_PROC(void,glVertexAttribI4i,(GLuint, GLint, GLint, GLint, GLint))
GL3_PROC(void,glVertexAttribI1ui,(GLuint, GLuint))
GL3_PROC(void,glVertexAttribI2ui,(GLuint, GLuint, GLuint))
GL3_PROC(void,glVertexAttribI3ui,(GLuint, GLuint, GLuint, GLuint))
GL3_PROC(void,glVertexAttribI4ui,(GLuint, GLuint, GLuint, GLuint, GLuint))
GL3_PROC(void,glVertexAttribI1iv,(GLuint, const GLint *))
GL3_PROC(void,glVertexAttribI2iv,(GLuint, const GLint *))
GL3_PROC(void,glVertexAttribI3iv,(GLuint, const GLint *))
GL3_PROC(void,glVertexAttribI4iv,(GLuint, const GLint *))
GL3_PROC(void,glVertexAttribI1uiv,(GLuint, const GLuint *))
GL3_PROC(void,glVertexAttribI2uiv,(GLuint, const GLuint *))
GL3_PROC(void,glVertexAttribI3uiv,(GLuint, const GLuint *))
GL3_PROC(void,glVertexAttribI4uiv,(GLuint, const GLuint *))
GL3_PROC(void,glVertexAttribI4bv,(GLuint, const GLbyte *))
GL3_PROC(void,glVertexAttribI4sv,(GLuint, const GLshort *))
GL3_PROC(void,glVertexAttribI4ubv,(GLuint, const GLubyte *))
GL3_PROC(void,glVertexAttribI4usv,(GLuint, const GLushort *))
GL3_PROC(void,glGetUniformuiv,(GLuint, GLint, GLuint *))
GL3_PROC(void,glBindFragDataLocation,(GLuint, GLuint, const GLchar *))
GL3_PROC(GLint,glGetFragDataLocation,(GLuint, const GLchar *))
GL3_PROC(void,glUniform1ui,(GLint, GLuint))
GL3_PROC(void,glUniform2ui,(GLint, GLuint, GLuint))
GL3_PROC(void,glUniform3ui,(GLint, GLuint, GLuint, GLuint))
GL3_PROC(void,glUniform4ui,(GLint, GLuint, GLuint, GLuint, GLuint))
GL3_PROC(void,glUniform1uiv,(GLint, GLsizei, const GLuint *))
GL3_PROC(void,glUniform2uiv,(GLint, GLsizei, const GLuint *))
GL3_PROC(void,glUniform3uiv,(GLint, GLsizei, const GLuint *))
GL3_PROC(void,glUniform4uiv,(GLint, GLsizei, const GLuint *))
GL3_PROC(void,glTexParameterIiv,(GLenum, GLenum, const GLint *))
GL3_PROC(void,glTexParameterIuiv,(GLenum, GLenum, const GLuint *))
GL3_PROC(void,glGetTexParameterIiv,(GLenum, GLenum, GLint *))
GL3_PROC(void,glGetTexParameterIuiv,(GLenum, GLenum, GLuint *))
GL3_PROC(void,glClearBufferiv,(GLenum, GLint, const GLint *))
GL3_PROC(void,glClearBufferuiv,(GLenum, GLint, const GLuint *))
GL3_PROC(void,glClearBufferfv,(GLenum, GLint, const GLfloat *))
GL3_PROC(void,glClearBufferfi,(GLenum, GLint, GLfloat, GLint))
GL3_PROC(const GLubyte *,glGetStringi,(GLenum, GLuint))

GL3_PROC(GLvoid*,glMapBufferRange,(GLenum, GLintptr, GLsizeiptr, GLbitfield))
GL3_PROC(void,glFlushMappedBufferRange,(GLenum, GLintptr, GLsizeiptr))

GL3_PROC(void,glBindBufferARB,(GLenum, GLuint))
GL3_PROC(void,glDeleteBuffersARB,(GLsizei, const GLuint *))
GL3_PROC(void,glGenBuffersARB,(GLsizei, GLuint *))
GL3_PROC(void,glBufferDataARB,(GLenum, GLsizeiptr, const GLvoid *, GLenum))
GL3_PROC(GLhandleARB,glCreateShaderObjectARB,(GLenum))
GL3_PROC(void,glShaderSourceARB,(GLhandleARB, GLsizei, const GLcharARB* *, const GLint *))
GL3_PROC(void,glCompileShaderARB,(GLhandleARB))
GL3_PROC(GLhandleARB,glCreateProgramObjectARB,(void))
GL3_PROC(void,glAttachObjectARB,(GLhandleARB, GLhandleARB))
GL3_PROC(void,glLinkProgramARB,(GLhandleARB))
GL3_PROC(void,glUseProgramObjectARB,(GLhandleARB))

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/