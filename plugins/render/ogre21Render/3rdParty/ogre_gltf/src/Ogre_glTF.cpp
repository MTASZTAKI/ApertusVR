#include <utility>

#include "Ogre_glTF.hpp"
#include "Ogre_glTF_modelConverter.hpp"
#include "Ogre_glTF_textureImporter.hpp"
#include "Ogre_glTF_materialLoader.hpp"
#include "Ogre_glTF_skeletonImporter.hpp"
#include "Ogre_glTF_common.hpp"
#include "Ogre_glTF_OgreResource.hpp"
#include "Ogre_glTF_internal_utils.hpp"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

#include <OgreItem.h>
#include <OgreMesh2.h>
#include <Animation/OgreTagPoint.h>

using namespace Ogre_glTF;

///Implementaiton of the adapter
struct loaderAdapter::impl
{
	///Constructor, initialize once all the objects inclosed in this class. They need a reference
	///to a model object (and sometimes more) given at construct time
	impl() : textureImp(model), materialLoad(model, textureImp), modelConv(model), skeletonImp(model) {}

	///Variable to check if everything is alright with the adapter
	bool valid = false;

	///The model object that data will be loaded into and read from
	tinygltf::Model model;

	///Where tinygltf will write it's error status
	std::string error = "";

	///Where tinygltf will write it's warning messages
	std::string warnings = "";

	///Texture importer object : go through the texture array and load them into Ogre
	textureImporter textureImp;

	///Material loader : get the data from the material section of the glTF file and create an HlmsDatablock to use
	materialLoader materialLoad;

	///Model converter : load all the actual mesh data from the glTF file, and convert them into index and vertex buffer that can
	///be used to create an Ogre VAO (Vertex Array Object), then create a mesh for it
	modelConverter modelConv;

	///Skeleton importer : load skins from the glTF model, create equivalent OgreSkeleton objects
	skeletonImporter skeletonImp;
};

loaderAdapter::loaderAdapter() : pimpl { std::make_unique<impl>() } { OgreLog("Created adapter object..."); }

loaderAdapter::~loaderAdapter() { OgreLog("Destructed adapter object..."); }

void loaderAdapter::loadMainScene(Ogre::SceneNode* parentNode, Ogre::SceneManager* smgr) const
{
	if(!isOk())
		return;

	pimpl->textureImp.loadTextures();
	auto sceneIdx = pimpl->model.defaultScene >= 0 ? pimpl->model.defaultScene : 0;
	const auto& scene = pimpl->model.scenes[sceneIdx];

	for(auto nodeIdx : scene.nodes)
	{
		getSceneNode(nodeIdx, parentNode, smgr);
	}
}

Ogre::SceneNode* loaderAdapter::getFirstSceneNode(Ogre::SceneManager* smgr) const
{
	if(!isOk())
		return nullptr;
	
	pimpl->textureImp.loadTextures();
	return getSceneNode(pimpl->model.scenes[0].nodes[0], smgr->getRootSceneNode(), smgr);
}

