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

#define APE_JNIPLUGIN_CPP

#include "../../core/eventManager/apeEventManagerImpl.h"
#include <map>
#include <apeColor.h>
#include "apeJNIPlugin.h"

#include "apeIFileGeometry.h"

ape::JNIPlugin::JNIPlugin()
{
	APE_LOG_FUNC_ENTER()
    mpSceneManager = ape::ISceneManager::getSingletonPtr();
    mpCoreConfig = ape::ICoreConfig::getSingletonPtr();
    mEventDoubleQueue = ape::DoubleQueue<Event>();
    mpEventManager = ape::IEventManager::getSingletonPtr();

    initEventNumberMap();

    /* JNI plugin acts as a singleton */
    mpThisPlugin = this;

    /* ---- Event connection ---- */
	mpEventManager->connectEvent(ape::Event::Group::NODE, std::bind(&JNIPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::LIGHT, std::bind(&JNIPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::CAMERA, std::bind(&JNIPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_FILE, std::bind(&JNIPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_TEXT, std::bind(&JNIPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_PLANE, std::bind(&JNIPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_BOX, std::bind(&JNIPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_CYLINDER, std::bind(&JNIPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_CONE, std::bind(&JNIPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_TUBE, std::bind(&JNIPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_SPHERE, std::bind(&JNIPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_TORUS, std::bind(&JNIPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_INDEXEDFACESET, std::bind(&JNIPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_INDEXEDLINESET, std::bind(&JNIPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::MATERIAL_FILE, std::bind(&JNIPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::MATERIAL_MANUAL, std::bind(&JNIPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::TEXTURE_MANUAL, std::bind(&JNIPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::TEXTURE_FILE, std::bind(&JNIPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::TEXTURE_UNIT, std::bind(&JNIPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::GEOMETRY_RAY, std::bind(&JNIPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::SKY, std::bind(&JNIPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::WATER, std::bind(&JNIPlugin::eventCallBack, this, std::placeholders::_1));
    mpEventManager->connectEvent(ape::Event::Group::POINT_CLOUD, std::bind(&JNIPlugin::eventCallBack, this, std::placeholders::_1));
	APE_LOG_FUNC_LEAVE()
}

void ape::JNIPlugin::Init()
{
	APE_LOG_FUNC_ENTER();
	printf("jni plugin init\n");
	APE_LOG_FUNC_LEAVE();
}

void ape::JNIPlugin::eventCallBack(const ape::Event& event)
{
//    printf("event type: %d, subject name: %s\n",event.group,event.subjectName.c_str());

    __android_log_print(ANDROID_LOG_INFO,"cpplog","event type: %s, subject name: %s\n", mEventTypeNameMap[event.type].c_str(),event.subjectName.c_str());
    if(event.type == Event::Type::GEOMETRY_FILE_FILENAME) {
        if (auto fileGeom = std::static_pointer_cast<ape::IFileGeometry>(
                mpSceneManager->getEntity(event.subjectName).lock()))
        {
            __android_log_print(ANDROID_LOG_INFO, "cpplog", "File name: %s",
                                fileGeom->getFileName().c_str());
        }
    }

//    if(event.type == ape::Event::Type::GEOMETRY_FILE_PARENTNODE ||
////       event.type == ape::Event::Type::GEOMETRY_PLANE_PARENTNODE)
////    {
////        if(auto geomShared = std::static_pointer_cast<ape::Geometry>(mpSceneManager->getEntity(event.subjectName).lock()))
////        {
////            if(auto parentShared = geomShared->getParentNode().lock())
////            {
////                std::string parentNodeName = parentShared->getName();
////                mNodeGeometryMap[parentNodeName] = geomShared;
////            }
////        }
////    }

	mEventDoubleQueue.push(event);
}

void ape::JNIPlugin::processEventDoubleQueue()
{
}

void ape::JNIPlugin::Run()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::JNIPlugin::Step()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::JNIPlugin::Stop()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::JNIPlugin::Suspend()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

void ape::JNIPlugin::Restart()
{
	APE_LOG_FUNC_ENTER();
	APE_LOG_FUNC_LEAVE();
}

ape::JNIPlugin* ape::JNIPlugin::mpThisPlugin;

ape::JNIPlugin* ape::JNIPlugin::getPluginPtr()
{
    return mpThisPlugin;
}

ape::ISceneManager *ape::JNIPlugin::getSceneManager()
{
    return mpSceneManager;
}

ape::DoubleQueue<ape::Event> *ape::JNIPlugin::getEventDoubleQueue()
{
    return &mEventDoubleQueue;
}

std::map<ape::Event::Type, int> ape::JNIPlugin::getEventNumberMap()
{
    return mEventNumberMap;
}

void ape::JNIPlugin::initEventNumberMap()
{
    std::vector<ape::Event::Type> eventTypes
    {
            ape::Event::Type::NODE_CREATE, /*= (NODE << 8) + 1,*/
            ape::Event::Type::NODE_DELETE,
            ape::Event::Type::NODE_POSITION,
            ape::Event::Type::NODE_ORIENTATION,
            ape::Event::Type::NODE_SCALE,
            ape::Event::Type::NODE_PARENTNODE,
            ape::Event::Type::NODE_DETACH,
            ape::Event::Type::NODE_CHILDVISIBILITY,
            ape::Event::Type::NODE_VISIBILITY,
            ape::Event::Type::NODE_FIXEDYAW,
            ape::Event::Type::NODE_SHOWBOUNDINGBOX,
            ape::Event::Type::NODE_HIDEBOUNDINGBOX,
            ape::Event::Type::NODE_INHERITORIENTATION,
            ape::Event::Type::NODE_INITIALSTATE,
            ape::Event::Type::LIGHT_CREATE, /*= (LIGHT << 8) + 1,*/
            ape::Event::Type::LIGHT_DELETE,
            ape::Event::Type::LIGHT_TYPE,
            ape::Event::Type::LIGHT_DIFFUSE,
            ape::Event::Type::LIGHT_SPECULAR,
            ape::Event::Type::LIGHT_ATTENUATION,
            ape::Event::Type::LIGHT_DIRECTION,
            ape::Event::Type::LIGHT_SPOTRANGE,
            ape::Event::Type::LIGHT_PARENTNODE,
            ape::Event::Type::GEOMETRY_FILE_CREATE, /*= (GEOMETRY_FILE << 8) + 1,*/
            ape::Event::Type::GEOMETRY_FILE_DELETE,
            ape::Event::Type::GEOMETRY_FILE_FILENAME,
            ape::Event::Type::GEOMETRY_FILE_EXPORT,
            ape::Event::Type::GEOMETRY_FILE_MERGESUBMESHES,
            ape::Event::Type::GEOMETRY_FILE_MATERIAL,
            ape::Event::Type::GEOMETRY_FILE_PARENTNODE,
            ape::Event::Type::GEOMETRY_FILE_VISIBILITY,
            ape::Event::Type::GEOMETRY_INDEXEDFACESET_CREATE, /*= (GEOMETRY_INDEXEDFACESET << 8) + 1,*/
            ape::Event::Type::GEOMETRY_INDEXEDFACESET_PARAMETERS,
            ape::Event::Type::GEOMETRY_INDEXEDFACESET_MATERIAL,
            ape::Event::Type::GEOMETRY_INDEXEDFACESET_PARENTNODE,
            ape::Event::Type::GEOMETRY_INDEXEDFACESET_DELETE,
            ape::Event::Type::GEOMETRY_INDEXEDLINESET_CREATE, /*= (GEOMETRY_INDEXEDLINESET << 8) + 1,*/
            ape::Event::Type::GEOMETRY_INDEXEDLINESET_PARAMETERS,
            ape::Event::Type::GEOMETRY_INDEXEDLINESET_PARENTNODE,
            ape::Event::Type::GEOMETRY_INDEXEDLINESET_DELETE,
            ape::Event::Type::GEOMETRY_TEXT_CREATE, /*= (GEOMETRY_TEXT << 8) + 1,*/
            ape::Event::Type::GEOMETRY_TEXT_DELETE,
            ape::Event::Type::GEOMETRY_TEXT_VISIBLE,
            ape::Event::Type::GEOMETRY_TEXT_SHOWONTOP,
            ape::Event::Type::GEOMETRY_TEXT_CAPTION,
            ape::Event::Type::GEOMETRY_TEXT_PARENTNODE,
            ape::Event::Type::GEOMETRY_PLANE_CREATE, /*= (GEOMETRY_PLANE << 8) + 1,*/
            ape::Event::Type::GEOMETRY_PLANE_PARAMETERS,
            ape::Event::Type::GEOMETRY_PLANE_MATERIAL,
            ape::Event::Type::GEOMETRY_PLANE_PARENTNODE,
            ape::Event::Type::GEOMETRY_PLANE_DELETE,
            ape::Event::Type::GEOMETRY_BOX_CREATE, /*= (GEOMETRY_BOX << 8) + 1,*/
            ape::Event::Type::GEOMETRY_BOX_PARAMETERS,
            ape::Event::Type::GEOMETRY_BOX_MATERIAL,
            ape::Event::Type::GEOMETRY_BOX_PARENTNODE,
            ape::Event::Type::GEOMETRY_BOX_DELETE,
            ape::Event::Type::GEOMETRY_SPHERE_CREATE, /*= (GEOMETRY_SPHERE << 8) + 1,*/
            ape::Event::Type::GEOMETRY_SPHERE_PARAMETERS,
            ape::Event::Type::GEOMETRY_SPHERE_MATERIAL,
            ape::Event::Type::GEOMETRY_SPHERE_PARENTNODE,
            ape::Event::Type::GEOMETRY_SPHERE_DELETE,
            ape::Event::Type::GEOMETRY_CONE_CREATE, /*= (GEOMETRY_CONE << 8) + 1,*/
            ape::Event::Type::GEOMETRY_CONE_PARAMETERS,
            ape::Event::Type::GEOMETRY_CONE_MATERIAL,
            ape::Event::Type::GEOMETRY_CONE_PARENTNODE,
            ape::Event::Type::GEOMETRY_CONE_DELETE,
            ape::Event::Type::GEOMETRY_TUBE_CREATE, /*= (GEOMETRY_TUBE << 8) + 1,*/
            ape::Event::Type::GEOMETRY_TUBE_PARAMETERS,
            ape::Event::Type::GEOMETRY_TUBE_MATERIAL,
            ape::Event::Type::GEOMETRY_TUBE_PARENTNODE,
            ape::Event::Type::GEOMETRY_TUBE_DELETE,
            ape::Event::Type::GEOMETRY_TORUS_CREATE, /*= (GEOMETRY_TORUS << 8) + 1,*/
            ape::Event::Type::GEOMETRY_TORUS_PARAMETERS,
            ape::Event::Type::GEOMETRY_TORUS_MATERIAL,
            ape::Event::Type::GEOMETRY_TORUS_PARENTNODE,
            ape::Event::Type::GEOMETRY_TORUS_DELETE,
            ape::Event::Type::GEOMETRY_CYLINDER_CREATE, /*= (GEOMETRY_CYLINDER << 8) + 1,*/
            ape::Event::Type::GEOMETRY_CYLINDER_PARAMETERS,
            ape::Event::Type::GEOMETRY_CYLINDER_MATERIAL,
            ape::Event::Type::GEOMETRY_CYLINDER_PARENTNODE,
            ape::Event::Type::GEOMETRY_CYLINDER_DELETE,
            ape::Event::Type::GEOMETRY_RAY_CREATE, /*= (GEOMETRY_RAY << 8) + 1,*/
            ape::Event::Type::GEOMETRY_RAY_INTERSECTIONENABLE,
            ape::Event::Type::GEOMETRY_RAY_INTERSECTION,
            ape::Event::Type::GEOMETRY_RAY_INTERSECTIONQUERY,
            ape::Event::Type::GEOMETRY_RAY_PARENTNODE,
            ape::Event::Type::GEOMETRY_RAY_DELETE,
            ape::Event::Type::GEOMETRY_CLONE_CREATE, /*= (GEOMETRY_CLONE << 8) + 1,*/
            ape::Event::Type::GEOMETRY_CLONE_SOURCEGEOMETRY,
            ape::Event::Type::GEOMETRY_CLONE_SOURCEGEOMETRYGROUP_NAME,
            ape::Event::Type::GEOMETRY_CLONE_PARENTNODE,
            ape::Event::Type::GEOMETRY_CLONE_DELETE,
            ape::Event::Type::MATERIAL_MANUAL_CREATE, /*= (MATERIAL_MANUAL << 8) + 1,*/
            ape::Event::Type::MATERIAL_MANUAL_AMBIENT,
            ape::Event::Type::MATERIAL_MANUAL_EMISSIVE,
            ape::Event::Type::MATERIAL_MANUAL_DIFFUSE,
            ape::Event::Type::MATERIAL_MANUAL_SPECULAR,
            ape::Event::Type::MATERIAL_MANUAL_CULLINGMODE,
            ape::Event::Type::MATERIAL_MANUAL_SCENEBLENDING,
            ape::Event::Type::MATERIAL_MANUAL_OVERLAY,
            ape::Event::Type::MATERIAL_MANUAL_TEXTURE,
            ape::Event::Type::MATERIAL_MANUAL_MANUALCULLINGMODE,
            ape::Event::Type::MATERIAL_MANUAL_DEPTHBIAS,
            ape::Event::Type::MATERIAL_MANUAL_DEPTHWRITE,
            ape::Event::Type::MATERIAL_MANUAL_DEPTHCHECK,
            ape::Event::Type::MATERIAL_MANUAL_LIGHTING,
            ape::Event::Type::MATERIAL_MANUAL_DELETE,
            ape::Event::Type::MATERIAL_FILE_CREATE, /*= (MATERIAL_FILE << 8) + 1,*/
            ape::Event::Type::MATERIAL_FILE_SETASSKYBOX,
            ape::Event::Type::MATERIAL_FILE_TEXTURE,
            ape::Event::Type::MATERIAL_FILE_GPUPARAMETERS,
            ape::Event::Type::MATERIAL_FILE_FILENAME,
            ape::Event::Type::MATERIAL_FILE_DELETE,
            ape::Event::Type::TEXTURE_MANUAL_CREATE, /*= (TEXTURE_MANUAL << 8) + 1,*/
            ape::Event::Type::TEXTURE_MANUAL_PARAMETERS,
            ape::Event::Type::TEXTURE_MANUAL_SOURCECAMERA,
            ape::Event::Type::TEXTURE_MANUAL_GRAPHICSAPIID,
            ape::Event::Type::TEXTURE_MANUAL_BUFFER,
            ape::Event::Type::TEXTURE_MANUAL_DELETE,
            ape::Event::Type::TEXTURE_FILE_CREATE, /*= (TEXTURE_FILE << 8) + 1,*/
            ape::Event::Type::TEXTURE_FILE_FILENAME,
            ape::Event::Type::TEXTURE_FILE_TYPE,
            ape::Event::Type::TEXTURE_FILE_DELETE,
            ape::Event::Type::TEXTURE_UNIT_CREATE, /*= (TEXTURE_UNIT << 8) + 1,*/
            ape::Event::Type::TEXTURE_UNIT_PARAMETERS,
            ape::Event::Type::TEXTURE_UNIT_SCROLL,
            ape::Event::Type::TEXTURE_UNIT_ADDRESSING,
            ape::Event::Type::TEXTURE_UNIT_FILTERING,
            ape::Event::Type::TEXTURE_UNIT_DELETE,
            ape::Event::Type::CAMERA_CREATE, /*= (CAMERA << 8) + 1,*/
            ape::Event::Type::CAMERA_DELETE,
            ape::Event::Type::CAMERA_FOCALLENGTH,
            ape::Event::Type::CAMERA_ASPECTRATIO,
            ape::Event::Type::CAMERA_AUTOASPECTRATIO,
            ape::Event::Type::CAMERA_FOVY,
            ape::Event::Type::CAMERA_FRUSTUMOFFSET,
            ape::Event::Type::CAMERA_NEARCLIP,
            ape::Event::Type::CAMERA_FARCLIP,
            ape::Event::Type::CAMERA_PROJECTION,
            ape::Event::Type::CAMERA_PARENTNODE,
            ape::Event::Type::CAMERA_PROJECTIONTYPE,
            ape::Event::Type::CAMERA_ORTHOWINDOWSIZE,
            ape::Event::Type::CAMERA_WINDOW,
            ape::Event::Type::CAMERA_VISIBILITY,
            ape::Event::Type::BROWSER_CREATE, /*= (BROWSER << 8) + 1,*/
            ape::Event::Type::BROWSER_URL,
            ape::Event::Type::BROWSER_RESOLUTION,
            ape::Event::Type::BROWSER_GEOMETRY,
            ape::Event::Type::BROWSER_OVERLAY,
            ape::Event::Type::BROWSER_ZOOM,
            ape::Event::Type::BROWSER_MOUSE_CLICK,
            ape::Event::Type::BROWSER_MOUSE_SCROLL,
            ape::Event::Type::BROWSER_MOUSE_MOVED,
            ape::Event::Type::BROWSER_KEY_VALUE,
            ape::Event::Type::BROWSER_FOCUS_ON_EDITABLE_FIELD,
            ape::Event::Type::BROWSER_RELOAD,
            ape::Event::Type::BROWSER_ELEMENT_CLICK,
            ape::Event::Type::BROWSER_HOVER_IN,
            ape::Event::Type::BROWSER_HOVER_OUT,
            ape::Event::Type::BROWSER_MESSAGE,
            ape::Event::Type::BROWSER_DELETE,
            ape::Event::Type::WATER_CREATE, /*= (WATER << 8) + 1,*/
            ape::Event::Type::WATER_SKY,
            ape::Event::Type::WATER_CAMERAS,
            ape::Event::Type::WATER_DELETE,
            ape::Event::Type::SKY_CREATE, /*= (SKY << 8) + 1,*/
            ape::Event::Type::SKY_TIME,
            ape::Event::Type::SKY_SUNLIGHT,
            ape::Event::Type::SKY_SKYLIGHT,
            ape::Event::Type::SKY_SIZE,
            ape::Event::Type::SKY_DELETE,
            ape::Event::Type::POINT_CLOUD_CREATE, /*= (POINT_CLOUD << 8) + 1,*/
            ape::Event::Type::POINT_CLOUD_PARAMETERS,
            ape::Event::Type::POINT_CLOUD_POINTS,
            ape::Event::Type::POINT_CLOUD_COLORS,
            ape::Event::Type::POINT_CLOUD_PARENTNODE,
            ape::Event::Type::POINT_CLOUD_DELETE,
            ape::Event::Type::ET_INVALID,
            ape::Event::Type::RIGIDBODY_CREATE, /*= (PHYSICS << 8) + 1,*/
            ape::Event::Type::RIGIDBODY_DELETE,
            ape::Event::Type::RIGIDBODY_MASS,
            ape::Event::Type::RIGIDBODY_FRICTION,
            ape::Event::Type::RIGIDBODY_DAMPING,
            ape::Event::Type::RIGIDBODY_BOUYANCY,
            ape::Event::Type::RIGIDBODY_RESTITUTION,
            ape::Event::Type::RIGIDBODY_PARENTNODE,
            ape::Event::Type::RIGIDBODY_SHAPE
    };

    std::vector<std::string> eventTypeNames
    {
            "NODE_CREATE", /*= (NODE << 8) + 1,*/
            "NODE_DELETE",
            "NODE_POSITION",
            "NODE_ORIENTATION",
            "NODE_SCALE",
            "NODE_PARENTNODE",
            "NODE_DETACH",
            "NODE_CHILDVISIBILITY",
            "NODE_VISIBILITY",
            "NODE_FIXEDYAW",
            "NODE_SHOWBOUNDINGBOX",
            "NODE_HIDEBOUNDINGBOX",
            "NODE_INHERITORIENTATION",
            "NODE_INITIALSTATE",
            "LIGHT_CREATE", /*= (LIGHT << 8) + 1,*/
            "LIGHT_DELETE",
            "LIGHT_TYPE",
            "LIGHT_DIFFUSE",
            "LIGHT_SPECULAR",
            "LIGHT_ATTENUATION",
            "LIGHT_DIRECTION",
            "LIGHT_SPOTRANGE",
            "LIGHT_PARENTNODE",
            "GEOMETRY_FILE_CREATE", /*= (GEOMETRY_FILE << 8) + 1,*/
            "GEOMETRY_FILE_DELETE",
            "GEOMETRY_FILE_FILENAME",
            "GEOMETRY_FILE_EXPORT",
            "GEOMETRY_FILE_MERGESUBMESHES",
            "GEOMETRY_FILE_MATERIAL",
            "GEOMETRY_FILE_PARENTNODE",
            "GEOMETRY_FILE_VISIBILITY",
            "GEOMETRY_INDEXEDFACESET_CREATE", /*= (GEOMETRY_INDEXEDFACESET << 8) + 1,*/
            "GEOMETRY_INDEXEDFACESET_PARAMETERS",
            "GEOMETRY_INDEXEDFACESET_MATERIAL",
            "GEOMETRY_INDEXEDFACESET_PARENTNODE",
            "GEOMETRY_INDEXEDFACESET_DELETE",
            "GEOMETRY_INDEXEDLINESET_CREATE", /*= (GEOMETRY_INDEXEDLINESET << 8) + 1,*/
            "GEOMETRY_INDEXEDLINESET_PARAMETERS",
            "GEOMETRY_INDEXEDLINESET_PARENTNODE",
            "GEOMETRY_INDEXEDLINESET_DELETE",
            "GEOMETRY_TEXT_CREATE", /*= (GEOMETRY_TEXT << 8) + 1,*/
            "GEOMETRY_TEXT_DELETE",
            "GEOMETRY_TEXT_VISIBLE",
            "GEOMETRY_TEXT_SHOWONTOP",
            "GEOMETRY_TEXT_CAPTION",
            "GEOMETRY_TEXT_PARENTNODE",
            "GEOMETRY_PLANE_CREATE", /*= (GEOMETRY_PLANE << 8) + 1,*/
            "GEOMETRY_PLANE_PARAMETERS",
            "GEOMETRY_PLANE_MATERIAL",
            "GEOMETRY_PLANE_PARENTNODE",
            "GEOMETRY_PLANE_DELETE",
            "GEOMETRY_BOX_CREATE", /*= (GEOMETRY_BOX << 8) + 1,*/
            "GEOMETRY_BOX_PARAMETERS",
            "GEOMETRY_BOX_MATERIAL",
            "GEOMETRY_BOX_PARENTNODE",
            "GEOMETRY_BOX_DELETE",
            "GEOMETRY_SPHERE_CREATE", /*= (GEOMETRY_SPHERE << 8) + 1,*/
            "GEOMETRY_SPHERE_PARAMETERS",
            "GEOMETRY_SPHERE_MATERIAL",
            "GEOMETRY_SPHERE_PARENTNODE",
            "GEOMETRY_SPHERE_DELETE",
            "GEOMETRY_CONE_CREATE", /*= (GEOMETRY_CONE << 8) + 1,*/
            "GEOMETRY_CONE_PARAMETERS",
            "GEOMETRY_CONE_MATERIAL",
            "GEOMETRY_CONE_PARENTNODE",
            "GEOMETRY_CONE_DELETE",
            "GEOMETRY_TUBE_CREATE", /*= (GEOMETRY_TUBE << 8) + 1,*/
            "GEOMETRY_TUBE_PARAMETERS",
            "GEOMETRY_TUBE_MATERIAL",
            "GEOMETRY_TUBE_PARENTNODE",
            "GEOMETRY_TUBE_DELETE",
            "GEOMETRY_TORUS_CREATE", /*= (GEOMETRY_TORUS << 8) + 1,*/
            "GEOMETRY_TORUS_PARAMETERS",
            "GEOMETRY_TORUS_MATERIAL",
            "GEOMETRY_TORUS_PARENTNODE",
            "GEOMETRY_TORUS_DELETE",
            "GEOMETRY_CYLINDER_CREATE", /*= (GEOMETRY_CYLINDER << 8) + 1,*/
            "GEOMETRY_CYLINDER_PARAMETERS",
            "GEOMETRY_CYLINDER_MATERIAL",
            "GEOMETRY_CYLINDER_PARENTNODE",
            "GEOMETRY_CYLINDER_DELETE",
            "GEOMETRY_RAY_CREATE", /*= (GEOMETRY_RAY << 8) + 1,*/
            "GEOMETRY_RAY_INTERSECTIONENABLE",
            "GEOMETRY_RAY_INTERSECTION",
            "GEOMETRY_RAY_INTERSECTIONQUERY",
            "GEOMETRY_RAY_PARENTNODE",
            "GEOMETRY_RAY_DELETE",
            "GEOMETRY_CLONE_CREATE", /*= (GEOMETRY_CLONE << 8) + 1,*/
            "GEOMETRY_CLONE_SOURCEGEOMETRY",
            "GEOMETRY_CLONE_SOURCEGEOMETRYGROUP_NAME",
            "GEOMETRY_CLONE_PARENTNODE",
            "GEOMETRY_CLONE_DELETE",
            "MATERIAL_MANUAL_CREATE", /*= (MATERIAL_MANUAL << 8) + 1,*/
            "MATERIAL_MANUAL_AMBIENT",
            "MATERIAL_MANUAL_EMISSIVE",
            "MATERIAL_MANUAL_DIFFUSE",
            "MATERIAL_MANUAL_SPECULAR",
            "MATERIAL_MANUAL_CULLINGMODE",
            "MATERIAL_MANUAL_SCENEBLENDING",
            "MATERIAL_MANUAL_OVERLAY",
            "MATERIAL_MANUAL_TEXTURE",
            "MATERIAL_MANUAL_MANUALCULLINGMODE",
            "MATERIAL_MANUAL_DEPTHBIAS",
            "MATERIAL_MANUAL_DEPTHWRITE",
            "MATERIAL_MANUAL_DEPTHCHECK",
            "MATERIAL_MANUAL_LIGHTING",
            "MATERIAL_MANUAL_DELETE",
            "MATERIAL_FILE_CREATE", /*= (MATERIAL_FILE << 8) + 1,*/
            "MATERIAL_FILE_SETASSKYBOX",
            "MATERIAL_FILE_TEXTURE",
            "MATERIAL_FILE_GPUPARAMETERS",
            "MATERIAL_FILE_FILENAME",
            "MATERIAL_FILE_DELETE",
            "TEXTURE_MANUAL_CREATE", /*= (TEXTURE_MANUAL << 8) + 1,*/
            "TEXTURE_MANUAL_PARAMETERS",
            "TEXTURE_MANUAL_SOURCECAMERA",
            "TEXTURE_MANUAL_GRAPHICSAPIID",
            "TEXTURE_MANUAL_BUFFER",
            "TEXTURE_MANUAL_DELETE",
            "TEXTURE_FILE_CREATE", /*= (TEXTURE_FILE << 8) + 1,*/
            "TEXTURE_FILE_FILENAME",
            "TEXTURE_FILE_TYPE",
            "TEXTURE_FILE_DELETE",
            "TEXTURE_UNIT_CREATE", /*= (TEXTURE_UNIT << 8) + 1,*/
            "TEXTURE_UNIT_PARAMETERS",
            "TEXTURE_UNIT_SCROLL",
            "TEXTURE_UNIT_ADDRESSING",
            "TEXTURE_UNIT_FILTERING",
            "TEXTURE_UNIT_DELETE",
            "CAMERA_CREATE", /*= (CAMERA << 8) + 1,*/
            "CAMERA_DELETE",
            "CAMERA_FOCALLENGTH",
            "CAMERA_ASPECTRATIO",
            "CAMERA_AUTOASPECTRATIO",
            "CAMERA_FOVY",
            "CAMERA_FRUSTUMOFFSET",
            "CAMERA_NEARCLIP",
            "CAMERA_FARCLIP",
            "CAMERA_PROJECTION",
            "CAMERA_PARENTNODE",
            "CAMERA_PROJECTIONTYPE",
            "CAMERA_ORTHOWINDOWSIZE",
            "CAMERA_WINDOW",
            "CAMERA_VISIBILITY",
            "BROWSER_CREATE", /*= (BROWSER << 8) + 1,*/
            "BROWSER_URL",
            "BROWSER_RESOLUTION",
            "BROWSER_GEOMETRY",
            "BROWSER_OVERLAY",
            "BROWSER_ZOOM",
            "BROWSER_MOUSE_CLICK",
            "BROWSER_MOUSE_SCROLL",
            "BROWSER_MOUSE_MOVED",
            "BROWSER_KEY_VALUE",
            "BROWSER_FOCUS_ON_EDITABLE_FIELD",
            "BROWSER_RELOAD",
            "BROWSER_ELEMENT_CLICK",
            "BROWSER_HOVER_IN",
            "BROWSER_HOVER_OUT",
            "BROWSER_MESSAGE",
            "BROWSER_DELETE",
            "WATER_CREATE", /*= (WATER << 8) + 1,*/
            "WATER_SKY",
            "WATER_CAMERAS",
            "WATER_DELETE",
            "SKY_CREATE", /*= (SKY << 8) + 1,*/
            "SKY_TIME",
            "SKY_SUNLIGHT",
            "SKY_SKYLIGHT",
            "SKY_SIZE",
            "SKY_DELETE",
            "POINT_CLOUD_CREATE", /*= (POINT_CLOUD << 8) + 1,*/
            "POINT_CLOUD_PARAMETERS",
            "POINT_CLOUD_POINTS",
            "POINT_CLOUD_COLORS",
            "POINT_CLOUD_PARENTNODE",
            "POINT_CLOUD_DELETE",
            "ET_INVALID",
            "RIGIDBODY_CREATE", /*= (PHYSICS << 8) + 1,*/
            "RIGIDBODY_DELETE",
            "RIGIDBODY_MASS",
            "RIGIDBODY_FRICTION",
            "RIGIDBODY_DAMPING",
            "RIGIDBODY_BOUYANCY",
            "RIGIDBODY_RESTITUTION",
            "RIGIDBODY_PARENTNODE",
            "RIGIDBODY_SHAPE"
    };

    for (int i = 0; i < eventTypes.size(); ++i) {
        mEventNumberMap.insert(std::make_pair(eventTypes[i],i));
        mEventTypeNameMap.insert(std::make_pair(eventTypes[i],eventTypeNames[i]));
    }
}

//std::map<std::string, ape::GeometryWeakPtr> *ape::JNIPlugin::getNodeGeometryMap()
//{
//    return &mNodeGeometryMap;
//}


// ---- jni ----
extern "C"
JNIEXPORT jstring JNICALL
Java_org_apertusvr_ApertusJNI_stringFromJNIPlugin(JNIEnv *env, jobject thiz, jstring jhello)
{
    const char* hello = env->GetStringUTFChars(jhello,NULL);
    std::stringstream ss;
    ss << hello << "\n(this message went through the JNI plugin)";
    env->ReleaseStringUTFChars(jhello,hello);
    return env->NewStringUTF(ss.str().c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_org_apertusvr_ApertusJNI_processEventDoubleQueue(JNIEnv *env, jclass clazz)
{
    ape::JNIPlugin* jniPlugin = ape::JNIPlugin::getPluginPtr();
    jclass jEventManagerClass = env->FindClass("org/apertusvr/apeEventManager");

    jmethodID jFireEventID = env->GetStaticMethodID(jEventManagerClass,"fireEvent","(IILjava/lang/String;)V");
    ape::DoubleQueue<ape::Event>* eventDoubleQueue = jniPlugin->getEventDoubleQueue();
    eventDoubleQueue->swap();

    while(!eventDoubleQueue->emptyPop())
    {
        ape::Event frontEvent = eventDoubleQueue->front();
        const char* sname = frontEvent.subjectName.c_str();
        jstring jSubjectName =  env->NewStringUTF(sname);
        jint typeAsJint = jniPlugin->getEventNumberMap().at(frontEvent.type);

        env->CallStaticVoidMethod(
                jEventManagerClass,
                jFireEventID,
                typeAsJint,
                (jint)frontEvent.group,
                jSubjectName
                );
        // env->ReleaseStringUTFChars(jSubjectName,sname); //?
        eventDoubleQueue->pop();
    }
}