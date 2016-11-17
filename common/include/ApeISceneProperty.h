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


#ifndef APE_ISCENEPROPERTY_H
#define APE_ISCENEPROPERTY_H

#include <map>
#include <vector>
#include <string>
#include <memory>
#include "ApeColor.h"

namespace Ape
{
	typedef std::vector<std::string> ResourceFolders;

	class ISceneProperty
	{ 
	protected:
		virtual ~ISceneProperty() {};
		
	public:
		virtual void addResourceLocation(std::string resourceFolder, std::string resourceGroupName) = 0;

		virtual void setAmbientColor(Color ambient) = 0;
		
		virtual void setSkyBoxMaterialName(std::string materialName) = 0;

		virtual ResourceFolders getResourceFolders() = 0;

		virtual Color getAmbientColor() = 0;

		virtual std::string getSkyBoxMaterialName() = 0;
	};

	typedef std::shared_ptr<ISceneProperty> ScenePropertySharedPtr;

	typedef std::weak_ptr<ISceneProperty> ScenePropertyWeakPtr;
}

#endif