Ogre::SceneNode* loaderAdapter::getSceneNode(size_t index, Ogre::SceneNode* parentSceneNode, Ogre::SceneManager* smgr) const
{
	assert(index < pimpl->model.nodes.size());

	const auto& node = pimpl->model.nodes[index];
	// Check if node is not a bone
	for(const auto& skin : pimpl->model.skins)
	{
		if(std::find(skin.joints.begin(), skin.joints.end(), index) != skin.joints.end())
		{
			return nullptr;
		}
	}

	auto sceneNode = parentSceneNode->createChildSceneNode();
	sceneNode->setName(node.name);
	
	if(!node.translation.empty())
		sceneNode->setPosition(node.translation[0], node.translation[1], node.translation[2]);

	if(!node.rotation.empty())
		sceneNode->setOrientation(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);

	if(!node.scale.empty())
		sceneNode->setScale(node.scale[0], node.scale[1], node.scale[2]);

	if(!node.matrix.empty())
	{
		std::array<Ogre::Real, 4 * 4> matrixArray { 0 };
		internal_utils::container_double_to_real(node.matrix, matrixArray);
		Ogre::Matrix4 matrix { matrixArray.data() };
		Ogre::Vector3 position;
		Ogre::Quaternion orientation;
		Ogre::Vector3 scale;
		matrix.transpose().decomposition(position, scale, orientation);
		sceneNode->setPosition(position);
		sceneNode->setOrientation(orientation);
		sceneNode->setScale(scale);
	}

	if(node.mesh >= 0)
	{
		auto ogreMesh = pimpl->modelConv.getOgreMesh(node.mesh);

		if(node.skin >= 0)
		{
			auto skeleton = this->pimpl->skeletonImp.getSkeleton(node.skin);
			if(skeleton)
			{
				ogreMesh->_notifySkeleton(skeleton);
			}
		}

		auto item = smgr->createItem(ogreMesh);
		const auto& mesh = pimpl->model.meshes[node.mesh];
		for(size_t i = 0; i < mesh.primitives.size(); ++i) 
		{ 
			auto subItem = item->getSubItem(i);
			subItem->setDatablock(getDatablock(mesh.primitives[i].material));
		}
		sceneNode->attachObject(item);

		// Add tag points
		auto skeletonInstance = item->getSkeletonInstance();
		if(skeletonInstance)
		{
			// Find all root bones. Collect all children of all nodes in the skin and then find
			// all nodes in the skin that are not in the set of children. Those are all nodes
			// that don't have a parent because they're not a child of any other node. 
			std::vector<int> rootBones;
			std::vector<int> allChildren;
			const auto& skin = pimpl->model.skins[node.skin];

			for(const auto& nodeIndex : skin.joints)
			{
				const auto& node = pimpl->model.nodes[nodeIndex];
				allChildren.insert(allChildren.end(), node.children.begin(), node.children.end());
			}

			for(const auto& nodeIndex : skin.joints)
			{
				if(std::find(allChildren.begin(), allChildren.end(), nodeIndex) == allChildren.end()) {
					rootBones.push_back(nodeIndex);
				}
			}

			for(int boneIndex : rootBones)
			{
				createTagPoints(boneIndex, skeletonInstance, smgr);
			}
		}
	}

	for(const auto& child : node.children)
	{
		getSceneNode(child, sceneNode, smgr);
	}

	return sceneNode;
}

void loaderAdapter::createTagPoints(int boneIndex, Ogre::SkeletonInstance* skeletonInstance, Ogre::SceneManager* smgr) const
{
	const auto& boneNode = pimpl->model.nodes[boneIndex];

	for(auto child : boneNode.children)
	{
		const auto& childNode = pimpl->model.nodes[child];

		if(childNode.mesh >= 0)
		{
			auto tagPoint = smgr->createTagPoint();
			tagPoint->setName(childNode.name);

			Ogre::Vector3 position;
			Ogre::Quaternion orientation;
			Ogre::Vector3 scale(1);
			
			if(!childNode.translation.empty())
				position = Ogre::Vector3(childNode.translation[0], childNode.translation[1], childNode.translation[2]);

			if(!childNode.rotation.empty())
				orientation = Ogre::Quaternion(childNode.rotation[3], childNode.rotation[0], childNode.rotation[1], childNode.rotation[2]);

			if(!childNode.scale.empty())
				scale = Ogre::Vector3(childNode.scale[0], childNode.scale[1], childNode.scale[2]);

			if(!childNode.matrix.empty())
			{
				std::array<Ogre::Real, 4 * 4> matrixArray { 0 };
				internal_utils::container_double_to_real(childNode.matrix, matrixArray);
				Ogre::Matrix4 matrix { matrixArray.data() };
				matrix.transpose().decomposition(position, scale, orientation);
			}

			tagPoint->setPosition(position);
			tagPoint->setOrientation(orientation);
			tagPoint->setScale(scale);

			auto ogreMesh = pimpl->modelConv.getOgreMesh(childNode.mesh);

			if(childNode.skin >= 0)
			{
				auto skeleton = this->pimpl->skeletonImp.getSkeleton(childNode.skin);
				if(skeleton)
				{
					ogreMesh->_notifySkeleton(skeleton);
				}
			}

			auto item = smgr->createItem(ogreMesh);
			const auto& mesh = pimpl->model.meshes[childNode.mesh];
			for(size_t i = 0; i < mesh.primitives.size(); ++i) 
			{ 
				auto subItem = item->getSubItem(i);
				subItem->setDatablock(getDatablock(mesh.primitives[i].material));
			}
			tagPoint->attachObject(item);

			auto parentBone = skeletonInstance->getBone(boneNode.name);
			parentBone->addTagPoint(tagPoint);

			for(const auto& childOfChild : childNode.children)
			{
				getSceneNode(childOfChild, tagPoint, smgr);
			}
		}
		else
		{
			createTagPoints(child, skeletonInstance, smgr);
		}
	}
}

Ogre::HlmsDatablock* loaderAdapter::getDatablock(size_t index) const { return pimpl->materialLoad.getDatablock(index); }

size_t loaderAdapter::getDatablockCount() { return pimpl->materialLoad.getDatablockCount(); }

loaderAdapter::loaderAdapter(loaderAdapter&& other) noexcept : pimpl { std::move(other.pimpl) }
{

	OgreLog("Moved adapter object...");
}

