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
#include <math.h>

//The pixel size of the textures we write the undistorted images to
#define TEX_WIDTH 400
#define TEX_HEIGHT 400
//Max field of view varies by device, use 8 for the peripheral, 22 for Rigel. I.e. tan(FOV/2)
#define MAX_FOV 22

LEAP_CONNECTION *connection;

LEAP_IMAGE_FRAME_REQUEST_TOKEN *image_token = 0;
float image_width;
float image_height;
void* image_buffer = NULL;
char* undistorted_image_left = NULL;
char* undistorted_image_right = NULL;
uint64_t image_size = 1;
bool imageRequested = false;
bool imageReady = false;
bool textureChanged = false;
GLuint texture = 0;

int window; // GLUT window handle

/** Callback for when an image request completes. */
void OnImages(const LEAP_IMAGE_COMPLETE_EVENT *imageCompleteEvent){
    free(image_token);
    image_token = 0;
    imageRequested = false;
    imageReady = true;
    if(image_width != imageCompleteEvent->properties->width ||
       image_height != imageCompleteEvent->properties->height){
      image_width = imageCompleteEvent->properties->width;
      image_height = imageCompleteEvent->properties->height;
      textureChanged = true;
    }
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

//display the textured quad
void display(void)
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(-32, -24, -50); //"Camera" viewpoint
  glClear(GL_COLOR_BUFFER_BIT);
  if(imageReady){
    //Left image
    for( float row = 0; row < TEX_HEIGHT; row++ ) {
      for( float col = 0; col < TEX_WIDTH; col++ ) {

            //Normalize from pixel xy to range [0..1]
            LEAP_VECTOR input;
            input.x = col/TEX_WIDTH;
            input.y = row/TEX_HEIGHT;

            //Convert from normalized [0..1] to ray slopes
            input.x = (input.x - .5) * MAX_FOV;
            input.y = (input.y - .5) * MAX_FOV;

            LEAP_VECTOR pixel = LeapRectilinearToPixel(*connection,
                                                       eLeapPerspectiveType_stereo_left,
                                                       input);
            int dindex = (int)floor(row * TEX_WIDTH + col);
            int pindex = (int)roundf(pixel.y) * image_width + (int)roundf(pixel.x);
            if(pixel.x >= 0 && pixel.x < image_width && pixel.y >=0 && pixel.y < image_height){
              undistorted_image_left[dindex] = ((char*)image_buffer)[pindex];
            } else {
              undistorted_image_left[dindex] = 128;
            }
        }
    }
    // right image
    for( float row = 0; row < TEX_HEIGHT; row++ ) {
        for( float col = 0; col < TEX_WIDTH; col++ ) {

            //Normalize from pixel xy to range [0..1]
            LEAP_VECTOR input;
            input.x = col/TEX_WIDTH;
            input.y = row/TEX_HEIGHT;

            //Convert from normalized [0..1] to ray slopes
            input.x = (input.x - .5) * MAX_FOV;
            input.y = (input.y - .5) * MAX_FOV;

            LEAP_VECTOR pixel = LeapRectilinearToPixel(*connection,
                                                       eLeapPerspectiveType_stereo_right,
                                                       input);
            int dindex = (int)floor(row * TEX_WIDTH + col);
            int pindex = (int)roundf(pixel.y + image_height) * image_width + (int)roundf(pixel.x);

            if(pixel.x >= 0 && pixel.x < image_width && pixel.y >=0 && pixel.y < image_height){
              undistorted_image_right[dindex] = ((char*)image_buffer)[pindex];
            } else {
              undistorted_image_right[dindex] = 200;
            }
        }
    }

    DrawImageQuad(0, 0, 64, 24, TEX_WIDTH, TEX_HEIGHT, undistorted_image_left);
    DrawImageQuad(0, 26, 64, 50, TEX_WIDTH, TEX_HEIGHT, undistorted_image_right);
    imageReady = false;
  }
  glFlush();
  glPopMatrix();
  glutSwapBuffers();
}
//end of display

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
  undistorted_image_left = malloc(TEX_WIDTH * TEX_HEIGHT);
  undistorted_image_right = malloc(TEX_WIDTH * TEX_HEIGHT);
  while(!IsConnected){
    millisleep(250);
  }

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(640, 480);
  window = glutCreateWindow("LeapC Undistorted Image Example");

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
  free(undistorted_image_left);
  free(undistorted_image_right);
  return 0;
}
//End-of-Sample
