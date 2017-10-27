/*MIT License

Copyright (c) 2016 MTA SZTAKI

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

#ifndef APE_KINECTPLUGIN_H
#define APE_KINECTPLUGIN_H

#include <iostream>
#include <string>
#include <thread> 
//#include "Leap.h"
//#include "LeapMath.h"
#include "ApePluginAPI.h"
#include "ApeIPlugin.h"
#include "ApeISystemConfig.h"
#include "ApeINode.h"
#include "ApeIScene.h"
#include "ApeIMainWindow.h"
#include "ApeIEventManager.h"
#include "ApeICamera.h"
#include "ApeITextGeometry.h"
#include "ApeIManualMaterial.h"
#include "ApeISphereGeometry.h"
#include "ApeIManualPass.h"
#include <ApeITubeGeometry.h>

#define THIS_PLUGINNAME "ApeKinectPlugin"

namespace Ape
{
	class KinectPlugin : public Ape::IPlugin/*, public Leap::Listener*/
	{
		static const int        cDepthWidth = 512;
		static const int        cDepthHeight = 424;

	public:
		KinectPlugin();

		~KinectPlugin();

		void Init() override;

		void Run() override;

		void Step() override;

		void Stop() override;

		void Suspend() override;

		void Restart() override;

		void Update();

		/// <summary>
		/// Initializes the default Kinect sensor
		/// </summary>
		/// <returns>indicates success or failure</returns>
		HRESULT InitializeDefaultSensor();

		/// <summary>
		/// Handle new body data
		/// <param name="nTime">timestamp of frame</param>
		/// <param name="nBodyCount">body data count</param>
		/// <param name="ppBodies">body data in frame</param>
		/// </summary>
		void ProcessBody(INT64 nTime, int nBodyCount, IBody** ppBodies);

		//void onInit(const Leap::Controller& controller) override;

		//void onConnect(const Leap::Controller& controller) override;

		//void onDisconnect(const Leap::Controller& controller) override;

		//void onExit(const Leap::Controller& controller) override;

		//void onFrame(const Leap::Controller& controller) override;

		//void onFocusGained(const Leap::Controller& controller) override;

		//void onFocusLost(const Leap::Controller& controller) override;

		//void onDeviceChange(const Leap::Controller& controller) override;

		//void onServiceConnect(const Leap::Controller& controller) override;

		//void onServiceDisconnect(const Leap::Controller& controller) override;

	private:
		BOOLEAN Operatorfound[BODY_COUNT] = { false,false,false,false,false,false };
		//Ape::NodeWeakPtr HeadNode;
		//std::vector<Ape::NodeWeakPtr> mBNodes;
		std::vector<Ape::NodeWeakPtr> _0Body;
		std::vector<Ape::NodeWeakPtr> _1Body;
		std::vector<Ape::NodeWeakPtr> _2Body;
		std::vector<Ape::NodeWeakPtr> _3Body;
		std::vector<Ape::NodeWeakPtr> _4Body;
		std::vector<Ape::NodeWeakPtr> _5Body;

		double                  m_fFreq;
		float					body[BODY_COUNT][JointType_Count][3];

		// Current Kinect
		IKinectSensor*          m_pKinectSensor;
		ICoordinateMapper*      m_pCoordinateMapper;

		// Body reader
		IBodyFrameReader*       m_pBodyFrameReader;

		Ape::IScene* mpScene;

		Ape::ISystemConfig* mpSystemConfig;

		Ape::IMainWindow* mpMainWindow;

		Ape::IEventManager* mpEventManager;

		Ape::NodeWeakPtr mUserNode;

		void eventCallBack(const Ape::Event& event);

		//Leap::Controller mLeapController;

		/*std::vector<std::string> mFingerNames;
		
		std::vector<std::string> mBoneNames;
		
		std::vector<std::string> mStateNames;*/

		//float mPreviousFramePitch;

		//float mPreviousFrameYaw;

		//float mPreviousFrameRoll;

		//bool mHandOrientationFlag;

		Ape::NodeWeakPtr mLeftHandNode;

		Ape::NodeWeakPtr mRightHandNode;
	};
	
	APE_PLUGIN_FUNC Ape::IPlugin* CreateKinectPlugin()
	{
		return new KinectPlugin;
	}

	APE_PLUGIN_FUNC void DestroyKinectPlugin(Ape::IPlugin *plugin)
	{
		delete (KinectPlugin*)plugin;
	}

	APE_PLUGIN_DISPLAY_NAME(THIS_PLUGINNAME);

	APE_PLUGIN_ALLOC()
	{
		std::cout << THIS_PLUGINNAME << "_CREATE" << std::endl;
		ApeRegisterPlugin(THIS_PLUGINNAME, CreateKinectPlugin, DestroyKinectPlugin);
		return 0;
	}
}

#endif
