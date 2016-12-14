/*
-----------------------------------------------------------------------------
This source file is part of ogre-procedural

For the latest info, see http://www.ogreprocedural.org

Copyright (c) 2010-2013 Michael Broutin

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
#include "ProceduralStableHeaders.h"
#include "Material.h"
#include "Procedural.h"
#include "ProceduralUtils.h"
#include "RTShaderSystem/OgreRTShaderSystem.h"

//-------------------------------------------------------------------------------------
void Sample_Material::createScene(void)
{
	// -- Ground material
	int brickLines = 4;
	int pxPerBrick = 32;

	Procedural::TextureBuffer bricks(brickLines * pxPerBrick);

	// Basic structure
	Procedural::Cell(&bricks).setRegularity(233).setDensity(brickLines).process();
	Procedural::Colours(&bricks).setBrightness(174).setContrast(198).process();
	Procedural::TextureBuffer distort(brickLines * pxPerBrick);
	Procedural::Solid(&distort).setColour((Ogre::uchar)125, (Ogre::uchar)133, (Ogre::uchar)0, (Ogre::uchar)255).process();
	Procedural::RectangleTexture rectDraw(&distort);
	for (int i = 1; i < brickLines; i++)
	{
		Ogre::ColourValue rc = Ogre::ColourValue((i % 2 == 0) ? Ogre::Math::RangeRandom(0.4f, 0.6f) : Ogre::Math::RangeRandom(0.0f, 0.2f), 0.52f, 1.0f);
		rc.a = 1.0f;
		rectDraw.setRectangle((size_t)0, (size_t)(i * pxPerBrick), (size_t)(brickLines * pxPerBrick), (size_t)(i * pxPerBrick + pxPerBrick)).setColour(rc).process();
	}
	Procedural::Distort(&bricks).setParameterImage(&distort).setPower(50).process();
	Procedural::Cloud(&distort).process();
	Procedural::Normals(&distort).process();
	Procedural::Distort(&bricks).setParameterImage(&distort).setPower(8).process();

	Procedural::TextureBuffer normal(&bricks);

	// Normal map & lightning
	Procedural::TextureBuffer light(&bricks);
	Procedural::Colours(&light).setColourBase(0.325f, 0.0f, 0.0f, 0.0f).setColourPercent(0.78f, 0.443f, 0.333f, 1.0f).process();
	Procedural::Normals(&normal).process();
	//Procedural::TextureLightBaker(&light).setNormalMap(&normal).setColourAmbient(0.164f, 0.0f, 0.0f, 0.0f).setPosition(255.0f, 255.0f, 200.0f).setBumpPower(48).setSpecularPower(8).process();

	// Joint
	Procedural::TextureBuffer joint(&bricks);
	Procedural::Invert(&joint).process();
	Procedural::Threshold(&joint).setThreshold(200).setRatio(255).process();
	Procedural::Colours(&joint).setColourBase(0.215f, 0.207f, 0.137f, 0.0f).setColourPercent(0.294f, 0.266f, 0.345f, 1.0f).setBrightness(110).setContrast(153).process();

	// Additional structure
	Procedural::TextureBuffer colourcloud(&bricks);
	Procedural::Threshold(&colourcloud).process();
	Procedural::TextureBuffer cloud(&bricks);
	Procedural::Cloud(&cloud).process();
	Procedural::Combine(&colourcloud).addImage(&cloud, Procedural::Combine::METHOD_MULTIPLY).process();
	Procedural::Colours(&colourcloud).setColourBase(0.329f, 0.141f, 0.0f, 0.0f).setColourPercent(0.95f, 0.949f, 0.862f, 1.0f).setBrightness(30).process();

	// Finish texture
	Procedural::Combine(&light)
	.addImage(&joint, Procedural::Combine::METHOD_ADD_CLAMP)
	.addImage(&colourcloud, Procedural::Combine::METHOD_ADD_CLAMP)
	.process();

	Ogre::TexturePtr demoTexture = light.createTexture("proceduralTexture");
	Ogre::TexturePtr demoTextureNormal = normal.createTexture("proceduralTextureNormal");

#if (OGRE_VERSION < ((1 << 16) | (9 << 8) | 0))
	Ogre::MaterialPtr demoMaterial = Ogre::MaterialManager::getSingletonPtr()->create("proceduralMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
#else
	Ogre::MaterialPtr demoMaterial = Ogre::MaterialManager::getSingletonPtr()->create("proceduralMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME).staticCast<Ogre::Material>();
#endif
	demoMaterial->getTechnique(0)->getPass(0)->setShininess(50);
	demoMaterial->getTechnique(0)->getPass(0)->setDiffuse(Ogre::ColourValue::White);
	demoMaterial->getTechnique(0)->getPass(0)->setSpecular(Ogre::ColourValue(1.0f,1.0f,0.9f));
	demoMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("proceduralTexture");

	if (Ogre::RTShader::ShaderGenerator::initialize())
	{
		Ogre::RTShader::ShaderGenerator* mShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
		mShaderGenerator->setShaderCachePath(".");
		mShaderGenerator->addSceneManager(mSceneMgr);
		RTShader::RenderState* pMainRenderState = mShaderGenerator->createOrRetrieveRenderState(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME).first;
		pMainRenderState->reset();

		RTShader::SubRenderState* subRenderState = mShaderGenerator->createSubRenderState(RTShader::NormalMapLighting::Type);
		RTShader::NormalMapLighting* normalMapSubRS = static_cast<RTShader::NormalMapLighting*>(subRenderState);
		normalMapSubRS->setNormalMapSpace(RTShader::NormalMapLighting::NMS_TANGENT);
		normalMapSubRS->setNormalMapTextureName("proceduralTextureNormal");

		pMainRenderState->addTemplateSubRenderState(normalMapSubRS);
		mShaderGenerator->createShaderBasedTechnique("proceduralMaterial", Ogre::MaterialManager::DEFAULT_SCHEME_NAME, Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
#if (OGRE_VERSION < ((1 << 16) | (9 << 8) | 0))
		mCamera->getViewport()->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
#endif
	}

	// -- Test plane
	Ogre::MeshPtr mesh = Procedural::PlaneGenerator().setNumSegX(20).setNumSegY(20).setSizeX(150).setSizeY(150).setUTile(5.0).setVTile(5.0).realizeMesh("planeMesh");
	unsigned short src, dest;
	if (!mesh->suggestTangentVectorBuildParams(Ogre::VES_TANGENT, src, dest))
		mesh->buildTangentVectors(Ogre::VES_TANGENT, src, dest);
	Entity* ent2 = mSceneMgr->createEntity("planeMesh");
	SceneNode* sn = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	sn->attachObject(ent2);
	ent2->setCastShadows(false);
	ent2->setMaterial(demoMaterial);
}
//-------------------------------------------------------------------------------------
void Sample_Material::createCamera(void)
{
	BaseApplication::createCamera();
}
//-------------------------------------------------------------------------------------
bool Sample_Material::frameStarted(const FrameEvent& evt)
{
#if (OGRE_VERSION < ((1 << 16) | (9 << 8) | 0))
	movingLight->setPosition(mCamera->getPosition());
#else
	movingLight->getParentSceneNode()->setPosition(mCamera->getPosition());
#endif
	return true;
}
//-------------------------------------------------------------------------------------
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
	int main(int argc, char* argv[])
#endif
	{
		// Create application object
		Sample_Material app;

		try
		{
			app.go();
		}
		catch ( Ogre::Exception& e )
		{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
			std::cerr << "An exception has occured: " <<
			          e.getFullDescription().c_str() << std::endl;
#endif
		}

		return 0;
	}

#ifdef __cplusplus
}
#endif
