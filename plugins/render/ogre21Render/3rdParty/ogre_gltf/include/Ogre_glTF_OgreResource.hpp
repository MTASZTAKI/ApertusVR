#pragma once

#include <OgreSharedPtr.h>
#include <OgreResourceManager.h>

#include "Ogre_glTF_DLL.hpp"

namespace Ogre_glTF
{
	///Represet a GlbFile resource as outputed by the resource manager
	class Ogre_glTF_EXPORT GlbFile : public Ogre::Resource
	{
		///Internally use a type called "byte" to represent a byte
		using byte = Ogre::uint8;

		///Loaded data is a plain old string of bytes
		std::vector<byte> data;

		///Ogre does it's thing, and give you a "data stream". This fetch every byte out of that stream, and write it inside the vector of byte
		void readFromStream(Ogre::DataStreamPtr& stream);

	protected:
		///Ogre resource API: called by "load"
		void loadImpl() override;

		///Called by unload
		void unloadImpl() override;

		///Return how many bytes are stored
		size_t calculateSize() const override;

	public:
		///Construct a resource. This is intened to be done by OgreResourceManager, not by you really
		GlbFile(Ogre::ResourceManager* creator,
				 const Ogre::String& name,
				 Ogre::ResourceHandle handle,
				 const Ogre::String& group,
				 bool isManual						= false,
				 Ogre::ManualResourceLoader* loader = nullptr);

		///Resource unloading, will dispose of memory
		virtual ~GlbFile();

		///Get the address of the data. This is effectively the start of the underlying vector
		const byte* getData() const;

		///Get the number of bytes stored. This is effectivly the size of the vector
		size_t getSize() const override;
	};

	///Define a pointer type
	using GlbFilePtr = Ogre::SharedPtr<GlbFile>;

	///File manager. initialized by the plugin, this is for Ogre, not for you
	class /*Ogre_glTF_EXPORT*/ GlbFileManager : public Ogre::ResourceManager, public Ogre::Singleton<GlbFileManager>
	{
	protected:
		///This effectively create a resource
		Ogre::Resource* createImpl(const Ogre::String& name, Ogre::ResourceHandle handle, const Ogre::String& group, bool isManual, Ogre::ManualResourceLoader* loader, const Ogre::NameValuePairList* createParams) override;

	public:
		///Create the GlbFileManager. will also initialize the singleton
		GlbFileManager();

		///Will release the singleton
		virtual ~GlbFileManager();

		///Load a GLB and return a GlbFile object (using a smart pointer)
		virtual GlbFilePtr load(const Ogre::String& name, const Ogre::String& group);

		///Return the single instance of the manager
		static GlbFileManager& getSingleton();

		///Return the pointer to the single instance of the manager
		static GlbFileManager* getSingletonPtr();
	};

}