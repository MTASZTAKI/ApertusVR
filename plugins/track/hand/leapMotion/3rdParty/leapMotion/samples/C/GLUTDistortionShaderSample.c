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
#include <string.h>

static const char* GLSL_VERT = STRINGIFY(
    \n#version 120\n

    void main()
    {
        gl_Position = ftransform();
        gl_TexCoord[0]=gl_MultiTexCoord0;
    }
);
static const char* GLSL_FRAG = STRINGIFY(
    \n#version 120\n
    uniform sampler2D rawTexture;
    uniform sampler2D distortionTexture;

    void main()
    {
        vec2 distortionIndex = texture2D(distortionTexture, gl_TexCoord[0].st).xy;
        float hIndex = distortionIndex.r;
        float vIndex = distortionIndex.g;

        if(vIndex > 0.0 && vIndex < 1.0 && hIndex > 0.0 && hIndex < 1.0)
        {
            gl_FragColor = vec4(texture2D(rawTexture, distortionIndex).rrr, 1.0);
        }
        else
        {
            gl_FragColor = vec4(0.2, 0.0, 0.0, 1.0);
        }
    }
);

LEAP_CONNECTION *connection;

LEAP_IMAGE_FRAME_REQUEST_TOKEN *image_token = 0;
GLsizei image_width;
GLsizei image_height;
void* image_buffer = NULL;
float* distortion_buffer_left = NULL;
float* distortion_buffer_right = NULL;
uint64_t image_size = 1;
bool imageRequested = false;
bool imageReady = false;
uint64_t currentDistortionId = 0;

int window; // GLUT window handle
int windowWidth = 800;
int windowHeight = 400;

GLuint vertShader, fragShader, program;
GLuint rawTextureLeft = 0;
GLuint rawTextureRight = 0;
GLuint distortionTextureLeft = 0;
GLuint distortionTextureRight = 0;

/** Callback for when an image request completes. */
void OnImages(const LEAP_IMAGE_COMPLETE_EVENT *imageCompleteEvent){
    free(image_token);
    image_token = 0;
    imageRequested = false;
    imageReady = true;
    image_width = imageCompleteEvent->properties->width;
    image_height = imageCompleteEvent->properties->height;

    //Save the distortion data if it's version id changes
    if(currentDistortionId != imageCompleteEvent->matrix_version){
      size_t distortion_size = 64 * 64 * 2;
      distortion_buffer_left = malloc(sizeof(float) * distortion_size);
      distortion_buffer_right = malloc(sizeof(float) * distortion_size);
      memcpy(distortion_buffer_left, imageCompleteEvent->distortion_matrix[0], sizeof(float) * distortion_size);
      memcpy(distortion_buffer_right, imageCompleteEvent->distortion_matrix[1], sizeof(float) * distortion_size);
      currentDistortionId = imageCompleteEvent->matrix_version;
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

void display(void)
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glPushMatrix();
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f );
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if(imageReady){
    //Setup perspective and view matrices
    glMatrixMode(GL_PROJECTION);
    checkGLError("set matrix mode");
    glLoadIdentity();
    setPerspectiveFrustrum(96, windowWidth/windowHeight, 1, 20);
    checkGLError("set frustrum");

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(1.05f, 0.0, -1.01f);

    glEnable(GL_TEXTURE_2D);
    glUseProgram(program);

    // Left camera image
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rawTextureLeft);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, image_width, image_height, 0, GL_RED, GL_UNSIGNED_BYTE, image_buffer);
    checkGLError("Initializing raw texture");

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, distortionTextureLeft);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, 64, 64, 0, GL_RG, GL_FLOAT, distortion_buffer_left);
    checkGLError("Initializing distortion texture");

    glBegin(GL_QUADS); // Draw A Quad for camera image
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f); // Top Left
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f, 0.0f); // Top Right
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, 0.0f); // Bottom Right
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f); // Bottom Left
    glEnd();

    // Right camera image
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rawTextureRight);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, image_width, image_height, 0, GL_RED, GL_UNSIGNED_BYTE,
      (GLvoid*)((size_t)(image_buffer) + (image_width * image_height)));
    checkGLError("Updating raw texture with right image");

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, distortionTextureRight);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, 64, 64, 0, GL_RG, GL_FLOAT, distortion_buffer_right);
    checkGLError("Updating distortion texture with right distortion map");

    glTranslatef(-2.01f, 0.0, 0.0);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f); // Top Left
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f, 0.0f); // Top Right
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,-1.0f, 0.0f); // Bottom Right
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,-1.0f, 0.0f); // Bottom Left
    glEnd();

    glPopMatrix();

    imageReady = false;
  }
  glFlush();
  glPopMatrix();
  glutSwapBuffers();
}

void reshape(int w, int h)
{
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
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
  glutInitWindowSize(windowWidth, windowHeight);
  window = glutCreateWindow("LeapC Distortion Shader Example");

#if defined(GLEW_VERSION)
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    /* Problem: glewInit failed, something is seriously wrong. */
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    return 1;
  }
#endif

  // GLUT callbacks
  glutIdleFunc(idle);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutDisplayFunc(display);

  // init GL
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glColor3f(1.0, 1.0, 1.0);

  // init shader and textures

  //Create the shader program
  program = createProgram(GLSL_VERT, GLSL_FRAG);
  glUseProgram(program);
  GLuint rawSampler = glGetUniformLocation(program, "rawTexture");
  GLuint distortionSampler  = glGetUniformLocation(program, "distortionTexture");
  glUniform1i(rawSampler, 0);
  glUniform1i(distortionSampler,  1);

  //Create textures
  rawTextureLeft = createTextureReference();
  distortionTextureLeft = createTextureReference();
  rawTextureRight = createTextureReference();
  distortionTextureRight = createTextureReference();

  // Start GLUT loop
  glutMainLoop();

  CloseConnection();
  free(distortion_buffer_left);
  free(distortion_buffer_right);
  return 0;
}
//End-of-Sample
