#include "Ogre_glTF_OgreResource.hpp"
#include "Ogre_glTF.hpp"

void Ogre_glTF::GlbFile::readFromStream(Ogre::DataStreamPtr& stream)
{
	data.resize(stream->size());
	stream->read(reinterpret_cast<void*>(data.data()), data.size());

	if(calculateSize() < 20) throw FileIOError("GLB file needs to be at least 20 bytes long. This cannot be possibly valid!");
	{
		char magic[5];
		memcpy(magic, getData(), 4);
		magic[4] = 0;

		if(std::string(magic) != "glTF") throw InitError("GLB files needs to start with 0x46546C67 \"glTF\" magic number!");
	}
}

void Ogre_glTF::GlbFile::loadImpl()
{
	auto stream = Ogre::ResourceGroupManager::getSingleton().openResource(mName, mGroup, true, this);
	readFromStream(stream);
}

void Ogre_glTF::GlbFile::unloadImpl()
{
	data.clear();
	data.resize(0);
}

size_t Ogre_glTF::GlbFile::calculateSize() const { return getSize(); }

Ogre_glTF::GlbFile::GlbFile(Ogre::ResourceManager* creator,
							const Ogre::String& name,
							Ogre::ResourceHandle handle,
							const Ogre::String& group,
							bool isManual,
							Ogre::ManualResourceLoader* loader) :
 Resource(creator, name, handle, group, isManual, loader)
{
	createParamDictionary("GlbFile");
}

Ogre_glTF::GlbFile::~GlbFile() { GlbFile::unload(); }

const Ogre_glTF::GlbFile::byte* Ogre_glTF::GlbFile::getData() const { return data.data(); }

size_t Ogre_glTF::GlbFile::getSize() const { return data.size(); }

Ogre::Resource* Ogre_glTF::GlbFileManager::createImpl(const Ogre::String& name,
													  Ogre::ResourceHandle handle,
													  const Ogre::String& group,
													  bool isManual,
													  Ogre::ManualResourceLoader* loader,
													  const Ogre::NameValuePairList* createParams)
{
	return OGRE_NEW GlbFile(this, name, handle, group, isManual, loader);
}

Ogre_glTF::GlbFileManager::GlbFileManager()
{
	mResourceType = "GlbFile";
	mLoadOrder	= 2;
	Ogre::ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
}

Ogre_glTF::GlbFileManager::~GlbFileManager() { Ogre::ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType); }

Ogre_glTF::GlbFilePtr Ogre_glTF::GlbFileManager::load(const Ogre::String& name, const Ogre::String& group)
{
	auto file = createOrRetrieve(name, group).first.staticCast<GlbFile>();
	file->load();
	return file;
}

template <>
Ogre_glTF::GlbFileManager* Ogre::Singleton<Ogre_glTF::GlbFileManager>::msSingleton = nullptr;

Ogre_glTF::GlbFileManager& Ogre_glTF::GlbFileManager::getSingleton() { return *msSingleton; }

Ogre_glTF::GlbFileManager* Ogre_glTF::GlbFileManager::getSingletonPtr() { return msSingleton; }
