/*
-----------------------------------------------------------------------------
This source file is part of ogre-procedural

For the latest info, see http://code.google.com/p/ogre-procedural/

Copyright (c) 2010-2013 Michael Broutin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef __Illustrations_h_
#define __Illustrations_h_

#include "Ogre.h"
#include "Procedural.h"

using namespace Ogre;

class dotFile
{
public:
	enum CONNECTION { JOIN, SPLIT, ROW };

private:
	struct sItem
	{
		std::string mName;
		std::string mFile;
	};
	std::map<int, sItem> mList;
	struct sPair
	{
		int mFrom;
		int mTo;
	};
	std::vector<sPair> mBinding;
	sItem mThis;
	std::string mPath;

public:
	dotFile(std::string path, std::string file, std::string name = "")
	{
		mPath = path;
		mThis.mFile = file;
		mThis.mName = (name.length() > 0) ? name : file;
	}

	int add(std::string name, std::string file = "")
	{
		if (name.size() < 3) return -1;
		int retVal = mList.size() + 1;
		sItem item;
		item.mName = name;
		item.mFile = file;
		mList[retVal] = item;
		return retVal;
	}

	void bind(int from, int to)
	{
		if (mList.find(from) == mList.end() || mList.find(to) == mList.end()) return;
		sPair pair;
		pair.mFrom = from;
		pair.mTo = to;
		mBinding.push_back(pair);
	}

	bool save()
	{
		std::ofstream dotfile((mThis.mFile + ".gv").c_str());
		if (dotfile.is_open())
		{
			dotfile << "digraph " << mThis.mName << " {" << std::endl;
			dotfile << "\tratio = \"auto\";" << std::endl << "\tmincross = 2.0;" << std::endl << std::endl;
			for (std::map<int, sItem>::iterator iter = mList.begin(); iter != mList.end(); iter++)
			{
				if (iter->second.mFile.length() > 3)
					dotfile << "\tIMG" << StringConverter::toString(iter->first) << " [shape=box,label=\"" << iter->second.mName << "\",image=\"" << mPath << "/" << iter->second.mFile << ".png\",labelloc=b];" << std::endl;
				else
					dotfile << "\tIMG" << StringConverter::toString(iter->first) << " [shape=circle,label=\"" << iter->second.mName << "\"];" << std::endl;

			}
			dotfile << std::endl;
			for (std::vector<sPair>::iterator iter = mBinding.begin(); iter != mBinding.end(); iter++)
			{
				dotfile << "\tIMG" << StringConverter::toString(iter->mFrom) << "->IMG" << StringConverter::toString(iter->mTo) << ";" << std::endl;
			}
			dotfile << "}" << std::endl;
			dotfile.close();
			return true;
		}
		else
			return false;
	}

	void set(std::string img1_name, std::string img1_file, std::string img2_name, std::string img2_file)
	{
		int a1 = add(img1_name, img1_file);
		int a2 = add(img2_name, img2_file);
		bind(a1, a2);
	}

	void set(std::string img1_name, std::string img1_file, std::string img2_name, std::string img2_file, std::string img3_name, std::string img3_file, CONNECTION cn = JOIN)
	{
		int a1 = add(img1_name, img1_file);
		int a2 = add(img2_name, img2_file);
		int a3 = add(img3_name, img3_file);
		switch (cn)
		{
		default:
		case JOIN:
			bind(a1, a3);
			bind(a2, a3);
			break;

		case SPLIT:
			bind(a1, a2);
			bind(a1, a3);
			break;

		case ROW:
			bind(a1, a2);
			bind(a2, a3);
		}
	}
};

class Illustrations
{
	Ogre::Root* mRoot;
	RenderWindow* mWindow;
	SceneManager* mSceneMgr;
	Camera* mCamera;

	std::vector<Entity*> mEntities;
	std::vector<SceneNode*> mSceneNodes;

	Ogre::TexturePtr mRttTexture;
	Ogre::RenderTexture* mRenderTexture;

public:
	String mOutputPath;

	bool init();
	void go();
	void next(std::string name, Real size);
	void putMesh(MeshPtr mesh, int materialIndex=0);
	void exportImage(std::string name, Procedural::TextureBufferPtr buffer, bool reset = false);

	void cameraPerspective()
	{
		mCamera->setPosition(3,5,-5);
		mCamera->lookAt(0,0,0);
	}

	void cameraBack()
	{
		mCamera->setPosition(0,0,5);
		mCamera->lookAt(0,0,0);
	}

	void cameraFront()
	{
		mCamera->setPosition(0,0,-5);
		mCamera->lookAt(0,0,0);
	}

};

#endif // #ifndef __TutorialApplication_h_
