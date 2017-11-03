/******************************************************************************\
* Copyright (C) 2012-2016 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/

#undef __cplusplus

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <time.h>
#include "LeapC.h"
#include "ExampleConnection.h"
#include "GLutils.h"

LEAP_CONNECTION *connection;

LEAP_IMAGE_FRAME_REQUEST_TOKEN *image_token = 0;
void* image_buffer = NULL;
uint64_t image_size = 1;
bool imageRequested = false;
bool imageReady = false;
bool textureChanged = false;
uint32_t image_width = 0;
uint32_t image_height = 0;
GLuint texture = 0;

int window; // GLUT window handle

/** Callback for when an image request completes. */
void OnImages(const LEAP_IMAGE_COMPLETE_EVENT *imageCompleteEvent){
    if(image_width != imageCompleteEvent->properties->width ||
       image_height != imageCompleteEvent->properties->height){
      image_width = imageCompleteEvent->properties->width;
      image_height = imageCompleteEvent->properties->height;
      textureChanged = true;
    }
    free(image_token);
    image_token = 0;
    imageRequested = false;
    imageReady = true;
}

/** Callback for when an image request fails. */
void OnImageError(const LEAP_IMAGE_FRAME_REQUEST_ERROR_EVENT *imageErrorEvent){
    if(imageErrorEvent->error == eLeapImageRequestError_ImagesDisabled)
        printf("Warning: Images disabled. Check your control panel settings.");

    //Resize image buffer if too small
    if(image_size < imageErrorEvent->required_buffer_len){
        image_size = imageErrorEvent->required_buffer_len;
        if(image_buffer) free(image_buffer);
        image_buffer = malloc((size_t)image_size);
        printf("Resized image buffer to %lli.\n", (long long int)image_size);
    }

    free(image_token);
    image_token = NULL;
    imageRequested = false;
    imageReady = false;
}

/* Notifies us that a new frame is available. */
void OnFrame(const LEAP_TRACKING_EVENT *frame){
  if(!imageRequested && !imageReady){
    imageRequested = true;
    LEAP_IMAGE_FRAME_DESCRIPTION frameDescription;
    frameDescription.type = eLeapImageType_Default;
    frameDescription.frame_id = frame->info.frame_id;
    frameDescription.buffer_len = image_size;
    frameDescription.pBuffer = image_buffer;

    image_token = malloc(sizeof(LEAP_IMAGE_FRAME_REQUEST_TOKEN));
    eLeapRS result = LeapRequestImages(*connection, &frameDescription, image_token);
    if(result != eLeapRS_Success)
        printf("LeapRequestImages call was %s.\n", ResultString(result));
  }
}

// Draw a textured quad displaying the image data
void DrawImageQuad(float p1X, float p1Y, float p2X, float p2Y, int width, int height, void* imagedata){
  glEnable(GL_TEXTURE_2D);
  if(textureChanged){
    textureChanged = false;
    glDeleteTextures(1, &texture);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, imagedata);
    checkGLError("Initializing texture.");
  } else { //update existing texture
    glBindTexture ( GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, imagedata);
    checkGLError("Updating texture.");
  }
  //Draw a texture-mapped quad
  glBegin(GL_QUADS);
    glTexCoord2f(1, 1); glVertex2f((GLfloat)p1X, (GLfloat)p1Y);
    glTexCoord2f(0, 1); glVertex2f((GLfloat)p2X, (GLfloat)p1Y);
    glTexCoord2f(0, 0); glVertex2f((GLfloat)p2X, (GLfloat)p2Y);
    glTexCoord2f(1, 0); glVertex2f((GLfloat)p1X, (GLfloat)p2Y);
  glEnd();
  checkGLError("Drawing quad.");
  glDisable(GL_TEXTURE_2D);
}
// Done drawing quad

void display(void)
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(-32, -24, -50); //"Camera" viewpoint
  glClear(GL_COLOR_BUFFER_BIT);
  if(imageReady){
    DrawImageQuad(0, 0, 64, 48, image_width, image_height * 2, image_buffer);
    imageReady = false;
  }
  glFlush();
  glPopMatrix();
  glutSwapBuffers();
}

void reshape(int w, int h)
{
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, 640/240, 1.0, 1000);
}

void keyboard(unsigned char key, int x, int y)
{
  switch((char)key) {
  case 'q':
  case 27:  // ESC
    glutDestroyWindow(window);
    if(imageRequested)
        LeapCancelImageFrameRequest(*connection, *image_token);
    if(image_buffer) free(image_buffer);
    CloseConnection();
    exit(0);
  default:
    break;
  }
}

void idle(void){
  if(imageReady)
    glutPostRedisplay();
}

int main(int argc, char *argv[])
{
  ConnectionCallbacks.on_frame               = OnFrame;
  ConnectionCallbacks.on_image_complete      = OnImages;
  ConnectionCallbacks.on_image_request_error = OnImageError;

  connection = OpenConnection();

  while(!IsConnected){
    millisleep(250);
  }

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(640, 480);
  window = glutCreateWindow("LeapC Image Example");

  // GLUT callbacks
  glutIdleFunc(idle);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutDisplayFunc(display);

  // init GL
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glColor3f(1.0, 1.0, 1.0);

  // Start GLUT loop
  glutMainLoop();

  CloseConnection();
  return 0;
}
//End-of-Sample