loaderAdapter& loaderAdapter::operator=(loaderAdapter&& other) noexcept
{
	pimpl = std::move(other.pimpl);
	return *this;
}

bool loaderAdapter::isOk() const { return pimpl->valid; }

std::string loaderAdapter::getLastError() const { return pimpl->error; }

///Implementation of the glTF loader. Exist as a pImpl inside the glTFLoader class
struct glTFLoader::glTFLoaderImpl
{
	///The loader object from TinyGLTF
	tinygltf::TinyGLTF loader;

	///Constructor. the loader is on the stack, there isn't much state to set inside the object
	glTFLoaderImpl() { OgreLog("initialized TinyGLTF loader"); }

	///For file type detection. Ascii is plain old JSON text, Binary is .glc files.
	enum class FileType { Ascii, Binary, Unknown };

	///Probe inside the file, or check the extension to determine if we have to load a text file, or a binary file
	FileType detectType(const std::string& path) const
	{
		//Quickly open the file as binary and check if there's the gltf binary magic number
		{
			auto probe = std::ifstream(path, std::ios_base::binary);
			if(!probe) throw FileIOError("Could not open " + path);

			std::array<char, 5> buffer {};
			for(size_t i { 0 }; i < 4; ++i) probe >> buffer[i];
			buffer[4] = 0;

			if(std::string("glTF") == std::string(buffer.data()))
			{
				//OgreLog("Detected binary file thanks to the magic number at the start!");
				return FileType::Binary;
			}
		}

		//If we don't have any better, check the file extension.
		auto extension = path.substr(path.find_last_of('.') + 1);
		std::transform(std::begin(extension), std::end(extension), std::begin(extension), [](char c) { return char(tolower(int(c))); });
		if(extension == "gltf") return FileType::Ascii;
		if(extension == "glb") return FileType::Binary;

		return FileType::Unknown;
	}

	///Load the content of a file into an adapter object
	bool loadInto(loaderAdapter& adapter, const std::string& path)
	{
		switch(detectType(path))
		{
			default:
			case FileType::Unknown: return false;
			case FileType::Ascii:
				//OgreLog("Detected ascii file type");
				return loader.LoadASCIIFromFile(&adapter.pimpl->model, &adapter.pimpl->error, &adapter.pimpl->warnings, path);
			case FileType::Binary:
				//OgreLog("Deteted binary file type");
				return loader.LoadBinaryFromFile(&adapter.pimpl->model, &adapter.pimpl->error, &adapter.pimpl->warnings, path);
		}
	}

	bool loadGlb(loaderAdapter& adapter, GlbFilePtr file)
	{
		return loader.LoadBinaryFromMemory(
			&adapter.pimpl->model, &adapter.pimpl->error, &adapter.pimpl->warnings, file->getData(), int(file->getSize()), ".", 0);
	}
};

glTFLoader::glTFLoader() : loaderImpl { std::make_unique<glTFLoaderImpl>() }
{
	if(Ogre::Root::getSingletonPtr() == nullptr) throw RootNotInitializedYet("Please create an Ogre::Root instance before initializing the glTF library!");

	if(!Ogre_glTF::GlbFileManager::getSingletonPtr()) new GlbFileManager;

	OgreLog("glTFLoader created!");
}

loaderAdapter glTFLoader::loadFromFileSystem(const std::string& path) const
{
	OgreLog("loading file " + path);
	loaderAdapter adapter;
	adapter.adapterName = path;
	loaderImpl->loadInto(adapter, path);

	//if (adapter.getLastError().empty())
	{
		OgreLog("Debug : it looks like the file was loaded without error!");
		adapter.pimpl->valid = true;
	}

	adapter.pimpl->modelConv.debugDump();
	return adapter;
}

loaderAdapter glTFLoader::loadGlbResource(const std::string& name) const
{
	OgreLog("Loading GLB from resource manager " + name);
	auto& glbManager = GlbFileManager::getSingleton();
	auto glbFile	 = glbManager.load(name, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);

	loaderAdapter adapter;
	if(glbFile)
	{
		loaderImpl->loadGlb(adapter, glbFile);
		adapter.pimpl->valid = true;
	}

	adapter.pimpl->modelConv.debugDump();
	return adapter;
}

glTFLoader::glTFLoader(glTFLoader&& other) noexcept : loaderImpl(std::move(other.loaderImpl)) {}

glTFLoader& glTFLoader::operator=(glTFLoader&& other) noexcept
{
	loaderImpl = std::move(other.loaderImpl);
	return *this;
}

glTFLoader::~glTFLoader() = default;
