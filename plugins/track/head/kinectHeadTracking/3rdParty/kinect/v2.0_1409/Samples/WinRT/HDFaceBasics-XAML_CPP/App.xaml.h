//------------------------------------------------------------------------------
// <copyright file="App.xaml.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------


#pragma once

#include "App.g.h"

namespace Microsoft
{
	namespace Samples
	{
		namespace Kinect
		{
			namespace HDFaceBasics
			{
				/// <summary>
				/// Provides application-specific behavior to supplement the default Application class.
				/// </summary>
				ref class App sealed
				{
				public:
					App();
					virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ args) override;

				private:
					void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);
				};
			}
		}
	}
}
