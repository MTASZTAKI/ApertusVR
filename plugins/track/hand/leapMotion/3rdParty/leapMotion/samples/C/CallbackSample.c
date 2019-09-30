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
#include "LeapC.h"
#include "ExampleConnection.h"

/** Callback for when the connection opens. */
void OnConnect(){
  printf("Connected.\n");
}

/** Callback for when a device is found. */
void OnDevice(const LEAP_DEVICE_INFO *props){
  printf("Found device %s.\n", props->serial);
}

/** Callback for when a frame of tracking data is available. */
void OnFrame(const LEAP_TRACKING_EVENT *frame){
  printf("Frame %lli with %i hands.\n", (long long int)frame->info.frame_id, frame->nHands);
  for(uint32_t h = 0; h < frame->nHands; h++){
    LEAP_HAND* hand = &frame->pHands[h];
    printf("    Hand id %i is a %s hand with position (%f, %f, %f).\n",
                hand->id,
                (hand->type == eLeapHandType_Left ? "left" : "right"),
                hand->palm.position.x,
                hand->palm.position.y,
                hand->palm.position.z);
  }
}


int main(int argc, char** argv) {
  //Set callback function pointers
  ConnectionCallbacks.on_connection          = &OnConnect;
  ConnectionCallbacks.on_device_found        = &OnDevice;
  ConnectionCallbacks.on_frame               = &OnFrame;

  OpenConnection();

  printf("Press Enter to exit program.\n");
  getchar();
  return 0;
}
//End-of-Sample.c
