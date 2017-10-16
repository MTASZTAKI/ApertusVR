/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#include "scenecomponents.h"

Dolphin::SceneComponent::SceneComponent()
{
}

Dolphin::SceneComponent::~SceneComponent()
{
}

void Dolphin::SceneComponent::SetPosition(Position _position)
{

}

Dolphin::Position Dolphin::SceneComponent::GetPosition()
{
	return *position;
}

void Dolphin::SceneComponent::SetNode(Node* _node)
{
	node = _node;
}


void Dolphin::RenderableSceneComponent::SetModel(Model* _model)
{
	model = _model;
}

Dolphin::Model* Dolphin::RenderableSceneComponent::GetModel()
{
	return model;
}

Dolphin::Node* Dolphin::RenderableSceneComponent::GetNode()
{
	return node;
}

Hydrax::Hydrax* Dolphin::OceanSceneComponent::GetHydrax()
{
	return mHydrax;
};

void Dolphin::OceanSceneComponent::SetHydrax(Hydrax::Hydrax* hydrax)
{
	mHydrax = hydrax;
};


SkyX::SkyX* Dolphin::SkySceneComponent::GetSkyX()
{
	return mSkyX;
};

void Dolphin::SkySceneComponent::SetSkyX(SkyX::SkyX* skyx)
{
	mSkyX = skyx;
};

SkyX::BasicController* Dolphin::SkySceneComponent::GetSkyXBasicController()
{
	return mSkyBasicController;
};

void Dolphin::SkySceneComponent::SetSkyXBasicController(SkyX::BasicController* skyxbasiccontorller)
{
	mSkyBasicController = skyxbasiccontorller;
};

void Dolphin::SceneComponent::SetPhysicsStatus(bool b)
{
	PhysicsStatus = b;
}

bool Dolphin::SceneComponent::GetPhysicsStatus()
{
	return PhysicsStatus;
}