//
//  GLutils.c
//  LeapCClient
//
//  Created by Joe Ward on 6/8/16.
//  Copyright © 2016 Leap Motion. All rights reserved.
//

#include "GLutils.h"

GLuint createProgram(const char* vertex_shader, const char* fragment_shader)
{
  GLint program = glCreateProgram();
  GLint compileSuccess;

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertex_shader, NULL);
  glCompileShader(vertexShader);
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compileSuccess);
  if (compileSuccess == GL_FALSE) {
    printf("Problem compiling vertex shader\n");
    printShaderInfoLog(vertexShader);
  }
  glAttachShader(program, vertexShader);

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragment_shader, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compileSuccess);
  if (compileSuccess == GL_FALSE) {
    printf("Problem compiling fragment shader\n");
    printShaderInfoLog(fragmentShader);
  }
  glAttachShader(program, fragmentShader);

  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &compileSuccess);
  if (compileSuccess == GL_FALSE) {
    printf("Problem linking program\n");
    printProgramInfoLog(vertexShader);
  }
  return program;
}

GLuint createTextureReference()
{
  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  return textureID;
}

void printShaderInfoLog(GLuint obj)
{
  int infologLength = 0;
  int charsWritten  = 0;
  char *infoLog;

  glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

  if (infologLength > 0) {
    infoLog = (char *)malloc(infologLength);
    glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
    printf("%s\n",infoLog);
    free(infoLog);
  }
}

void printProgramInfoLog(GLuint obj)
{
  int infologLength = 0;
  int charsWritten  = 0;
  char *infoLog;

  glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

  if (infologLength > 0) {
    infoLog = (char *)malloc(infologLength);
    glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
    printf("%s\n",infoLog);
    free(infoLog);
  }
}

void setPerspectiveFrustrum(GLdouble fovY, GLdouble aspect, GLdouble neard, GLdouble fard)
{
    const GLdouble pi = 3.1415926535897932384626433832795;
    GLdouble fW, fH;
    fH = tan( fovY / 360 * pi ) * neard;
    fW = fH * aspect;
    glFrustum( -fW, fW, -fH, fH, neard, fard);
}

/* Prints OpenGL errors to the stdout. */
void checkGLError(char * contextMessage) {
  GLenum err = glGetError();
  while(err!=GL_NO_ERROR) {
    char *error;

    switch(err) {
      case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break;
      case GL_INVALID_ENUM:           error="INVALID_ENUM";           break;
      case GL_INVALID_VALUE:          error="INVALID_VALUE";          break;
      case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break;
      default:                        error="Unknown error code";
    }
    printf("GL_%s at %s.\n", error, contextMessage);
    err = glGetError();
  }
}
