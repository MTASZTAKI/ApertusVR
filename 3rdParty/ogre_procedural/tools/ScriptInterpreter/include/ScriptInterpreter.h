/*
-----------------------------------------------------------------------------
This source file is part of ogre-procedural

For the latest info, see http://code.google.com/p/ogre-procedural/

Copyright (c) 2010-2012 Michael Broutin

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

#ifndef SCRIPTINTERPRETER_H
#define SCRIPTINTERPRETER_H

#include "BaseApplication.h"
#include "Procedural.h"
#include "Ogre.h"
using namespace Ogre;
using namespace Procedural;

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

class dotFile
{
public:
	enum CONNECTION {
		JOIN, SPLIT, ROW
	};

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

public:
	dotFile(std::string file, std::string name = "")
	{
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

	bool save(std::string path)
	{
		std::ofstream dotfile((mThis.mFile + ".gv").c_str());
		if (dotfile.is_open())
		{
			dotfile << "digraph " << mThis.mName << " {" << std::endl;
			dotfile << "\tratio = \"auto\";" << std::endl << "\tmincross = 2.0;" << std::endl << std::endl;
			for (std::map<int, sItem>::iterator iter = mList.begin(); iter != mList.end(); iter++)
			{
				if (iter->second.mFile.length() > 3)
					dotfile << "\tIMG" << StringConverter::toString(iter->first) << " [shape=box,label=\"" << iter->second.mName << "\",image=\"" << path << "/" << iter->second.mFile << ".png\",labelloc=b];" << std::endl;
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

class ScriptInterpreter : public BaseApplication
{
	Ogre::OverlayElement* mTextMessage;

	void reloadScript();
	void checkScriptModified();

	enum ScriptSourceMode
	{
	    SSM_RESOURCES, SSM_SCRIPTDIR, SSM_SCRIPTFILE
	};

	std::map<std::string, long> mExecutionTimes;

	ScriptSourceMode mScriptSourceMode;

	std::string mScriptFileName;
	std::string mScriptDir;
	bool mBatchMode;
	bool mPerformanceMode;
	bool mWriteToDisk;
	int mCurrentPerformanceIndex;

	void writeEveythingToDisk();

	void writePerformanceFile();

	void reloadScriptNameFromIndex()
	{
		StringVectorPtr scripts = ResourceGroupManager::getSingleton().findResourceNames("Scripts", "*.lua");
		mCurrentScriptName= (*scripts)[mCurrentScriptIndex];
	}

protected:
	bool keyReleased( const OIS::KeyEvent& arg );

	bool keyPressed(const OIS::KeyEvent& arg)
	{
		return BaseApplication::keyPressed(arg);
	}

	virtual void createScene(void);

	virtual void createCamera(void);

	virtual void createViewports(void);

	virtual bool frameStarted(const FrameEvent& evt);

	virtual void destroyScene(void);

	virtual void createLogManager(void);

	static ScriptInterpreter* mInstance;

	std::vector<Entity*> mEntities;
	std::vector<MaterialPtr> mMaterials;
	std::vector<SceneNode*> mSceneNodes;
	std::vector<TexturePtr> mTextures;
	dotFile* mCurrentDotFile;

	TriangleBuffer mDefaultTriangleBuffer;

	Ogre::String mCurrentScriptName;
	time_t mCurrentScriptReloadTime;
	size_t mCurrentScriptIndex;

public:
	bool processInput(int argc, char* argv[]);

	void addTextureBuffer(const TextureBuffer* tb)
	{
		addTriangleTextureBuffer(&mDefaultTriangleBuffer, tb);
	}

	void addTriangleBuffer(const TriangleBuffer* tb)
	{
		std::string s = Utils::getName();
		tb->transformToMesh(s);
		addMesh(s.c_str());
	}

	void addTriangleBufferTexture(const TriangleBuffer* tb, const std::string& texName)
	{
		std::string meshId = Utils::getName();
		std::string matId = Utils::getName();
		tb->transformToMesh(meshId);
		addMaterial(texName.c_str(), matId.c_str());
		addMesh(meshId.c_str(), matId.c_str());
	}

	void addTriangleTextureBuffer(const TriangleBuffer* tb, const TextureBuffer* texb)
	{
		std::string meshId = Utils::getName();
		std::string texId = Utils::getName();
		std::string matId = Utils::getName();
		tb->transformToMesh(meshId);
		texb->createTexture(texId);
		addMaterial(texId.c_str(), matId.c_str());
		addMesh(meshId.c_str(), matId.c_str());
	}

	void addShape(const Shape* shape)
	{
		std::string meshId = Utils::getName();
		shape->realizeMesh(meshId);
		addMesh(meshId.c_str());
	}

	void addPath(const Path* path)
	{
		std::string meshId = Utils::getName();
		path->realizeMesh(meshId);
		addMesh(meshId.c_str());
	}

	void addMesh(const char* meshName, const char* materialName="Examples/Rockwall")
	{
		Entity* entity = mSceneMgr->createEntity(meshName);
		entity->setMaterialName(materialName);
		SceneNode* sceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		sceneNode->attachObject(entity);
		mSceneNodes.push_back(sceneNode);
		mEntities.push_back(entity);
	}

	void addMaterial(const char* textureName, const char* materialName)
	{
		Ogre::MaterialPtr demoMaterial = Ogre::MaterialManager::getSingletonPtr()->create(std::string(materialName), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		demoMaterial->getTechnique(0)->getPass(0)->setDiffuse(Ogre::ColourValue::White);
		demoMaterial->getTechnique(0)->getPass(0)->createTextureUnitState(std::string(textureName));
		mMaterials.push_back(demoMaterial);
		TexturePtr tex = TextureManager::getSingleton().getByName(textureName);
		mTextures.push_back(tex);
	}

	dotFile* getDotFile(const char* file, const char* name)
	{
		mCurrentDotFile = new dotFile(file, name);
		return mCurrentDotFile;
	}

	static ScriptInterpreter* getInstance()
	{
		return mInstance;
	}

	ScriptInterpreter()
	{
		mInstance = this;
		mCurrentScriptIndex = 0;
		mCurrentScriptReloadTime = 0;
		mPerformanceMode = false;
		mCurrentPerformanceIndex = 0;
		mNonExclusiveMouse = true;
		mCurrentDotFile = 0;
	}

	~ScriptInterpreter()
	{
		delete mCurrentDotFile;
	}



};

#endif
