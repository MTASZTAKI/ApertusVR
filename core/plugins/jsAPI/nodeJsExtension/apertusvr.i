%module(directors="1") apertusvr

%import "ApeGeometry.h"

// include Singleton template first
%include "ApeSingleton.h"

// list singleton templates
namespace Ape {
    %template(IEventManager) Singleton<IEventManager>;
    %template(IPluginManager) Singleton<IPluginManager>;
    %template(IScene) Singleton<IScene>;
};

%feature("director") IScene::Singleton<IScene>;

%include "ApeIEventManager.h"
%include "ApeIPluginManager.h"
%include "ApeIScene.h"

%{
// Apertus Common Includes
#include "node_pointer.h"
#include "ApeColor.h"
#include "ApeDegree.h"
#include "ApeEntity.h"
#include "ApeEuler.h"
#include "ApeEvent.h"
#include "ApeIEventManager.h"
#include "ApeGeometry.h"
#include "ApeITextGeometry.h"
#include "ApeILight.h"
#include "ApeIManualGeometry.h"
#include "ApeINode.h"
#include "ApeIPluginManager.h"
#include "ApeIScene.h"
#include "ApeIFileGeometry.h"
#include "ApeMaterial.h"
#include "ApeQuaternion.h"
#include "ApeRadian.h"
#include "ApeSystem.h"
#include "ApeITexture.h"
#include "ApeVector2.h"
#include "ApeVector3.h"
#include "ApeVector4.h"
using namespace Ape;
%}

%inline %{
typedef std::shared_ptr<Ape::INode> NodeSharedPtr;
typedef std::weak_ptr<Ape::INode> NodeWeakPtr;

typedef std::vector<Ape::NodeSharedPtr> NodeSharedPtrVector;
typedef std::map<std::string, Ape::NodeSharedPtr> NodeSharedPtrNameMap;
%}

// include common headers
%include <std_string.i>
%include "ApeColor.h"
%include "ApeDegree.h"
%include "ApeEntity.h"
%include "ApeEuler.h"
%include "ApeEvent.h"
#include "ApeGeometry.h"
%include "ApeITextGeometry.h"
%include "ApeILight.h"
%include "ApeIManualGeometry.h"
%include "ApeINode.h"
%include "ApeIFileGeometry.h"
%include "ApeMaterial.h"
%include "ApeQuaternion.h"
%include "ApeRadian.h"
%include "ApeSystem.h"
%include "ApeITexture.h"
%include "ApeVector2.h"
%include "ApeVector3.h"
%include "ApeVector4.h"
