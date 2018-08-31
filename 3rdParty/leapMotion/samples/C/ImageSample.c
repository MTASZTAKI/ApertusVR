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

LEAP_CONNECTION *connection;

//Image request variables
LEAP_IMAGE_FRAME_REQUEST_TOKEN *image_token = 0;
void* image_buffer = NULL;
uint64_t image_size = 1;
bool imageRequested = false;

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

  //Request the matching image (only do this if using images)
  if(!imageRequested){ //only support one outstanding image request to reduce bookkeeping
    imageRequested = true;
    LEAP_IMAGE_FRAME_DESCRIPTION frameDescription;
    frameDescription.type = eLeapImageType_Default;
    frameDescription.frame_id = frame->info.frame_id;
    frameDescription.buffer_len = image_size;
    frameDescription.pBuffer = image_buffer;

    image_token = malloc(sizeof(LEAP_IMAGE_FRAME_REQUEST_TOKEN));
    eLeapRS result = LeapRequestImages(*connection, &frameDescription, image_token);
    printf("LeapRequestImages() result was %s.\n", ResultString(result));
  }
}

/** Callback for when an image request completes. */
void OnImages(const LEAP_IMAGE_COMPLETE_EVENT *imageCompleteEvent){
    printf("Received image set for frame %lli with size %lli.\n",
           (long long int)imageCompleteEvent->info.frame_id,
           (long long int)imageCompleteEvent->data_written);

    free(image_token);
    image_token = 0;
    imageRequested = false;
}

/** Callback for when an image request fails. */
void OnImageError(const LEAP_IMAGE_FRAME_REQUEST_ERROR_EVENT *imageErrorEvent){
    if(imageErrorEvent->error == eLeapImageRequestError_ImagesDisabled)
        printf("Warning: Images disabled. Check your control panel settings.");

    //Resize image buffer if too small
    if(image_size < imageErrorEvent->required_buffer_len){
        image_size = imageErrorEvent->required_buffer_len;
        image_buffer = malloc((size_t)image_size);
        printf("Resized image buffer to %lli.\n", (long long int)image_size);
    }

    free(image_token);
    image_token = NULL;
    imageRequested = false;
}

int main(int argc, char** argv) {
  //Set callback function pointers
  ConnectionCallbacks.on_connection          = &OnConnect;
  ConnectionCallbacks.on_device_found        = &OnDevice;
  ConnectionCallbacks.on_frame               = &OnFrame;
  ConnectionCallbacks.on_image_complete      = &OnImages;
  ConnectionCallbacks.on_image_request_error = &OnImageError;

  connection = OpenConnection();

  printf("Press Enter to exit program.\n");
  getchar();
  return 0;
}
//End-of-Sample.c
