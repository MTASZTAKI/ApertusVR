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

#ifndef APE_WATERIMPL_H
#define APE_WATERIMPL_H

#include "apeISceneManager.h"
#include "apeIWater.h"
#include "apeEventManagerImpl.h"
#include "apeReplica.h"

namespace ape
{
	class WaterImpl : public IWater
	{
	public:

		WaterImpl(std::string name);

		~WaterImpl();

		void setSky(ape::SkyWeakPtr sky) override;

		ape::SkyWeakPtr getSky() override;

		void setCameras(std::vector<ape::CameraWeakPtr> cameras) override;

		std::vector<ape::CameraWeakPtr> getCameras() override;

	private:
		ape::EventManagerImpl* mpEventManagerImpl;

		ape::ISceneManager* mpSceneManager;

		ape::SkyWeakPtr mSky;

		std::string mSkyName;

		std::vector<ape::CameraWeakPtr> mCameras;

		std::vector<std::string> mCamerasName;
	};
}

#endif
