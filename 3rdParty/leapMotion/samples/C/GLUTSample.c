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

int32_t lastDrawnFrameId = 0;
volatile int32_t newestFrameId = 0;
int window; // GLUT window handle

/* Notifies us that a new frame is available. */
void OnFrame(const LEAP_TRACKING_EVENT *frame){
  newestFrameId = (int32_t)frame->tracking_frame_id;
}

void display(void)
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(0, -300, -500); //"Camera" viewpoint
  glClear(GL_COLOR_BUFFER_BIT);
  LEAP_TRACKING_EVENT *frame = GetFrame();
  for(uint32_t h = 0; h < frame->nHands; h++){
    // Draw the hand
    LEAP_HAND *hand = &frame->pHands[h];
    //elbow
    glPushMatrix();
    glTranslatef(hand->arm.prev_joint.x, hand->arm.prev_joint.y, hand->arm.prev_joint.z);
    glutWireOctahedron();
    glPopMatrix();

    //wrist
    glPushMatrix();
    glTranslatef(hand->arm.next_joint.x, hand->arm.next_joint.y, hand->arm.next_joint.z);
    glutWireOctahedron();
    glPopMatrix();

    //palm position
    glPushMatrix();
    glTranslatef(hand->palm.position.x, hand->palm.position.y, hand->palm.position.z);
    glutWireOctahedron();
    glPopMatrix();

    //Distal ends of bones for each digit
    for(int f = 0; f < 5; f++){
      LEAP_DIGIT finger = hand->digits[f];
      for(int b = 0; b < 4; b++){
        LEAP_BONE bone = finger.bones[b];
        glPushMatrix();
        glTranslatef(bone.next_joint.x, bone.next_joint.y, bone.next_joint.z);
        glutWireOctahedron();
        glPopMatrix();
      }
    }
    // End of draw hand
  }
  glFlush();
  glPopMatrix();
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
    CloseConnection();
    exit(0);
  default:
    break;
  }
}

void idle(void){
  if(lastDrawnFrameId < newestFrameId){
    lastDrawnFrameId = newestFrameId;
    glutPostRedisplay();
  }
}

int main(int argc, char *argv[])
{
  ConnectionCallbacks.on_frame = OnFrame;
  OpenConnection();

  while(!IsConnected){
    millisleep(250);
  }

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
  glutInitWindowSize(640, 240);
  window = glutCreateWindow("LeapC Example");

  // GLUT callbacks
  glutIdleFunc(idle);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutDisplayFunc(display);

  // init GL
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glColor3f(0.0, 0.0, 0.0);
  glLineWidth(3.0);

  // Start GLUT loop
  glutMainLoop();

  CloseConnection();
  return 0;
}
//End-of-Sample
