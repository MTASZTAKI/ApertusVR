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

option(APE_SAMPLES_MultiPlayer "Use APE_SAMPLES_MultiPlayer?" ON )
option(APE_SAMPLES_SinglePlayer "Use APE_SAMPLES_SinglePlayer?" ON )
option(APE_SAMPLES_CaveSystem "Use APE_SAMPLES_CaveSystem?" ON )
option(APE_SAMPLES_LinkageDesignerVR "Use APE_SAMPLES_LinkageDesignerVR?" ON )
option(APE_SAMPLES_FobHeadTracking "APE_SAMPLES_FobHeadTracking?" ON )
option(APE_SAMPLES_MultiDisplay "APE_SAMPLES_MultiDisplay?" ON )
option(APE_SAMPLES_Tester "APE_SAMPLES_Tester?" ON )
option(APE_SAMPLES_RobotMonitoring "APE_SAMPLES_RobotMonitoring?" ON )
option(APE_SAMPLES_Lego "APE_SAMPLES_Lego?" ON )
option(APE_SAMPLES_OculusDK2 "APE_SAMPLES_OculusDK2?" ON )

if (${APE_SAMPLES_MultiPlayer})
	add_subdirectory (multiPlayer)
endif ()	
if (${APE_SAMPLES_MultiDisplay})
	add_subdirectory (multiDisplay)
endif ()
if (${APE_SAMPLES_SinglePlayer})
	add_subdirectory (singlePlayer)
endif ()
if (${APE_SAMPLES_CaveSystem})
	add_subdirectory (caveSystem)
endif ()
if (${APE_SAMPLES_LinkageDesignerVR})
	add_subdirectory (linkageDesignerVR)
endif ()
if (${APE_SAMPLES_FobHeadTracking})
	add_subdirectory (fobHeadTracking)
endif ()
if (${APE_SAMPLES_Tester})
	add_subdirectory (tester)
endif ()
if (${APE_SAMPLES_RobotMonitoring})
	add_subdirectory (robotMonitoring)
endif ()
if (${APE_SAMPLES_Lego})
	add_subdirectory (lego)
endif ()
if (${APE_SAMPLES_OculusDK2})
	add_subdirectory (oculusDK2)
endif ()
