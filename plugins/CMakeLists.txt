#MIT License
#
#Copyright (c) 2016 MTA SZTAKI
#
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in all
#copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#SOFTWARE.


add_subdirectory (360ImageScene)
add_subdirectory (360StereoImageScene)
add_subdirectory (360VideoScene)
add_subdirectory (assimpAssetLoader)
add_subdirectory (bulletPhysics)
add_subdirectory (cefBrowser)
add_subdirectory (htcVive)
add_subdirectory (htmlOverlayUiManager)
add_subdirectory (inGameBrowserScene)
add_subdirectory (kinect)
add_subdirectory (kinectHeadTracking)
add_subdirectory (leapMotion)
add_subdirectory (oceanAndSkySimulationScene)
add_subdirectory (ogreRender)
add_subdirectory (oisUserInput)
add_subdirectory (physicsSimulationScene)
add_subdirectory (sampleScene)
add_subdirectory (viewPointManager)

option(APE_JSAPI "Use JS_API?" OFF )
if (${APE_JSAPI})
	add_subdirectory (jsAPI)
endif ()
