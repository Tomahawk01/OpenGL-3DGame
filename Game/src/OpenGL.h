#pragma once

#include <Windows.h>
#include <gl/GL.h>

#include "opengl/glext.h"
#include "opengl/wglext.h"

#define FOR_OPENGL_FUNCTIONS(DO) \
	DO(::PFNGLCREATESHADERPROC, glCreateShader)

#if !defined(NO_EXTERN)
#define DO_EXTERN(TYPE, NAME) extern TYPE NAME;
FOR_OPENGL_FUNCTIONS(DO_EXTERN)
#else
#define DO_DEFINE(TYPE, NAME) TYPE NAME;
FOR_OPENGL_FUNCTIONS(DO_DEFINE)
#endif
