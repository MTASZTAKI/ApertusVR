/*MIT License

Copyright (c) 2018 MTA SZTAKI

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#ifndef APE_FOBHEADTRACKING_H
#define APE_FOBHEADTRACKING_H

#ifndef WIN32
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#endif 


#if  (WIN32 && TRACKD_DLL_EXPORTS)
#define TRACKD_EXPORT_API __declspec(dllexport)
#else
#define TRACKD_EXPORT_API
#endif

#ifdef __cplusplus
extern "C" {
#endif 

TRACKD_EXPORT_API void* trackdInitTrackerReader(int);


TRACKD_EXPORT_API int trackdGetPosition(void* tracker, int id, float* pos);


TRACKD_EXPORT_API int trackdGetEulerAngles(void* tracker, int id, float* orn);


TRACKD_EXPORT_API int trackdGetMatrix(void* tracker, int id, float mat[4][4]);


#ifdef __cplusplus
}
#endif 

#endif 
