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

LEAP_CLOCK_REBASER clockSynchronizer;

int main(int argc, char** argv) {
  LEAP_CONNECTION* connHandle = OpenConnection();

  while(!IsConnected){
    millisleep(250);
  }

  printf("Connected.\n");
  //Create the clock synchronizer
  LeapCreateClockRebaser(&clockSynchronizer);
  clock_t cpuTime;
  int64_t targetFrameTime = 0;
  uint64_t targetFrameSize = 0;
  eLeapRS result;
  for(;;){
    //Calculate the application time
    cpuTime = (clock_t).000001 * clock()/CLOCKS_PER_SEC;//microseconds
    //Synchronize the clocks
    LeapUpdateRebase(clockSynchronizer, cpuTime, LeapGetNow());

    //Simulate delay (i.e. processing load, v-sync, etc)
    millisleep(10);

    //Now get the updated application time
    cpuTime = (clock_t) .000001 * clock()/CLOCKS_PER_SEC;

    //Translate application time to Leap time
    LeapRebaseClock(clockSynchronizer, cpuTime, &targetFrameTime);

    //Get the buffer size needed to hold the tracking data
    result = LeapGetFrameSize(*connHandle, targetFrameTime, &targetFrameSize);
    if(result == eLeapRS_Success){
      //Allocate enough memory
      LEAP_TRACKING_EVENT* interpolatedFrame = malloc((size_t)targetFrameSize);
      //Get the frame
      result = LeapInterpolateFrame(*connHandle, targetFrameTime, interpolatedFrame, targetFrameSize);
      if(result == eLeapRS_Success){
        //Use the data...
        printf("Frame %lli with %i hands with delay of %lli microseconds.\n",
               (long long int)interpolatedFrame->tracking_frame_id,
               interpolatedFrame->nHands,
               (long long int)LeapGetNow() - interpolatedFrame->info.timestamp);
        for(uint32_t h = 0; h < interpolatedFrame->nHands; h++){
        LEAP_HAND* hand = &interpolatedFrame->pHands[h];
        printf("    Hand id %i is a %s hand with position (%f, %f, %f).\n",
                    hand->id,
                    (hand->type == eLeapHandType_Left ? "left" : "right"),
                    hand->palm.position.x,
                    hand->palm.position.y,
                    hand->palm.position.z);
        }
        //Free the allocated buffer when done.
        free(interpolatedFrame);
      }
      else {
        printf("LeapInterpolateFrame() result was %s.\n", ResultString(result));
      }
    }
    else {
      printf("LeapGetFrameSize() result was %s.\n", ResultString(result));
    }
  } //ctrl-c to exit
  return 0;
}
//End-of-Sample
