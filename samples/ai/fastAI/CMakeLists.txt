#MIT License
#
#Copyright (c) 2018 MTA SZTAKI
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

message(STATUS "ApeFastAI: Try to find Python")
FIND_PACKAGE(PythonLibs)
if (PYTHONLIBS_FOUND)
	message(STATUS "ApeFastAI: Python is installed")
	message(STATUS "ApeFastAI: Try to update pip")
	execute_process(COMMAND cmd /c "python -m pip install --upgrade pip" WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	message(STATUS "ApeFastAI: Try to install jupyter notebook")
	execute_process(COMMAND cmd /c "pip install jupyter notebook jupyter_contrib_nbextensions" WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	message(STATUS "ApeFastAI: Try to install matplotlib")
	execute_process(COMMAND cmd /c "pip install matplotlib" WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	message(STATUS "ApeFastAI: Try to install requests")
	execute_process(COMMAND cmd /c "pip install requests" WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	message(STATUS "ApeFastAI: Try to install torch")
	execute_process(COMMAND cmd /c "pip3 install torch===1.1.0 torchvision===0.4.2 -f https://download.pytorch.org/whl/torch_stable.html" WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	message(STATUS "ApeFastAI: Try to install fastai")
	execute_process(COMMAND cmd /c "pip install --upgrade setuptools" WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	execute_process(COMMAND cmd /c "pip install fastai==1.0.55" WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	message(STATUS "ApeFastAI: Try to install pretrainedmodels")
	execute_process(COMMAND cmd /c "pip install pretrainedmodels" WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	message(STATUS "ApeFastAI: Try to install pandas")
	execute_process(COMMAND cmd /c "pip install pandas" WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	message(STATUS "ApeFastAI: Try to install scipy")
	execute_process(COMMAND cmd /c "pip install scipy" WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	message(STATUS "ApeFastAI: Try to install dataclasses")
	execute_process(COMMAND cmd /c "pip install dataclasses" WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
else()
	message(STATUS "ApeFastAI: Python is not installed, try to install an 3.6.X version and run the cmake again to get the FastAI sample")
endif()

set (CONFIGS
	apeOgreRenderPlugin.json
	apeViewPointManagerPlugin.json
	apeNodeJsPlugin.json
	apeCore.json
	)

add_custom_target(
    FastAI ALL
    SOURCES ${CONFIGS}
)

set_property (TARGET FastAI PROPERTY FOLDER "Samples/AI")
