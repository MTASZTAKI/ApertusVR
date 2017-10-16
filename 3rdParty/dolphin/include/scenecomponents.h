/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#ifndef SCENECOMPONENT_H
#define SCENECOMPONENT_H

#include <OgreEntity.h>
#include "define.h"
#include "ocean/ocean.h"
#include "sky/sky.h"

namespace Dolphin
{
#ifdef RENDEROGRE110
	typedef Ogre::Entity Model;
	typedef std::string Material;
	typedef Ogre::SceneNode Node;
	typedef Ogre::Vector3 Position;
#else 
	typedef Ogre::Entity Model;
#endif
	class SceneComponent
	{
	public:
		SceneComponent();
		virtual ~SceneComponent();
		void SetPosition(Position _position);
		void SetNode(Node* _node);
		virtual Model* GetModel(){return 0;	};
		virtual Node* GetNode(){ return 0; };
		Position GetPosition();
		void SetPhysicsStatus(bool b);
		bool GetPhysicsStatus();
	protected:
		bool PhysicsStatus;
		Node* node;
		Position* position;
		
	};

	class OceanSceneComponent : public SceneComponent
	{
	public:
		void SetHydrax(Hydrax::Hydrax* hydrax);
		Hydrax::Hydrax* GetHydrax();
	private:
		Hydrax::Hydrax* mHydrax;
	};

	class SkySceneComponent : public SceneComponent
	{
	public:
		void SetSkyX(SkyX::SkyX* skyx);
		SkyX::SkyX* GetSkyX();

		void SetSkyXBasicController(SkyX::BasicController* skyxbasiccontorller);
		SkyX::BasicController* GetSkyXBasicController();

	private:
		SkyX::SkyX* mSkyX;
		SkyX::BasicController* mSkyBasicController;
	};

	class LightSceneComponent : public SceneComponent
	{
	};

	class RenderableSceneComponent : public SceneComponent
	{
	public:
		void SetModel(Model* model);
		void SetMaterial(Material* material);
		Model* GetModel();
		Node* GetNode();
		Material* GetMaterial();
	protected:
		Model* model;
		Material* material;
	};


	class SceneComponentGameLevelPlatform : public RenderableSceneComponent
	{

	};
}
#endif //SCENECOMPONENT_H
