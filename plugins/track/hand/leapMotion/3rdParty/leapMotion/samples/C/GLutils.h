#pragma once

#if defined(_WIN32)
#include <Windows.h>
#endif

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
  #if !defined(OPENGL_ES)
    #define GLEW_STATIC
    #include "GL/glew.h"
    #include <GL/gl.h>
  #else
    #include <GLES/gl.h>
    #include <GLES/glext.h>
    #include <GLES3/gl3.h>
    #include <GLES3/gl3ext.h>
  #endif
  #include <GL/glut.h>
#endif

#include <stdio.h>
#include "LeapC.h"
#include <stdlib.h>
#include <math.h>

#define STRINGIFY(s) #s

void checkGLError(char * contextMessage);
GLuint createProgram(const char* vertex_shader, const char* fragment_shader);
GLuint createTextureReference();
void printProgramInfoLog(GLuint obj);
void printShaderInfoLog(GLuint obj);
void setPerspectiveFrustrum(GLdouble fovY, GLdouble aspect, GLdouble neard, GLdouble fard);
