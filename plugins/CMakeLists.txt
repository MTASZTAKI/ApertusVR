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

# Basic

option(APE_PLUGIN_HelloWorld "Use HelloWorldPlugin?" ON )
if (${APE_PLUGIN_HelloWorld})
	add_subdirectory (helloWorld)
endif ()

option(APE_PLUGIN_Tester "Use TesterPlugin?" ON )
if (${APE_PLUGIN_Tester})
	add_subdirectory (tester)
endif ()

option(APE_PLUGIN_CefBrowser "Use CefBrowserPlugin?" ON )
if (${APE_PLUGIN_CefBrowser})
	add_subdirectory (cefBrowser)
endif ()
option(APE_PLUGIN_CefBrowser_VideoCodecsSupport "Use CefBrowserPlugin VideoCodecsSupport?" OFF )
option(APE_PLUGIN_CefBrowser_VideoCodecsSupport_FromSource "Use CefBrowserPlugin VideoCodecsSupport_FromSource?" OFF )

option(APE_PLUGIN_InGameBrowserSample "Use InGameBrowserSamplePlugin?" ON )
if (${APE_PLUGIN_InGameBrowserSample})
	add_subdirectory (inGameBrowserSample)
endif ()

# Environment Simulation

option(APE_PLUGIN_SkyTerrainWaterScene "Use SkyTerrainWaterScenePlugin?" ON )
if (${APE_PLUGIN_SkyTerrainWaterScene})
	add_subdirectory (skyTerrainWaterScene)
endif ()

# 360

option(APE_PLUGIN_360Image "Use 360ImagePlugin?" ON )
if (${APE_PLUGIN_360Image})
	add_subdirectory (360Image)
endif ()

option(APE_PLUGIN_360Video "Use 360VideoPlugin?" ON )
if (${APE_PLUGIN_360Video})
	add_subdirectory (360Video)
endif ()

# 3D model formats

option(APE_PLUGIN_AssimpAssetLoader "Use AssimpAssetLoaderPlugin?" ON )
if (${APE_PLUGIN_AssimpAssetLoader})
	add_subdirectory (assimpAssetLoader)
endif ()

# UI

option(APE_PLUGIN_HtmlOverlayUI "Use HtmlOverlayUIPlugin?" ON )
if (${APE_PLUGIN_HtmlOverlayUI})
	add_subdirectory (htmlOverlayUI)
endif ()

option(APE_PLUGIN_Avatar "Use AvatarPlugin?" ON )
if (${APE_PLUGIN_Avatar})
	add_subdirectory (avatar)
endif ()

option(APE_PLUGIN_GallerySamplePlugin "Use GallerySamplePlugin?" ON )
if (${APE_PLUGIN_GallerySamplePlugin})
	add_subdirectory (gallerySample)
endif ()

# Hand Tracking

option(APE_PLUGIN_LeapMotion "Use LeapMotionPlugin?" ON )
if (${APE_PLUGIN_LeapMotion})
	add_subdirectory (leapMotion)
endif ()

# Head Tracking

option(APE_PLUGIN_FobHeadTracking "Use FobHeadTrackingPlugin?" ON )
if (${APE_PLUGIN_FobHeadTracking})
	add_subdirectory (fobHeadTracking)
endif ()

# Renders

option(APE_PLUGIN_OgreRender "Use OgreRenderPlugin?" ON )
if (${APE_PLUGIN_OgreRender})
	add_subdirectory (ogreRender)
endif ()

option(APE_PLUGIN_Ogre21Render "Use Ogre21RenderPlugin?" OFF )
if (${APE_PLUGIN_Ogre21Render})
	add_subdirectory (ogre21Render)
endif ()

# Input handlers

option(APE_PLUGIN_OisUserInput "Use OisUserInputPlugin?" ON )
if (${APE_PLUGIN_OisUserInput})
	add_subdirectory (oisUserInput)
endif ()

# Displays

option(APE_PLUGIN_OculusDK2 "Use OculusDK2Plugin?" ON )
if (${APE_PLUGIN_OculusDK2})
	add_subdirectory (oculusDK2)
endif ()

option(APE_PLUGIN_HtcVive "Use HtcVivePlugin?" ON )
if (${APE_PLUGIN_HtcVive})
	add_subdirectory (htcVive)
endif ()

# PointCloud

option(APE_PLUGIN_Kinect "Use KinectPlugin?" ON )
if (${APE_PLUGIN_Kinect})
	add_subdirectory (kinect)
endif ()

option(APE_PLUGIN_Zed "Use ZedPlugin?" ON )
if (${APE_PLUGIN_Zed})
	add_subdirectory (zed)
endif ()

option(APE_PLUGIN_PointCloudRecorder "Use PointCloudRecorder?" ON )
if (${APE_PLUGIN_PointCloudRecorder})
 	add_subdirectory (pointCloudRecorder)
endif ()

# Industry / IoT / Controllers / Sensors

option(APE_PLUGIN_Industry40 "Use Industry40Plugin?" ON )
if (${APE_PLUGIN_Industry40})
	add_subdirectory (industry40)
endif ()

option(APE_PLUGIN_RobotMonitoring "Use RobotMonitoringPlugin?" ON )
if (${APE_PLUGIN_RobotMonitoring})
	add_subdirectory (robotMonitoring)
endif ()

# Apis, servers

option(APE_PLUGIN_JsAPI "Use JsAPIPlugin?" ON )
if (${APE_PLUGIN_JsAPI})
	add_subdirectory (jsAPI)
endif ()

option(APE_PLUGIN_Webserver "Use WebserverPlugin?" ON )
if (${APE_PLUGIN_Webserver})
	add_subdirectory (webserver)
endif ()

# Others

option(APE_PLUGIN_EngineeringScene "Use EngineeringScenePlugin?" ON )
if (${APE_PLUGIN_EngineeringScene})
	add_subdirectory (engineeringScene)
endif ()

option(APE_PLUGIN_PresentationScene "Use PresentationScenePlugin?" ON )
if (${APE_PLUGIN_PresentationScene})
	add_subdirectory (presentationScene)
endif ()

option(APE_PLUGIN_ArtScene "Use ArtScenePlugin?" ON )
if (${APE_PLUGIN_ArtScene})
	add_subdirectory (artScene)
endif ()

option(APE_PLUGIN_LinkageDesignerVR "Use LinkageDesignerVRPlugin?" ON )
if (${APE_PLUGIN_LinkageDesignerVR})
	add_subdirectory (linkageDesignerVR)
endif ()

option(APE_PLUGIN_Lego "Use LegoPlugin?" ON )
if (${APE_PLUGIN_Lego})
	add_subdirectory (lego)
endif ()

option(APE_PLUGIN_TexasEEGPlugin "Use TexasEEGPlugin?" ON )
if (${APE_PLUGIN_TexasEEGPlugin})
	add_subdirectory (texasEEG)
endif ()

option(APE_PLUGIN_RobotCalibration "Use RobotCalibrationPlugin?" ON )
if (${APE_PLUGIN_RobotCalibration})
	add_subdirectory (robotCalibration)
endif ()

option(APE_PLUGIN_GyorPlugin "Use GyorPlugin?" ON )
if (${APE_PLUGIN_GyorPlugin})
	add_subdirectory (gyor)
endif ()

option(APE_PLUGIN_AudiPlugin "Use AudiPlugin?" ON )
if (${APE_PLUGIN_AudiPlugin})
	add_subdirectory (audi)
endif ()
