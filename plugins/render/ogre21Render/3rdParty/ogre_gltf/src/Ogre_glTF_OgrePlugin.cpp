#include "Ogre_glTF_OgrePlugin.hpp"

Ogre_glTF::glTFLoaderPlugin* gPluginInstaller = nullptr;

extern "C" {

void Ogre_glTF_EXPORT dllStartPlugin(void)
{
	if(gPluginInstaller)
	{
		//In normal usage situation of this plugin, this function is declared inside an extern "C" block
		//but is never called from non-C++ code. The only reason this function is extern "C" is for being
		//able to retrieve it's address by name independently of compiler mangling.
		//We should ignore any warning about this.
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4297)
#endif
		throw Ogre_glTF::InitError("Apparently called dllStartPlugin on the Ogre_glTF"
								   "plugin twice. I'm afraid you can't do that... ");
#ifdef _MSC_VER
#pragma warning(pop)
#endif
	}

	gPluginInstaller = new Ogre_glTF::glTFLoaderPlugin;
	Ogre::Root::getSingleton().installPlugin(gPluginInstaller);
}

void Ogre_glTF_EXPORT dllStopPlugin(void)
{
	Ogre::Root::getSingleton().uninstallPlugin(gPluginInstaller);
	delete gPluginInstaller;
	gPluginInstaller = nullptr;
}
}

Ogre_glTF::glTFLoaderPlugin::glTFLoaderPlugin() : Ogre::Plugin() {}

Ogre_glTF::glTFLoaderPlugin::~glTFLoaderPlugin() = default;

const Ogre::String& Ogre_glTF::glTFLoaderPlugin::getName() const { return name; }

void Ogre_glTF::glTFLoaderPlugin::install() {}

void Ogre_glTF::glTFLoaderPlugin::initialise() { gltf = std::make_unique<Ogre_glTF::glTFLoader>(); }

void Ogre_glTF::glTFLoaderPlugin::shutdown() { gltf = nullptr; }

void Ogre_glTF::glTFLoaderPlugin::uninstall() {}
