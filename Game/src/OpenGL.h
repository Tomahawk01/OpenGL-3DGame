#pragma once

#include <Windows.h>
#include <gl/GL.h>

#include "opengl/glext.h"
#include "opengl/wglext.h"

#define FOR_OPENGL_FUNCTIONS(DO) \
	DO(::PFNGLCREATESHADERPROC, glCreateShader) \
	DO(::PFNGLDELETESHADERPROC, glDeleteShader) \
	DO(::PFNGLSHADERSOURCEPROC, glShaderSource) \
	DO(::PFNGLCOMPILESHADERPROC, glCompileShader) \
	DO(::PFNGLGETSHADERIVPROC, glGetShaderiv) \
	DO(::PFNGLCREATEPROGRAMPROC, glCreateProgram) \
	DO(::PFNGLDELETEPROGRAMPROC, glDeleteProgram) \
	DO(::PFNGLATTACHSHADERPROC, glAttachShader) \
	DO(::PFNGLLINKPROGRAMPROC, glLinkProgram) \
	DO(::PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays) \
	DO(::PFNGLGENBUFFERSPROC, glGenBuffers) \
	DO(::PFNGLBINDVERTEXARRAYPROC, glBindVertexArray) \
	DO(::PFNGLBINDBUFFERPROC, glBindBuffer) \
	DO(::PFNGLBUFFERDATAPROC, glBufferData) \
	DO(::PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer) \
	DO(::PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray) \
	DO(::PFNGLUSEPROGRAMPROC, glUseProgram) \
	DO(::PFNGLDRAWARRAYSEXTPROC, glDrawArraysEXT)

#if !defined(NO_EXTERN)
#define DO_EXTERN(TYPE, NAME) extern TYPE NAME;
FOR_OPENGL_FUNCTIONS(DO_EXTERN)
#else
#define DO_DEFINE(TYPE, NAME) TYPE NAME;
FOR_OPENGL_FUNCTIONS(DO_DEFINE)
#endif
