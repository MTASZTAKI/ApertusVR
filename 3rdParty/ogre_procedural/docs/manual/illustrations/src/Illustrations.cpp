/*
-----------------------------------------------------------------------------
This source file is part of ogre-procedural

For the latest info, see http://www.ogreprocedural.org

Copyright (c) 2011 Michael Broutin

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
#include "Ogre.h"
#include "Illustrations.h"
#include "Procedural.h"
#include <iostream>
#include <fstream>
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <direct.h>
#define chdir(p) _chdir(p)
#else
#include <unistd.h>
#endif
#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
#include <Compositor/OgreCompositorManager2.h>
#endif

using namespace Procedural;

//-------------------------------------------------------------------------------------
bool Illustrations::init()
{

	String resourcesCfg, pluginsCfg;
#ifdef _DEBUG
	pluginsCfg = "plugins_d.cfg";
#else
	pluginsCfg = "plugins.cfg";
#endif
	resourcesCfg = "resources.cfg";

	mRoot = new Ogre::Root(pluginsCfg);

	ConfigFile cf;
	cf.load(resourcesCfg);

	ConfigFile::SectionIterator seci = cf.getSectionIterator();

	String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		ConfigFile::SettingsMultiMap* settings = seci.getNext();
		ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			ResourceGroupManager::getSingleton().addResourceLocation(
			    archName, typeName, secName);
		}
	}

	const RenderSystemList& rsList = mRoot->getAvailableRenderers();
	if (rsList.size() == 0)
	{
		Utils::log("Impossible to execute Illustrations : no renderer available!");
		return false;
	}
	RenderSystem* rs = *rsList.begin();

	ConfigOptionMap optionMap = rs->getConfigOptions();
	rs->setConfigOption("FSAA", optionMap["FSAA"].possibleValues.back());
	rs->setConfigOption("Full Screen", "No");
	rs->setConfigOption("Video Mode", optionMap["Video Mode"].possibleValues.back());

	mRoot->setRenderSystem(rs);
	mRoot->initialise(false);

	//Create dummy invisible window
	Ogre::NameValuePairList windowParams;
	windowParams["hidden"] = "true";
	mWindow=mRoot->createRenderWindow("dummyWindow", 1,1,false, &windowParams);
#if OGRE_VERSION < ((2 << 16) | (0 << 8) | 0)
	mWindow->setAutoUpdated(false);
#endif

	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
#if OGRE_VERSION < ((2 << 16) | (0 << 8) | 0)
	mSceneMgr = mRoot->createSceneManager(ST_GENERIC);
#else
	const size_t numThreads = std::max<size_t>(1, Ogre::PlatformInformation::getNumLogicalCores() / 2);
	Ogre::InstancingTheadedCullingMethod threadedCullingMethod = Ogre::INSTANCING_CULLING_SINGLETHREAD;
	if(numThreads > 1) Ogre::InstancingTheadedCullingMethod threadedCullingMethod = Ogre::INSTANCING_CULLING_THREADED;
	mSceneMgr = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC, numThreads, threadedCullingMethod);
#endif
	mCamera = mSceneMgr->createCamera("SimpleCamera");
	mCamera->setAspectRatio(1.);
	cameraPerspective();
	mCamera->setNearClipDistance(1.);
	mSceneMgr->setAmbientLight(ColourValue(0.5f,0.3f,0.1f));
	Ogre::Light* light = mSceneMgr->createLight();
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDiffuseColour(ColourValue::White);
#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
	Ogre::SceneNode* lightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	lightNode->attachObject(light);
#endif
	light->setDirection(Vector3(-1,-1,-1).normalisedCopy());

	// Create main render to texture
	mRttTexture = Ogre::TextureManager::getSingleton().createManual("RttTex", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
	              Ogre::TEX_TYPE_2D, 256, 256, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET, 0, 0, 4);
	mRenderTexture = mRttTexture->getBuffer()->getRenderTarget();
#if OGRE_VERSION < ((2 << 16) | (0 << 8) | 0)
	Ogre::Viewport* vp = mRenderTexture->addViewport(mCamera);
	vp->setClearEveryFrame(true);
	vp->setBackgroundColour(Ogre::ColourValue::White);
	vp->setOverlaysEnabled(false);
#else
	Ogre::CompositorManager2* pCompositorManager = mRoot->getCompositorManager2();
	const Ogre::IdString workspaceName = "scene workspace";
	pCompositorManager->createBasicWorkspaceDef(workspaceName, Ogre::ColourValue::White);
	pCompositorManager->addWorkspace(mSceneMgr, mWindow, mCamera, workspaceName, true);
#endif
	return true;
}

void Illustrations::next(std::string name, Real size)
{
	// Optimise camera placing
	Real distance = 2*size/Math::Tan(mCamera->getFOVy());
	mCamera->setPosition(distance * mCamera->getPosition().normalisedCopy());

	// Write scene to png image
#if OGRE_VERSION < ((2 << 16) | (0 << 8) | 0)
	mRenderTexture->update();
#else
	mRoot->renderOneFrame();
#endif
	mRenderTexture->writeContentsToFile(name + ".png");

	// Clear the scene
	for (std::vector<SceneNode*>::iterator it = mSceneNodes.begin(); it != mSceneNodes.end(); it++)
	{
		(*it)->detachAllObjects();
		mSceneMgr->destroySceneNode(*it);
	}
	for (std::vector<Entity*>::iterator it = mEntities.begin(); it != mEntities.end(); it++)
	{
		MeshManager::getSingletonPtr()->remove((*it)->getMesh()->getName());
		mSceneMgr->destroyEntity(*it);
	}
	mEntities.clear();
	mSceneNodes.clear();
}

void Illustrations::putMesh(MeshPtr mesh, int materialIndex)
{
	Entity* ent = mSceneMgr->createEntity(mesh->getName());
	SceneNode* sn = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	sn->attachObject(ent);
	if (materialIndex==0)
		ent->setMaterialName("HiddenLine");
	else if (materialIndex==1)
		ent->setMaterialName("RedLine");
	else if (materialIndex==2)
		ent->setMaterialName("Examples/Road");
	mEntities.push_back(ent);
	mSceneNodes.push_back(sn);
}

void Illustrations::exportImage(std::string name, Procedural::TextureBufferPtr buffer, bool reset)
{
	size_t border = 32;
	size_t w = buffer->getWidth() + 2 * border;
	size_t h = buffer->getWidth() + 2 * border;
	Ogre::Image* pImgData = buffer->getImage();
	Ogre::uchar* pixelBuffer = new Ogre::uchar[h * w * 4];
	Ogre::Image* image = new Ogre::Image();
	for (size_t y = 0; y < h; y++)
	{
		for (size_t x = 0; x < w; x++)
		{
			Ogre::ColourValue pixel = Ogre::ColourValue::White;
			if (x >= border && x < (w - border) && y >= border && y < (h - border)) pixel = pImgData->getColourAt(x - border, y - border, 0);
#if OGRE_ENDIAN == OGRE_ENDIAN_LITTLE
			pixelBuffer[y * w * 4 + x * 4 + 3] = (Ogre::uchar)std::min<Ogre::Real>(std::max<Ogre::Real>(pixel.r * 255.0f, 0.0f), 255.0f);
			pixelBuffer[y * w * 4 + x * 4 + 2] = (Ogre::uchar)std::min<Ogre::Real>(std::max<Ogre::Real>(pixel.g * 255.0f, 0.0f), 255.0f);
			pixelBuffer[y * w * 4 + x * 4 + 1] = (Ogre::uchar)std::min<Ogre::Real>(std::max<Ogre::Real>(pixel.b * 255.0f, 0.0f), 255.0f);
			pixelBuffer[y * w * 4 + x * 4 + 0] = (Ogre::uchar)std::min<Ogre::Real>(std::max<Ogre::Real>(pixel.a * 255.0f, 0.0f), 255.0f);
#else
			pixelBuffer[y * w * 4 + x * 4 + 0] = (Ogre::uchar)std::min<Ogre::Real>(std::max<Ogre::Real>(pixel.r * 255.0f, 0.0f), 255.0f);
			pixelBuffer[y * w * 4 + x * 4 + 1] = (Ogre::uchar)std::min<Ogre::Real>(std::max<Ogre::Real>(pixel.g * 255.0f, 0.0f), 255.0f);
			pixelBuffer[y * w * 4 + x * 4 + 2] = (Ogre::uchar)std::min<Ogre::Real>(std::max<Ogre::Real>(pixel.b * 255.0f, 0.0f), 255.0f);
			pixelBuffer[y * w * 4 + x * 4 + 3] = (Ogre::uchar)std::min<Ogre::Real>(std::max<Ogre::Real>(pixel.a * 255.0f, 0.0f), 255.0f);
#endif
		}
	}
	image->loadDynamicImage(pixelBuffer, w, h, 1, PF_R8G8B8A8);
	image->save(name + ".png");
	delete image;
	delete pixelBuffer;
	delete pImgData;
	if (reset) Procedural::Solid(buffer).setColour(Ogre::ColourValue::Black).process();
}

void Illustrations::go()
{
	chdir(mOutputPath.c_str());

	//
	// Primitives
	//

	MeshPtr mp;
	mp = BoxGenerator().realizeMesh();
	putMesh(mp);
	next("primitive_box", 1.1f);

	mp = RoundedBoxGenerator().realizeMesh();
	putMesh(mp);
	next("primitive_roundedbox", 1.3f);

	mp = SphereGenerator().realizeMesh();
	putMesh(mp);
	next("primitive_sphere", 1.4f);

	mp = IcoSphereGenerator().realizeMesh();
	putMesh(mp);
	next("primitive_icosphere", 1.4f);

	mp = TorusGenerator().realizeMesh();
	putMesh(mp);
	next("primitive_torus", 1.6f);

	mp = TorusKnotGenerator().realizeMesh();
	putMesh(mp);
	next("primitive_torusknot", 1.6f);

	mp = CylinderGenerator().realizeMesh();
	putMesh(mp);
	next("primitive_cylinder", 1.8f);

	mp = ConeGenerator().realizeMesh();
	putMesh(mp);
	next("primitive_cone", 1.4f);

	mp = TubeGenerator().realizeMesh();
	putMesh(mp);
	next("primitive_tube", 3);

	mp = CapsuleGenerator().realizeMesh();
	putMesh(mp);
	next("primitive_capsule", 2);

	mp = PlaneGenerator().realizeMesh();
	putMesh(mp);
	next("primitive_plane", 1);

	mp = PrismGenerator(1.0f, 1.0f, 3).realizeMesh();
	putMesh(mp);
	next("primitive_prism3", 2);

	mp = PrismGenerator(1.0f, 1.0f, 5).realizeMesh();
	putMesh(mp);
	next("primitive_prism5", 2);

	mp = PrismGenerator(1.0f, 1.0f, 6).realizeMesh();
	putMesh(mp);
	next("primitive_prism6", 2);

	mCamera->setPosition(mCamera->getPosition() + Vector3(0.0f, 1.5f, 0.0f));
	mp = SpringGenerator().setNumRound(3).realizeMesh();
	putMesh(mp);
	next("primitive_spring", 3);

	//
	// Operations on shapes and splines
	//

	cameraBack();

	Shape s = CatmullRomSpline2().addPoint(0,0).addPoint(1,0).addPoint(1,1).addPoint(2,1).addPoint(2,0).addPoint(3,0).addPoint(3,1).addPoint(4,1).realizeShape().translate(-2, 0);
	mp = s.realizeMesh();
	putMesh(mp,1);
	next("spline_catmull", 3);

	s = CubicHermiteSpline2().addPoint(Vector2(0,0), AT_CATMULL).addPoint(Vector2(1,0), AT_CATMULL).addPoint(Vector2(1,1), Vector2(0,2), Vector2(0,-2)).addPoint(Vector2(2,1), AT_CATMULL).addPoint(2,0).addPoint(3,0).addPoint(3,1).addPoint(4,1).setNumSeg(16).realizeShape().translate(-2,0);
	mp = s.realizeMesh();
	putMesh(mp,1);
	next("spline_cubichermite", 3);

	s = KochanekBartelsSpline2().addPoint(Vector2(0,0)).addPoint(Vector2(1,0),1,0,0).addPoint(Vector2(1,1),-1,0,0).addPoint(Vector2(2,1),0,1,0).addPoint(Vector2(2,0),0,-1,0).addPoint(Vector2(3,0),0,0,1).addPoint(Vector2(3,1),0,0,-1).addPoint(Vector2(4,1)).addPoint(Vector2(4,0)).realizeShape().translate(-2,0);
	mp = s.realizeMesh();
	putMesh(mp,1);
	next("spline_kochanekbartels", 3);

	s = RoundedCornerSpline2().addPoint(Vector2(0,0)).addPoint(Vector2(1,0)).addPoint(Vector2(1,1)).addPoint(Vector2(2,1)).addPoint(Vector2(2,0)).addPoint(Vector2(3,0)).addPoint(Vector2(3,1)).addPoint(Vector2(4,1)).addPoint(Vector2(4,0)).setRadius(0.3f).realizeShape().translate(-2,0);
	mp = s.realizeMesh();
	putMesh(mp,1);
	next("spline_roundedcorner", 3);

	s = BezierCurve2().addPoint(Vector2(0,0)).addPoint(Vector2(1,0)).addPoint(Vector2(1,1)).addPoint(Vector2(2,1)).addPoint(Vector2(2,0)).addPoint(Vector2(3,0)).addPoint(Vector2(3,1)).addPoint(Vector2(4,1)).addPoint(Vector2(4,0)).realizeShape().translate(-2,0);
	mp = s.realizeMesh();
	putMesh(mp,1);
	next("spline_beziercurve", 3);

	cameraPerspective();
	Path p = HelixPath().setNumSegPath(64).setNumRound(3).setHeight(1.5f).realizePath().translate(0.0f, -2.2f, 0.0f);
	mp = p.realizeMesh();
	putMesh(mp,1);
	next("spline_helix", 3);

	cameraBack();

	s = RectangleShape().setHeight(2).setWidth(4).realizeShape();
	mp = s.realizeMesh();
	putMesh(mp,1);
	next("shape_rectangle", 3);

	s = CircleShape().setNumSeg(64).setRadius(2).realizeShape();
	mp = s.realizeMesh();
	putMesh(mp,1);
	next("shape_circle", 3);

	s = EllipseShape().setNumSeg(64).setRadiusX(2.25f).setRadiusY(1.25f).realizeShape();
	mp = s.realizeMesh();
	putMesh(mp,1);
	next("shape_ellipse", 3);

	s = TriangleShape().setLength(3).realizeShape();
	mp = s.realizeMesh();
	putMesh(mp,1);
	next("shape_triangle", 3);

	//
	// Boolean operations
	//
	Shape s1 = RectangleShape().realizeShape();
	Shape s2 = s1;
	s2.translate(.5f,.5f);

	putMesh(s1.realizeMesh(), 1);
	putMesh(s2.realizeMesh(), 1);
	next("shape_booleansetup", 1.5);

	s = s1.booleanUnion(s2).getShape(0);
	mp = s.realizeMesh();
	putMesh(mp,1);
	next("shape_booleanunion", 1.5f);

	s = s1.booleanIntersect(s2).getShape(0);
	mp = s.realizeMesh();
	putMesh(mp,1);
	next("shape_booleanintersection", 1.5f);

	s = s1.booleanDifference(s2).getShape(0);
	mp = s.realizeMesh();
	putMesh(mp,1);
	next("shape_booleandifference", 1.5f);

	//
	// Geometric operations
	//
	s = Shape().addPoint(0.0f, 0.0f).addPoint(-0.5f, -1.0f).addPoint(-0.75f, 1.0f).addPoint(0.0f, 0.5f);
	mp = s.realizeMesh();
	putMesh(mp,1);
	next("shape_geometricsetup", 2.5);

	s.translate(0.5f, 1.0f);
	mp = s.realizeMesh();
	putMesh(mp,1);
	next("shape_geometrictranslate", 2.5);
	s.translate(-0.5f, -1.0f);

	s.scale(2.0f, 2.0f);
	mp = s.realizeMesh();
	putMesh(mp,1);
	next("shape_geometricscale", 2.5);
	s.scale(0.5f, 0.5f);

	s.rotate(Ogre::Degree(45));
	mp = s.realizeMesh();
	putMesh(mp,1);
	next("shape_geometricrotate", 2.5);
	s.rotate(Ogre::Degree(-45));

	s1 = s;
	s1.mirror(0.5f, 0.5f);
	mp = s1.realizeMesh();
	putMesh(mp,1);
	next("shape_geometricmirror_point", 2.5);

	s2 = s;
	s2.mirrorAroundAxis(Vector2::UNIT_Y);
	s2.close();
	mp = s2.realizeMesh();
	putMesh(mp,1);
	next("shape_geometricmirror_yaxis", 2.5);

	//
	// Thicken
	//
	s = Shape().addPoint(-1,-1).addPoint(0.5,0).addPoint(-0.5,0).addPoint(1,1);
	mp = s.realizeMesh();
	putMesh(mp,1);
	next("shape_thick1", 1.5f);

	s = s.thicken(.2f).getShape(0);
	mp = s.realizeMesh();
	putMesh(mp,1);
	next("shape_thick2", 1.5f);

	//
	// Delaunay
	//
	cameraFront();

	MultiShape ms;
	CircleShape cs;
	ms.addShape(cs.setRadius(2).realizeShape());
	ms.addShape(cs.setRadius(.3f).realizeShape().translate(-1,.3f).switchSide());
	ms.addShape(cs.realizeShape().translate(1,.3f).switchSide());
	ms.addShape(cs.realizeShape().switchSide());
	ms.addShape(cs.realizeShape().scale(2,1).translate(0,-1).switchSide());
	mp = Triangulator().setMultiShapeToTriangulate(&ms).realizeMesh();
	putMesh(mp);
	next("shape_triangulation", 3);

	//
	// Extrusion
	//
	cameraPerspective();

	s = Shape().addPoint(-1,-1).addPoint(1,-1).addPoint(1,1).addPoint(0,0).addPoint(-1,1).close();
	p = RoundedCornerSpline3().addPoint(-6,2.5,-2.5).addPoint(-5,0,-2.5).addPoint(0,0,2.5).addPoint(5,0,-2.5).setRadius(1.).realizePath();
	mp = Extruder().setShapeToExtrude(&s).setExtrusionPath(&p).realizeMesh();
	putMesh(mp);
	next("extruder_generic", 10);

	s2 = RectangleShape().setHeight(.5).realizeShape();
	Track t = Track(Track::AM_RELATIVE_LINEIC).addKeyFrame(0,0).addKeyFrame(1.0,-1.0);
	Path p2 = LinePath().betweenPoints(Vector3(-5,0,0),Vector3(5,0,0)).setNumSeg(10).realizePath();
	mp = Extruder().setShapeToExtrude(&s2).setExtrusionPath(&p2).setRotationTrack(&t).realizeMesh();
	putMesh(mp);
	next("extruder_rotationtrack", 7);

	t = Track(Track::AM_RELATIVE_LINEIC).addKeyFrame(0,.5f).addKeyFrame(.4f,.5f).addKeyFrame(.5f,1.2f).addKeyFrame(.8f,1).addKeyFrame(1.0f,1);
	mp = Extruder().setShapeToExtrude(&s2).setExtrusionPath(&p2).setScaleTrack(&t).realizeMesh();
	putMesh(mp);
	next("extruder_scaletrack", 7);

	Procedural::Shape s4 = Procedural::Shape().addPoint(-1.2f,.2f).addPoint(-1.f,.2f).addPoint(-.9f,.1f).addPoint(.9f,.1f).addPoint(1.f,.2f).addPoint(1.2f,.2f).scale(2).setOutSide(Procedural::SIDE_LEFT);
	Procedural::Track textureTrack = Procedural::Track(Procedural::Track::AM_POINT).addKeyFrame(0,0).addKeyFrame(2,.2f).addKeyFrame(3,.8f).addKeyFrame(5,1);
	mp = Extruder().setShapeTextureTrack(&textureTrack).setShapeToExtrude(&s4).setExtrusionPath(&p2).setCapped(false).realizeMesh();
	putMesh(mp, 2);
	next("extruder_texturetrack", 7);

	cameraFront();

	Shape s3 = CircleShape().setNumSeg(16).realizeShape();
	ms = MultiShape(2, &s3.switchSide(), &Shape(s3).scale(1.1f));
	Path p3 = CatmullRomSpline3().addPoint(0,0,-5).addPoint(0,0,0).addPoint(1,-1,5).realizePath();
	mp = Extruder().setMultiShapeToExtrude(&ms).setExtrusionPath(&p3).realizeMesh();
	putMesh(mp);
	next("extruder_multishape", 4);

	//
	// Lathe
	//
	s = Shape().addPoint(0,-3).addPoint(1,-3).addPoint(1,0).addPoint(.8f,1).addPoint(.8f,2).addPoint(1.5f,3).addPoint(0,4);
	mp = Lathe().setShapeToExtrude(&s).realizeMesh();
	putMesh(mp);
	next("lathe_generic",5);

	mp = Lathe().setShapeToExtrude(&s).setAngleBegin((Radian)2).setAngleEnd((Radian)0).realizeMesh();
	putMesh(mp);
	next("lathe_anglerange",5);

	//
	// MultiShapes
	//
#ifdef PROCEDURAL_USE_FREETYPE
	mCamera->setPosition(1.8f,0.8f,5);
	mCamera->lookAt(1.8f,0.8f,0);

	// Extract and save font file from Ogre resources
	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource("cuckoo.ttf", "Essential"); // Font from SdkTrays.zip
	std::ofstream fontFile("cuckoo.ttf", std::ios::out | std::ios::binary);
	char block[1024];
	while (!stream->eof())
	{
		size_t len = stream->read(block, 1024);
		fontFile.write(block, len);
		if (len < 1024) break;
	}
	fontFile.close();

	// Use font file to write a text on a texture
	mp = TextShape().setFont("cuckoo.ttf", 12).setText("Ogre").realizeShapes().realizeMesh();
	putMesh(mp);
	next("shape_text", 20);
#endif

	//
	// Texture
	//
	cameraFront();
	int bufSize = 128;

	// Render
	Procedural::TextureBuffer bufferSolid(bufSize);
	Procedural::Solid(&bufferSolid).setColour(Ogre::ColourValue(0.0f, 0.5f, 1.0f, 1.0f)).process();
	exportImage("texture_solid", &bufferSolid);

	Procedural::TextureBuffer bufferGradient(bufSize);
	Procedural::Gradient(&bufferGradient).setColours(Ogre::ColourValue::Black, Ogre::ColourValue::Red, Ogre::ColourValue::Green, Ogre::ColourValue::Blue).process();
	exportImage("texture_gradient", &bufferGradient);

	Procedural::TextureBuffer bufferCellNormal(bufSize);
	Procedural::Cell(&bufferCellNormal).setDensity(4).process();
	exportImage("texture_cell_default", &bufferCellNormal);
	Procedural::TextureBuffer bufferCellChessCone(bufSize);
	Procedural::Cell(&bufferCellChessCone).setDensity(4).setMode(Procedural::Cell::MODE_CHESSBOARD).setPattern(Procedural::Cell::PATTERN_CONE).process();
	exportImage("texture_cell_chess", &bufferCellChessCone);
	Procedural::TextureBuffer bufferCellGridCross(bufSize);
	Procedural::Cell(&bufferCellGridCross).setDensity(4).setMode(Procedural::Cell::MODE_GRID).setPattern(Procedural::Cell::PATTERN_CROSS).process();
	exportImage("texture_cell_grid", &bufferCellGridCross);

	Procedural::TextureBuffer bufferNoiseWhite(bufSize);
	Procedural::Noise(&bufferNoiseWhite).setType(Procedural::Noise::NOISE_WHITE).process();
	exportImage("texture_noise_white", &bufferNoiseWhite);
	Procedural::TextureBuffer bufferNoisePerlin(bufSize);
	Procedural::Noise(&bufferNoisePerlin).setType(Procedural::Noise::NOISE_PERLIN).process();
	exportImage("texture_noise_perlin", &bufferNoisePerlin);

	Procedural::TextureBuffer bufferMarble(bufSize);
	Procedural::Marble(&bufferMarble).process();
	exportImage("texture_marble", &bufferMarble);

	Procedural::TextureBuffer bufferWood(bufSize);
	Procedural::Wood(&bufferWood).setRings(5).process();
	exportImage("texture_wood", &bufferWood);

	Procedural::TextureBuffer bufferCloud(bufSize);
	Procedural::Cloud(&bufferCloud).process();
	exportImage("texture_cloud", &bufferCloud);

	Procedural::TextureBuffer bufferLabyrinth(bufSize);
	Procedural::Labyrinth(&bufferLabyrinth).process();
	exportImage("texture_labyrinth", &bufferLabyrinth);

	Procedural::TextureBuffer bufferTextile(bufSize);
	Procedural::Textile(&bufferTextile).process();
	exportImage("texture_textile", &bufferTextile);

	Procedural::TextureBuffer bufferImage(bufSize);
	Procedural::Image(&bufferImage).setFile("red_brick.jpg").process();
	exportImage("texture_image", &bufferImage);

	// Manipulation
	Procedural::TextureBuffer buffer(bufSize);
	Procedural::Gradient(&buffer).setColours(Ogre::ColourValue::Black, Ogre::ColourValue::Red, Ogre::ColourValue::Green, Ogre::ColourValue::Blue).process();
	Procedural::Cell(&bufferCellNormal).setDensity(4).setRegularity(234).process();
	exportImage("texture_cell_smooth", &bufferCellNormal);
	Procedural::Abnormals(&buffer).setParameterImage(&bufferCellNormal).process();
	exportImage("texture_abnormals", &buffer, true);
	dotFile dotfile(mOutputPath, "texture_02", "Abnormals_Demo");
	dotfile.set("Gradient", "texture_gradient", "Cell", "texture_cell_smooth", "Abnormals", "texture_abnormals");
	dotfile.save();

	Procedural::Cell(&buffer).setDensity(4).setRegularity(234).process();
	Procedural::Alpha(&buffer).process();
	exportImage("texture_alpha", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_03", "Alpha_Demo");
	dotfile.set("Cell", "texture_cell_smooth", "Alpha", "texture_alpha");
	dotfile.save();

	Procedural::Gradient(&buffer).setColours(Ogre::ColourValue::Black, Ogre::ColourValue::Red, Ogre::ColourValue::Green, Ogre::ColourValue::Blue).process();
	Procedural::AlphaMask(&buffer).setParameterImage(&bufferCellNormal).process();
	exportImage("texture_alphamask", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_04", "AlphaMask_Demo");
	dotfile.set("Cell", "texture_cell_smooth", "Gradient", "texture_gradient", "AlphaMask", "texture_alphamask");
	dotfile.save();

	Procedural::Image(&buffer).setFile("red_brick.jpg").process();
	Procedural::Blur(&buffer).setType(Procedural::Blur::BLUR_MEAN).process();
	exportImage("texture_blur_1", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_05a", "Blur_Mean_Demo");
	dotfile.set("Image", "texture_image", "Blur", "texture_blur_1");
	dotfile.save();
	Procedural::Image(&buffer).setFile("red_brick.jpg").process();
	Procedural::Blur(&buffer).setType(Procedural::Blur::BLUR_GAUSSIAN).process();
	exportImage("texture_blur_2", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_05b", "Blur_Gaussian_Demo");
	dotfile.set("Image", "texture_image", "Blur", "texture_blur_2");
	dotfile.save();

	Procedural::Image(&buffer).setFile("red_brick.jpg").process();
	Procedural::Channel(&buffer).setSelection(Procedural::Channel::SELECT_BLUE).process();
	exportImage("texture_channel_1", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_06a", "Channel_blue_Demo");
	dotfile.set("Image", "texture_image", "Channel", "texture_channel_1");
	dotfile.save();
	Procedural::Image(&buffer).setFile("red_brick.jpg").process();
	Procedural::Channel(&buffer).setSelection(Procedural::Channel::SELECT_GRAY).process();
	exportImage("texture_channel_2", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_06b", "Channel_gray_Demo");
	dotfile.set("Image", "texture_image", "Channel", "texture_channel_2");
	dotfile.save();

	Procedural::Gradient(&buffer).setColours(Ogre::ColourValue::Black, Ogre::ColourValue::Red, Ogre::ColourValue::Green, Ogre::ColourValue::Blue).process();
	Procedural::Colours(&buffer).setColourBase(Ogre::ColourValue::Red).setColourPercent(Ogre::ColourValue::Blue).process();
	exportImage("texture_colours", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_07", "Colours_Demo");
	dotfile.set("Gradient", "texture_gradient", "Colours", "texture_colours");
	dotfile.save();

	Procedural::Cloud(&buffer).process();
	Procedural::Combine(&buffer).addImage(&bufferGradient, Procedural::Combine::METHOD_ADD_CLAMP).process();
	exportImage("texture_combine", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_08", "Combine_Demo");
	dotfile.set("Cloud", "texture_cloud", "Gradient", "texture_gradient", "Combine", "texture_combine");
	dotfile.save();

	Procedural::Image(&buffer).setFile("red_brick.jpg").process();
	Procedural::Convolution(&buffer).setKernel(Ogre::Matrix3(10.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -10.0f)).process();
	exportImage("texture_convolution", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_09", "Convolution_Demo");
	dotfile.set("Image", "texture_image", "Kernel matrix", "", "Convolution", "texture_convolution");
	dotfile.save();

	Procedural::Cloud(&buffer).process();
	Procedural::Crack(&buffer).setParameterImage(&bufferGradient).process();
	exportImage("texture_crack", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_10", "Crack_Demo");
	dotfile.set("Cloud", "texture_cloud", "Gradient", "texture_gradient", "Crack", "texture_crack");
	dotfile.save();

	Procedural::Cloud(&buffer).process();
	Procedural::Dilate(&buffer).process();
	exportImage("texture_dilate", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_11", "Dilate_Demo");
	dotfile.set("Cloud", "texture_cloud", "Dilate", "texture_dilate");
	dotfile.save();

	Procedural::Gradient(&buffer).setColours(Ogre::ColourValue::Black, Ogre::ColourValue::Red, Ogre::ColourValue::Green, Ogre::ColourValue::Blue).process();
	Procedural::Distort(&buffer).setParameterImage(&bufferCellNormal).setPower(255).process();
	exportImage("texture_distort", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_12", "Distort_Demo");
	dotfile.set("Gradient", "texture_gradient", "Cell", "texture_cell_smooth", "Distort", "texture_distort");
	dotfile.save();

	Procedural::Cell(&buffer).setDensity(4).setRegularity(234).process();
	Procedural::EdgeDetection(&buffer).setType(Procedural::EdgeDetection::DETECTION_SOBEL).process();
	exportImage("texture_edgedetection", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_13", "EdgeDetection_Demo");
	dotfile.set("Cell", "texture_cell_smooth", "EdgeDetection", "texture_edgedetection");
	dotfile.save();

	Procedural::Image(&buffer).setFile("red_brick.jpg").process();
	Procedural::Flip(&buffer).setAxis(Procedural::Flip::FLIP_POINT).process();
	exportImage("texture_flip_1", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_14a", "Flip_point_Demo");
	dotfile.set("Image", "texture_image", "Flip", "texture_flip_1");
	dotfile.save();
	Procedural::Image(&buffer).setFile("red_brick.jpg").process();
	Procedural::Flip(&buffer).setAxis(Procedural::Flip::FLIP_VERTICAL).process();
	exportImage("texture_flip_2", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_14b", "Flip_vertical_Demo");
	dotfile.set("Image", "texture_image", "Flip", "texture_flip_2");
	dotfile.save();
	Procedural::Image(&buffer).setFile("red_brick.jpg").process();
	Procedural::Flip(&buffer).setAxis(Procedural::Flip::FLIP_HORIZONTAL).process();
	exportImage("texture_flip_3", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_14c", "Flip_horizontal_Demo");
	dotfile.set("Image", "texture_image", "Flip", "texture_flip_3");
	dotfile.save();

	Procedural::Gradient(&buffer).process();
	Procedural::Glow(&buffer).process();
	exportImage("texture_glow", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_15", "Glow_Demo");
	dotfile.set("Gradient", "texture_gradient", "Glow", "texture_glow");
	dotfile.save();

	Procedural::Gradient(&buffer).process();
	Procedural::Invert(&buffer).process();
	exportImage("texture_invert", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_16", "Invert_Demo");
	dotfile.set("Gradient", "texture_gradient", "Invert", "texture_invert");
	dotfile.save();

	Procedural::Image(&buffer).setFile("red_brick.jpg").process();
	Procedural::Jitter(&buffer).process();
	exportImage("texture_jitter", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_17", "Jitter_Demo");
	dotfile.set("Image", "texture_image", "Jitter", "texture_jitter");
	dotfile.save();

	Procedural::Cloud(&buffer).process();
	Procedural::Lerp(&buffer).setImageA(&bufferGradient).setImageB(&bufferCellNormal).process();
	exportImage("texture_lerp", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_18", "Lerp_Demo");
	dotfile.set("Cloud", "texture_cloud", "Gradient", "texture_gradient", "Lerp", "texture_lerp");
	dotfile.add("Cell", "texture_cell_smooth");
	dotfile.bind(4, 3);
	dotfile.save();

	Procedural::Cell(&buffer).setDensity(4).setRegularity(234).process();
	Procedural::TextureLightBaker(&buffer).setColourAmbient(0.5f, 0.2f, 0, 0).setColourDiffuse(0.2f, 0.1f, 0, 0).setBumpPower(255).process();
	exportImage("texture_light", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_19a", "Light_1_Demo");
	dotfile.set("Cell", "texture_cell_smooth", "Normals", "texture_normals", "Light", "texture_light", dotFile::ROW);
	dotfile.bind(1, 3);
	dotfile.save();
	dotfile = dotFile(mOutputPath, "texture_19b", "Light_2_Demo");
	dotfile.set("Cell", "texture_cell_smooth", "Light", "texture_light");
	dotfile.save();

	Procedural::Gradient(&buffer).process();
	Procedural::Lookup(&buffer).setParameterImage(&bufferCellNormal).process();
	exportImage("texture_lookup", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_20", "Lookup_Demo");
	dotfile.set("Gradient", "texture_gradient", "Cell", "texture_cell_smooth", "Lookup", "texture_lookup");
	dotfile.save();

	Procedural::Cell(&buffer).setDensity(4).setRegularity(234).process();
	Procedural::Normals(&buffer).process();
	exportImage("texture_normals", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_21a", "Normals_1_Demo");
	dotfile.set("Cell", "texture_cell_smooth", "Normals", "texture_normals");
	dotfile.save();
	Procedural::Cell(&buffer).setDensity(4).setRegularity(234).process();
	Procedural::Colours(&buffer).setColourBase(Ogre::ColourValue::Red).setColourPercent(Ogre::ColourValue::Blue).process();
	exportImage("texture_normals_tip", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_21b", "Normals_2_Demo");
	dotfile.set("Cell", "texture_cell_smooth", "Normals", "texture_normals", "Colours", "texture_normals_tip", dotFile::SPLIT);
	dotfile.save();

	Procedural::Image(&buffer).setFile("red_brick.jpg").process();
	Procedural::OilPaint(&buffer).setRadius(5).process();
	exportImage("texture_oilpaint", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_22", "OilPaint_Demo");
	dotfile.set("Image", "texture_image", "OilPaint", "texture_oilpaint");
	dotfile.save();

	Procedural::Solid(&buffer).setColour(Ogre::ColourValue(0.0f, 0.5f, 1.0f, 1.0f)).process();
	Procedural::RandomPixels(&buffer).setColour(Ogre::ColourValue::Red).setCount(200).process();
	exportImage("texture_randompixels", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_23", "RandomPixels_Demo");
	dotfile.set("Solid", "texture_solid", "RandomPixels", "texture_randompixels");
	dotfile.save();

	Procedural::Solid(&buffer).setColour(Ogre::ColourValue(0.0f, 0.5f, 1.0f, 1.0f)).process();
	Procedural::RectangleTexture(&buffer).setColour(Ogre::ColourValue::Red).setRectangle(0.25f, 0.25f, 0.75f, 0.75f).process();
	exportImage("texture_rectangle", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_24", "Rectangle_Demo");
	dotfile.set("Solid", "texture_solid", "Rectangle", "texture_rectangle");
	dotfile.save();

	Procedural::Gradient(&buffer).setColours(Ogre::ColourValue::Black, Ogre::ColourValue::Red, Ogre::ColourValue::Green, Ogre::ColourValue::Blue).process();
	Procedural::RotationZoom(&buffer).setRotation(0.125f).process();
	exportImage("texture_rotationzoom", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_25", "RotationZoom_Demo");
	dotfile.set("Gradient", "texture_gradient", "RotationZoom", "texture_rotationzoom");
	dotfile.save();

	Procedural::Cell(&buffer).setDensity(4).setRegularity(234).process();
	Procedural::Segment(&buffer).setColourSource(&bufferGradient).process();
	exportImage("texture_segment", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_26", "Segment_Demo");
	dotfile.set("Cell", "texture_cell_smooth", "Gradient", "texture_gradient", "Segment", "texture_segment");
	dotfile.save();

	Procedural::Image(&buffer).setFile("red_brick.jpg").process();
	Procedural::Sharpen(&buffer).setType(Procedural::Sharpen::SHARP_BASIC).process();
	exportImage("texture_sharpen_1", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_27a", "Sharpen_basic_Demo");
	dotfile.set("Image", "texture_image", "Sharpen", "texture_sharpen_1");
	dotfile.save();
	Procedural::Image(&buffer).setFile("red_brick.jpg").process();
	Procedural::Sharpen(&buffer).setType(Procedural::Sharpen::SHARP_GAUSSIAN).process();
	exportImage("texture_sharpen_2", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_27b", "Sharpen_gaussian_Demo");
	dotfile.set("Image", "texture_image", "Sharpen", "texture_sharpen_2");
	dotfile.save();

	Procedural::Cell(&buffer).setDensity(4).setRegularity(234).process();
	Procedural::Threshold(&buffer).process();
	exportImage("texture_threshold", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_28", "Threshold_Demo");
	dotfile.set("Cell", "texture_cell_smooth", "Threshold", "texture_threshold");
	dotfile.save();

	Procedural::Cell(&buffer).setDensity(4).setRegularity(234).process();
	Procedural::Vortex(&buffer).process();
	exportImage("texture_vortex", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_29", "Vortex_Demo");
	dotfile.set("Cell", "texture_cell_smooth", "Vortex", "texture_vortex");
	dotfile.save();

	Procedural::Solid(&buffer).setColour(Ogre::ColourValue(0.0f, 0.5f, 1.0f, 1.0f)).process();
	Procedural::Cycloid(&buffer).setColour(Ogre::ColourValue::Red).setPenSize(2).setType(Procedural::Cycloid::HYPOCYCLOID).process();
	exportImage("texture_cycloid_hypocycloid", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_30", "Cycloid_Demo");
	dotfile.set("Solid", "texture_solid", "Hypocycloid", "texture_cycloid_hypocycloid");
	dotfile.save();
	Procedural::Solid(&buffer).setColour(Ogre::ColourValue(0.0f, 0.5f, 1.0f, 1.0f)).process();
	Procedural::Cycloid(&buffer).setColour(Ogre::ColourValue::Red).setPenSize(2).setType(Procedural::Cycloid::HYPOTROCHOID).process();
	exportImage("texture_cycloid_hypotrochoid", &buffer, true);
	Procedural::Solid(&buffer).setColour(Ogre::ColourValue(0.0f, 0.5f, 1.0f, 1.0f)).process();
	Procedural::Cycloid(&buffer).setColour(Ogre::ColourValue::Red).setPenSize(2).setType(Procedural::Cycloid::EPICYCLOID).process();
	exportImage("texture_cycloid_epicycloid", &buffer, true);
	Procedural::Solid(&buffer).setColour(Ogre::ColourValue(0.0f, 0.5f, 1.0f, 1.0f)).process();
	Procedural::Cycloid(&buffer).setColour(Ogre::ColourValue::Red).setPenSize(2).setType(Procedural::Cycloid::EPITROCHOID).process();
	exportImage("texture_cycloid_epitrochoid", &buffer, true);
	Procedural::Solid(&buffer).setColour(Ogre::ColourValue(0.0f, 0.5f, 1.0f, 1.0f)).process();
	Procedural::Cycloid(&buffer).setColour(Ogre::ColourValue::Red).setPenSize(2).setType(Procedural::Cycloid::ROSE_CURVE).process();
	exportImage("texture_cycloid_rose", &buffer, true);
	Procedural::Solid(&buffer).setColour(Ogre::ColourValue(0.0f, 0.5f, 1.0f, 1.0f)).process();
	Procedural::Cycloid(&buffer).setColour(Ogre::ColourValue::Red).setPenSize(2).setType(Procedural::Cycloid::LISSAJOUS_CURVE).process();
	exportImage("texture_cycloid_lissajous", &buffer, true);


	Procedural::Image(&buffer).setFile("red_brick.jpg").process();
	Procedural::Blit(&buffer).setInputBuffer(&bufferGradient).setInputRect(0.0f, 0.0f, 0.5f, 0.5f).setOutputRect(0.25f, 0.25f, 0.75f, 0.75f).process();
	exportImage("texture_blit", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_31", "Blit_Demo");
	dotfile.set("Image", "texture_image", "Gradient", "texture_gradient", "Blit", "texture_blit");
	dotfile.save();

	Procedural::Solid(&buffer).setColour(Ogre::ColourValue(0.0f, 0.5f, 1.0f, 1.0f)).process();
	Procedural::CircleTexture(&buffer).setColour(Ogre::ColourValue::Red).setRadius(0.3f).process();
	exportImage("texture_circle", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_32", "Circle_Demo");
	dotfile.set("Solid", "texture_solid", "Circle", "texture_circle");
	dotfile.save();

	Procedural::Solid(&buffer).setColour(Ogre::ColourValue(0.0f, 0.5f, 1.0f, 1.0f)).process();
	Procedural::EllipseTexture(&buffer).setColour(Ogre::ColourValue::Red).setRadiusX(0.4f).setRadiusY(0.2f).process();
	exportImage("texture_ellipse", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_33", "Ellipse_Demo");
	dotfile.set("Solid", "texture_solid", "Ellipse", "texture_ellipse");
	dotfile.save();

#ifdef PROCEDURAL_USE_FREETYPE
	Procedural::Cell(&buffer).setDensity(4).setRegularity(234).process();
	Procedural::TextTexture(&buffer).setFont("cuckoo.ttf", 30).setColour(Ogre::ColourValue::Red).setPosition((size_t)20, (size_t)20).setText("OGRE").process();
	Procedural::TextTexture(&buffer).setFont("cuckoo.ttf", 20).setColour(Ogre::ColourValue::Green).setPosition((size_t)10, (size_t)60).setText("Procedural").process();
	exportImage("texture_text", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_34", "Text_Demo");
	dotfile.set("Cell", "texture_cell_smooth", "Text", "texture_text");
	dotfile.save();
#endif

	Procedural::Solid(&buffer).setColour(Ogre::ColourValue(0.0f, 0.5f, 1.0f, 1.0f)).process();
	Procedural::Polygon(&buffer).setColour(Ogre::ColourValue::Red).setSides(3).process();
	exportImage("texture_polygon3", &buffer, true);
	Procedural::Solid(&buffer).setColour(Ogre::ColourValue(0.0f, 0.5f, 1.0f, 1.0f)).process();
	Procedural::Polygon(&buffer).setColour(Ogre::ColourValue::Red).setSides(5).process();
	exportImage("texture_polygon5", &buffer, true);
	dotfile = dotFile(mOutputPath, "texture_35", "Polygon_5_Demo");
	dotfile.set("Solid", "texture_solid", "Polygon", "texture_polygon5");
	dotfile.save();
	Procedural::Solid(&buffer).setColour(Ogre::ColourValue(0.0f, 0.5f, 1.0f, 1.0f)).process();
	Procedural::Polygon(&buffer).setColour(Ogre::ColourValue::Red).setSides(6).process();
	exportImage("texture_polygon6", &buffer, true);

	// Example
	dotfile = dotFile(mOutputPath, "texture_01", "Material_Example");
	int pxPerBrick = 32;
	int brickLines = bufSize / pxPerBrick;
	Procedural::TextureBuffer bricks(brickLines * pxPerBrick);
	Procedural::Cell(&bricks).setRegularity(233).setDensity(brickLines).process();
	exportImage("texture_example_cell", &bricks);
	int s01 = dotfile.add("Cell", "texture_example_cell");
	Procedural::Colours(&bricks).setBrightness(174).setContrast(198).process();
	exportImage("texture_example_colours_1", &bricks);
	int s02 = dotfile.add("Colours", "texture_example_colours_1");
	dotfile.bind(s01, s02);
	Procedural::TextureBuffer distort(brickLines * pxPerBrick);
	Procedural::Solid(&distort).setColour(.48f, .52f, 0, 1.0f).process();
	exportImage("texture_example_solid", &distort);
	int s03x = dotfile.add("Solid", "texture_example_solid");
	Procedural::RectangleTexture rectDraw(&distort);
	for (size_t i = 1; i < (size_t)brickLines; i++)
	{
		Ogre::ColourValue rc = Ogre::ColourValue((i % 2 == 0) ? Ogre::Math::RangeRandom(0.4f, 0.6f) : Ogre::Math::RangeRandom(0.0f, 0.2f), 0.52f, 1.0f);
		rc.a = 1.0f;
		rectDraw.setRectangle(0, i * pxPerBrick, brickLines * pxPerBrick, i * pxPerBrick + pxPerBrick).setColour(rc).process();
		exportImage("texture_example_rectangle_" + Ogre::StringConverter::toString(i), &distort);
		int s03y = dotfile.add("Rectangle", "texture_example_rectangle_" + Ogre::StringConverter::toString(i));
		dotfile.bind(s03x, s03y);
		s03x = s03y;
	}
	Procedural::Distort(&bricks).setParameterImage(&distort).setPower(50).process();
	exportImage("texture_example_distort_1", &bricks);
	int s07 = dotfile.add("Distort", "texture_example_distort_1");
	dotfile.bind(s02, s07);
	dotfile.bind(s03x, s07);
	Procedural::Cloud(&distort).process();
	exportImage("texture_example_cloud_1", &distort);
	int s08 = dotfile.add("Cloud", "texture_example_cloud_1");
	Procedural::Normals(&distort).process();
	exportImage("texture_example_normals_1", &distort);
	int s09 = dotfile.add("Normals", "texture_example_normals_1");
	dotfile.bind(s08, s09);
	Procedural::Distort(&bricks).setParameterImage(&distort).setPower(8).process();
	exportImage("texture_example_distort_2", &bricks);
	int s10 = dotfile.add("Distort", "texture_example_distort_2");
	dotfile.bind(s07, s10);
	dotfile.bind(s09, s10);
	Procedural::TextureBuffer normal(&bricks);
	Procedural::TextureBuffer light(&bricks);
	Procedural::Colours(&light).setColourBase(0.325f, 0.0f, 0.0f, 0.0f).setColourPercent(0.78f, 0.443f, 0.333f, 1.0f).process();
	exportImage("texture_example_colours_2", &light);
	int s11 = dotfile.add("Colours", "texture_example_colours_2");
	dotfile.bind(s10, s11);
	Procedural::Normals(&normal).process();
	exportImage("texture_example_normals_2", &normal);
	int s12 = dotfile.add("Normals", "texture_example_normals_2");
	dotfile.bind(s10, s12);
	Procedural::TextureLightBaker(&light).setNormalMap(&normal).setColourAmbient(0.164f, 0.0f, 0.0f, 0.0f).setPosition(255.0f, 255.0f, 200.0f).setBumpPower(48).setSpecularPower(8).process();
	exportImage("texture_example_light", &light);
	int s13 = dotfile.add("Light", "texture_example_light");
	dotfile.bind(s11, s13);
	dotfile.bind(s12, s13);
	Procedural::TextureBuffer joint(&bricks);
	Procedural::Invert(&joint).process();
	exportImage("texture_example_invert", &joint);
	int s14 = dotfile.add("Invert", "texture_example_invert");
	dotfile.bind(s10, s14);
	Procedural::Threshold(&joint).setThreshold(200).setRatio(255).process();
	exportImage("texture_example_threshold_1", &joint);
	int s15 = dotfile.add("Threshold", "texture_example_threshold_1");
	dotfile.bind(s14, s15);
	Procedural::Colours(&joint).setColourBase(0.215f, 0.207f, 0.137f, 0.0f).setColourPercent(0.294f, 0.266f, 0.345f, 1.0f).setBrightness(110).setContrast(153).process();
	exportImage("texture_example_colours_3", &joint);
	int s16 = dotfile.add("Colours", "texture_example_colours_3");
	dotfile.bind(s15, s16);
	Procedural::TextureBuffer colourcloud(&bricks);
	Procedural::Threshold(&colourcloud).process();
	exportImage("texture_example_threshold_2", &colourcloud);
	int s17 = dotfile.add("Threshold", "texture_example_threshold_2");
	dotfile.bind(s10, s17);
	Procedural::TextureBuffer cloud(&bricks);
	Procedural::Cloud(&cloud).process();
	exportImage("texture_example_cloud_2", &cloud);
	int s18 = dotfile.add("Cloud", "texture_example_cloud_2");
	Procedural::Combine(&colourcloud).addImage(&cloud, Procedural::Combine::METHOD_MULTIPLY).process();
	exportImage("texture_example_combine_1", &colourcloud);
	int s19 = dotfile.add("Combine", "texture_example_combine_1");
	dotfile.bind(s17, s19);
	dotfile.bind(s18, s19);
	Procedural::Colours(&colourcloud).setColourBase(0.329f, 0.141f, 0.0f, 0.0f).setColourPercent(0.95f, 0.949f, 0.862f, 1.0f).setBrightness(30).process();
	exportImage("texture_example_colours_4", &colourcloud);
	int s20 = dotfile.add("Colours", "texture_example_colours_4");
	dotfile.bind(s19, s20);
	Procedural::Combine(&light)
	.addImage(&joint, Procedural::Combine::METHOD_ADD_CLAMP)
	.addImage(&colourcloud, Procedural::Combine::METHOD_ADD_CLAMP)
	.process();
	exportImage("texture_example_combine_2_finish", &light);
	int s21 = dotfile.add("Combine", "texture_example_combine_2_finish");
	dotfile.bind(s13, s21);
	dotfile.bind(s16, s21);
	dotfile.bind(s20, s21);
	dotfile.save();
}

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
		Illustrations app;

		if (!app.init())
			return 1;

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		app.mOutputPath = strCmdLine;
#else
		size_t size=_PC_PATH_MAX;
		char path[size];
		app.mOutputPath = (argc > 1) ? argv[1] : getcwd(path,size);
#endif

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