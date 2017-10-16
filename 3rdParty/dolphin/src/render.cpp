/* Written by Fedak Laszlo <fedak.laszlo@gmail.com>, May 2016 */

#include "render.h"
#include "renderogre.h"
#include "gui.h"
#include "eventmanager.h"
#include "event.h"

Dolphin::Render::Render(void)
{


}

Dolphin::Render::~Render(void)
{
	delete renderogre;
}

void Dolphin::Render::Initialize()
{
	renderogre = new RenderOgre();
	Dolphin::EventManager<Dolphin::DO_KeyPressEvent, const OIS::KeyEvent >::GetSingleton().SubscribeEvent(KEYPRESS_T, std::bind(&Render::CycleTextureFilteringMode, this, std::placeholders::_1));
	Dolphin::EventManager<Dolphin::DO_KeyPressEvent, const OIS::KeyEvent >::GetSingleton().SubscribeEvent(KEYPRESS_R, std::bind(&Render::CyclePolygonRenderingMode, this, std::placeholders::_1));
	Dolphin::EventManager<Dolphin::DO_KeyPressEvent, const OIS::KeyEvent >::GetSingleton().SubscribeEvent(KEYPRESS_F5, std::bind(&Render::RefreshAllTexture, this, std::placeholders::_1));
	Dolphin::EventManager<Dolphin::DO_KeyPressEvent, const OIS::KeyEvent >::GetSingleton().SubscribeEvent(KEYPRESS_PRINTSCREEN, std::bind(&Render::TakeScreeShot, this, std::placeholders::_1));
	renderogre->Initialize();
}

void Dolphin::Render::CycleTextureFilteringMode(const OIS::KeyEvent arg)
{
	std::string newVal;
	unsigned int texturefilteringoptions;
	unsigned int aniso;
	
	switch (Dolphin::Gui::GetSingleton().GetInfoPanelParam("DetailsPanel", 9).at(0))
	{
	case 'B':
		newVal = "Trilinear";
		texturefilteringoptions = Ogre::TFO_TRILINEAR;
		aniso = 1;
		break;
	case 'T':
		newVal = "Anisotropic";
		texturefilteringoptions = Ogre::TFO_ANISOTROPIC;
		aniso = 8;
		break;
	case 'A':
		newVal = "None";
		texturefilteringoptions = Ogre::TFO_NONE;
		aniso = 1;
		break;
	default:
		newVal = "Bilinear";
		texturefilteringoptions = Ogre::TFO_BILINEAR;
		aniso = 1;
	}

	renderogre->SetDefaultTextureFiltering(texturefilteringoptions);
	renderogre->SetDefaultAnisotropy(aniso);
	Dolphin::Gui::GetSingleton().SetInfoPanelParam("DetailsPanel", 9, newVal);
}

void Dolphin::Render::CyclePolygonRenderingMode(const OIS::KeyEvent arg)
{
	std::string newVal;
	unsigned int poligonmode;

	switch (renderogre->GetPolygonMode())
	{
	case Ogre::PM_SOLID:
		newVal = "Wireframe";
		poligonmode = Ogre::PM_WIREFRAME;
		break;
	case Ogre::PM_WIREFRAME:
		newVal = "Points";
		poligonmode = Ogre::PM_POINTS;
		break;
	default:
		newVal = "Solid";
		poligonmode = Ogre::PM_SOLID;
	}
	renderogre->SetPolygonMode(poligonmode);
	Dolphin::Gui::GetSingleton().SetInfoPanelParam("DetailsPanel", 10, newVal);
}

void Dolphin::Render::RefreshAllTexture(const OIS::KeyEvent arg)
{
	renderogre->RefreshAllTexture();
}

void Dolphin::Render::TakeScreeShot(const OIS::KeyEvent arg)
{
	renderogre->TakeScreeShot();
}

void Dolphin::Render::SetTerrain(Terrain* tr)
{
	renderogre->SetTerrain(tr);
}

void Dolphin::Render::SetPhysics(Physics* ph)
{
	renderogre->SetPhysics(ph);
}