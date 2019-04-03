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

#include "apeFobHeadTracking.h"

void* trackdInitTrackerReader(int)
{
	//return 0;
	return (void*)1;
}

int trackdGetEulerAngles(void* tracker, int id, float* orn)
{
	orn[0] = 45.0f;
	orn[1] = 0.0f;
	orn[2] = 0.0f;
	return 1;
}

int trackdGetPosition(void* tracker, int id, float* pos)
{
	pos[0] = 0.1f;
	pos[1] = 0.1f;
	pos[2] = 0.1f;
	return 1;
}

int trackdGetMatrix( void* tracker, int id, float mat[4][4] )
{
	return 0;
}