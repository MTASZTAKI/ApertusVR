/*
-----------------------------------------------------------------------------
This source file is part of ogre-procedural

For the latest info, see http://code.google.com/p/ogre-procedural/

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
#ifndef __Test_h_
#define __Test_h_

#include "BaseApplication.h"
#include "Procedural.h"
using namespace Ogre;
using namespace Procedural;

class Unit_Test
{
protected:
	SceneManager* mSceneMgr;

	std::vector<Entity*> mEntities;
	std::vector<SceneNode*> mSceneNodes;
	Timer mTimer;

	void putMesh(const String& meshName, int materialIndex=0)
	{
		Entity* ent = mSceneMgr->createEntity(meshName);
		SceneNode* sn = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		sn->attachObject(ent);
		sn->showBoundingBox(false);
		switch (materialIndex)
		{
		case 1:
			ent->setMaterialName("Examples/Rockwall");
			break;
		case 2:
			ent->setMaterialName("Examples/Road");
			break;
		}
		mEntities.push_back(ent);
		mSceneNodes.push_back(sn);
		int currentIndex = mEntities.size();
		int numSN = mSceneNodes.size();
		int side = sqrt((float)numSN);
		for (int i=0; i<numSN; i++)
		{
			mSceneNodes[i]->setPosition(((i%side) - side * .5)*10., 0., -(i/side - side*.5)*10.);
		}
	}

	void putMesh(MeshPtr mesh, int materialIndex=0)
	{
		String s = mesh->getName();
		putMesh(s, materialIndex);
	}

public:
	Unit_Test(SceneManager* sn) : mSceneMgr(sn) {}

	virtual String getDescription()=0;
	virtual void initImpl()=0;

	void rotate(Real amount)
	{
		for (std::vector<SceneNode*>::iterator it = mSceneNodes.begin(); it!=mSceneNodes.end(); it++)
			(*it)->rotate(Vector3::UNIT_Y, (Radian)amount);
	}

	double init()
	{
		Utils::log("Loading test : " + getDescription());
		mTimer.reset();
		initImpl();
		Utils::log("Test loaded in : " + StringConverter::toString(mTimer.getMicroseconds() / 1000.0f) + " ms");
		return mTimer.getMicroseconds() / 1000.0f;
	}

	void destroy()
	{
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
};

class Unit_Tests : public BaseApplication
{
	/* --------------------------------------------------------------------------- */
	class Test_Boolean : public Unit_Test
	{
	public:
		Test_Boolean(SceneManager* sn) : Unit_Test(sn) {}

		String getDescription()
		{
			return "3D CSG";
		}

		void initImpl()
		{
			TriangleBuffer tb1;
			TriangleBuffer tb2;
			BoxGenerator().addToTriangleBuffer(tb1);
			//PlaneGenerator().setSizeX(5.0).setSizeY(5.0).addToTriangleBuffer(tb2);
			//BoxGenerator().setPosition(.5,.12,.11).addToTriangleBuffer(tb2);
			BoxGenerator().setSize(Vector3(2,.5,.5)).addToTriangleBuffer(tb2);
			//SphereGenerator().setScale(.7).addToTriangleBuffer(tb2);
			putMesh(Boolean().setBooleanOperation(Boolean::BT_DIFFERENCE).setMesh1(&tb1).setMesh2(&tb2).realizeMesh(), 1);
			//putMesh(tb1.transformToMesh("t1"),1);
			//putMesh(tb2.transformToMesh("t2"),1);
			putMesh(Boolean().setBooleanOperation(Boolean::BT_UNION).setMesh1(&tb1).setMesh2(&tb2).realizeMesh(), 1);
			putMesh(Boolean().setBooleanOperation(Boolean::BT_INTERSECTION).setMesh1(&tb1).setMesh2(&tb2).realizeMesh(), 1);

		}
	};
	/* --------------------------------------------------------------------------- */
	class Test_Primitives : public Unit_Test
	{
	public:
		Test_Primitives(SceneManager* sn) : Unit_Test(sn) {}

		String getDescription()
		{
			return "Primitive generation";
		}

		void initImpl()
		{
			putMesh(BoxGenerator().realizeMesh(), 1);
			putMesh(CapsuleGenerator().realizeMesh(), 1);
			putMesh(ConeGenerator().realizeMesh(), 1);
			putMesh(CylinderGenerator().realizeMesh(), 1);
			putMesh(IcoSphereGenerator().realizeMesh(), 1);
			putMesh(PlaneGenerator().realizeMesh(), 1);
			putMesh(RoundedBoxGenerator().realizeMesh(), 1);
			putMesh(SphereGenerator().realizeMesh(), 1);
			putMesh(TorusGenerator().realizeMesh(), 1);
			putMesh(TorusKnotGenerator().realizeMesh(), 1);
			putMesh(TubeGenerator().realizeMesh(), 1);
		}
	};
	/* --------------------------------------------------------------------------- */
	class Test_Triangulation : public Unit_Test
	{
	public:
		Test_Triangulation(SceneManager* sn) : Unit_Test(sn) {}

		String getDescription()
		{
			return "Delaunay Triangulation";
		}

		void initImpl()
		{
			Shape s1 = CircleShape().setNumSeg(16).realizeShape().scale(4,4);
			Shape s2 = CircleShape().setNumSeg(16).realizeShape().switchSide().scale(1,.3f).translate(1.5f*Vector2::UNIT_X);
			MultiShape ms = MultiShape().addShape(s1);
			for (int i=0; i<8; i++)
			{
				Shape s = s2;
				s.rotate((Radian)i/8.*Math::TWO_PI);
				ms.addShape(s);
			}
			putMesh(ms.realizeMesh());

			putMesh(Triangulator().setMultiShapeToTriangulate(&ms).realizeMesh());

			Path p = LinePath().realizePath();
			Extruder().setMultiShapeToExtrude(&ms).setExtrusionPath(&p).realizeMesh("extrudedMesh");
			putMesh("extrudedMesh",1);

			Shape s = Shape().addPoint(0.2f,.9f).addPoint(1,0).addPoint(1,1).addPoint(0,1).addPoint(0,2).addPoint(2,2).addPoint(2,-1).addPoint(0.,-.2f).setOutSide(SIDE_LEFT).close();
			putMesh(s.realizeMesh(),1);
			putMesh(Triangulator().setShapeToTriangulate(&s).realizeMesh(),1);

			Shape s3 = CircleShape().setNumSeg(16).realizeShape();
			putMesh(Triangulator().setShapeToTriangulate(&s3).realizeMesh());

			s3.translate(Vector2(.01f,0));
			putMesh(Triangulator().setShapeToTriangulate(&s3).realizeMesh());

			s3.translate(Vector2(.5f,0));
			putMesh(Triangulator().setShapeToTriangulate(&s3).realizeMesh());

			s3.translate(Vector2(.1f,0));
			putMesh(Triangulator().setShapeToTriangulate(&s3).realizeMesh());


			Shape s4;
			s4.addPointRel(-54.951207f,-16.247524f).addPointRel(27.24849f,0).addPointRel(2.54842f,5.29287f)
			.addPointRel(37.0501f,-0.19603f).addPointRel(1.56826f,-13.13416f).addPointRel(5.48891f,0)
			.addPointRel(2.15635f,12.35003f).addPointRel(30.58104f,-0.19603f).addPointRel(2.35239f,-4.70477f)
			.addPointRel(23.13181f,0).addPointRel(-0.19604f,7.8412902f).addPointRel(-131.92973f,0)
			.close()
			.scale(.1f);

			putMesh(Triangulator().setShapeToTriangulate(&s4).realizeMesh());

			//   Schema:
			//   3---4      7---8
			//   |   5------6   |
			//   2--------------1
			//
			Shape s5 = Shape()
			           .addPoint(1,0)       // 1
			           .addPoint(-1,0)      // 2
			           .addPoint(-1,-0.5f)   // 3
			           .addPoint(-0.5f,-0.5f) // 4
			           .addPoint(-0.5f,-0.1f) // 5
			           .addPoint(0.5f,-0.1f)  // 6
			           .addPoint(0.5f,-0.5f)  // 7
			           .addPoint(1,-0.5f)    // 8
			           .close();

			putMesh(Triangulator().setShapeToTriangulate(&s5).realizeMesh());

			// Tests for the "shape order bug": if a multishape contains shapes in an order or the opposite, artifacts may happen
			{
				Shape s6 = CircleShape().realizeShape();
				Shape s7 = Shape(s6).scale(.5f).switchSide();
				MultiShape ms(2, &s7, &s6);
				putMesh(Triangulator().setMultiShapeToTriangulate(&ms).realizeMesh());

				MultiShape ms2(2, &s6, &s7);
				putMesh(Triangulator().setMultiShapeToTriangulate(&ms2).realizeMesh());

			}

			{
				Shape s2 = RectangleShape().setHeight(.5).realizeShape().switchSide();
				Shape s3 = s2;
				s3.scale(1.5).switchSide();
				MultiShape ms(2, &s3, &s2);
				putMesh(Triangulator().setMultiShapeToTriangulate(&ms).realizeMesh());

				MultiShape ms2(2, &s2, &s3);
				putMesh(Triangulator().setMultiShapeToTriangulate(&ms2).realizeMesh());
			}
		}
	};

	/* --------------------------------------------------------------------------- */
	class Test_ShapeBoolean : public Unit_Test
	{
	public:
		Test_ShapeBoolean(SceneManager* sn) : Unit_Test(sn) {}

		String getDescription()
		{
			return "Tests different types of 2D CSG : union, difference and intersection";
		}

		void initImpl()
		{
			//CSG
			Shape s1 = CircleShape().realizeShape();
			Shape s2 = RectangleShape().setWidth(0.5).setHeight(2).realizeShape();

			s1.realizeMesh("contour1");
			putMesh("contour1");
			s2.realizeMesh("contour2");
			putMesh("contour2");
			MultiShape s;
			s = s1.booleanIntersect(s2);
			s.realizeMesh("contourinter");
			putMesh("contourinter");
			s = s1.booleanUnion(s2);
			s.realizeMesh("contourunion");
			putMesh("contourunion");
			s = s1.booleanDifference(s2);
			s.realizeMesh("contourdifference");
			putMesh("contourdifference");
		}
	};

	/* --------------------------------------------------------------------------- */
	class Test_ShapeThick : public Unit_Test
	{
	public:
		Test_ShapeThick(SceneManager* sn) : Unit_Test(sn) {}

		String getDescription()
		{
			return "Thicken a shape";
		}

		void initImpl()
		{
			//Open shape
			Shape s = Shape().addPoint(0,0).addPoint(5,5).addPoint(-5,10).addPoint(0,15);
			putMesh(s.realizeMesh());
			MultiShape ms = s.thicken(1.0);
			putMesh(ms.realizeMesh());

			//Closed shape
			Shape s2 = CircleShape().setNumSeg(16).realizeShape().translate(Vector2(.8f,0)).booleanUnion(CircleShape().setNumSeg(16).realizeShape().translate(Vector2(-.8f,0))).getShape(0);
			putMesh(s2.realizeMesh());
			MultiShape ms2 = s2.thicken(.1f);
			putMesh(ms2.realizeMesh());
		}
	};

	/* --------------------------------------------------------------------------- */
	class Test_Splines : public Unit_Test
	{
	public:
		Test_Splines(SceneManager* sn) : Unit_Test(sn) {}

		String getDescription()
		{
			return "Shape and path splines";
		}

		void initImpl()
		{
			// CatmullRomSpline
			CatmullRomSpline2 cs;
			cs.addPoint(0,-1)
			.addPoint(2,2)
			.addPoint(1,2.5f)
			.addPoint(0,1.5f)
			.addPoint(-1,2.5f)
			.addPoint(-2,2)
			.setNumSeg(8)
			.close();
			putMesh(cs.realizeShape().realizeMesh());

			// CubicHermite Spline
			CubicHermiteSpline2 chs;
			chs.addPoint(Vector2(0,0), Vector2(0,1), Vector2(0,1))
			.addPoint(Vector2(0,2), Vector2(1,0), Vector2(0,1))
			.addPoint(Vector2(2,2), Vector2(0,1), Vector2(0,1))
			.setNumSeg(8).close();
			putMesh(chs.realizeShape().realizeMesh());

			// Kochanek Bartels
			KochanekBartelsSpline2 kbs2;
			kbs2.addPoint(Vector2(0,-1),0,0,-1)
			.addPoint(Vector2(2,2))
			.addPoint(Vector2(1,3))
			.addPoint(Vector2(0,1.5f),0,0,-1)
			.addPoint(Vector2(-1,3))
			.addPoint(Vector2(-2,2))
			.setNumSeg(8)
			.close();

			putMesh(kbs2.realizeShape().realizeMesh());

			// RoundedCornerSpline2
			RoundedCornerSpline2 rcs2;
			rcs2.addPoint(0,0)
			.addPoint(1,0)
			.addPoint(1,1)
			.addPoint(2,1)
			.addPoint(3,0);
			putMesh(rcs2.realizeShape().realizeMesh());

			rcs2.addPoint(Vector2(1,-1))
			.setRadius(.2f)
			.close();
			putMesh(rcs2.realizeShape().realizeMesh());

			// RoundedCornerSpline3
			RoundedCornerSpline3 rcs3;
			rcs3.addPoint(0,0,0)
			.addPoint(0,1,0)
			.addPoint(1,1,1)
			.addPoint(0,1,1)
			.addPoint(0,2,1)
			.setRadius(.2f);
			putMesh(rcs3.realizePath().realizeMesh());

			// CatmullRomSpline3
			CatmullRomSpline3 cs3;
			cs3.addPoint(Vector3(0,-1,0))
			.addPoint(Vector3(2,2,0))
			.addPoint(Vector3(1,2.5f,0))
			.addPoint(Vector3(0,1.5f,0))
			.addPoint(Vector3(-1,2.5f,0))
			.addPoint(Vector3(-2,2,0))
			.setNumSeg(8)
			.close();
			putMesh(cs3.realizePath().realizeMesh());

			// CubicHermite Spline3
			CubicHermiteSpline3 chs3;
			chs3.addPoint(Vector3(0,0,0), Vector3(0,1,0), Vector3(0,1,0))
			.addPoint(Vector3(0,2,0), Vector3(1,0,0), Vector3(0,1,0))
			.addPoint(Vector3(2,2,0), Vector3(0,1,0), Vector3(0,1,0))
			.setNumSeg(8).close();
			putMesh(chs3.realizePath().realizeMesh());

			// Pseudo Track spline
			CircleShape circ;
			Shape s = circ.realizeShape();
			CatmullRomSpline2 cs2;
			Track t = cs2.addPoint(0,.8f).addPoint(.1f,1).addPoint(0.5f,0.5f).addPoint(1,1).realizeShape().convertToTrack(Track::AM_RELATIVE_LINEIC);
			Path p;
			p.addPoint(0,0,0).addPoint(1,1,1);
			Extruder ex;
			putMesh(ex.setExtrusionPath(&p).setShapeToExtrude(&s).setScaleTrack(&t).realizeMesh(),1);

			// AutoTangents
			CubicHermiteSpline2 chsAuto;
			chsAuto.addPoint(Vector2(0,0), AT_STRAIGHT).addPoint(Vector2(1,0),AT_STRAIGHT)
			.addPoint(Vector2(1,1), AT_STRAIGHT).addPoint(Vector2(2,1),AT_CATMULL)
			.addPoint(Vector2(2,0), AT_CATMULL).addPoint(Vector2(3,0),AT_CATMULL);
			putMesh(chsAuto.realizeShape().realizeMesh());

			CubicHermiteSpline3 chs3Auto;
			chs3Auto.addPoint(Vector3(0,0,0), AT_STRAIGHT).addPoint(Vector3(1,0,0),AT_STRAIGHT)
			.addPoint(Vector3(1,1,1), AT_STRAIGHT).addPoint(Vector3(2,1,1),AT_CATMULL)
			.addPoint(Vector3(2,0,1), AT_CATMULL).addPoint(Vector3(3,0,0),AT_CATMULL);
			putMesh(chs3Auto.realizePath().realizeMesh());
		}
	};
	/* --------------------------------------------------------------------------- */
	class Test_Extruder_MultiPath : public Unit_Test
	{
	public:
		Test_Extruder_MultiPath(SceneManager* sn) : Unit_Test(sn) {}

		String getDescription()
		{
			return "Extruder multi path";
		}

		void initImpl()
		{
			/*{
			Shape s;
			s.addPoint(-5,0).addPoint(0,0).addPoint(5,0).addPoint(6,5).addPoint(0,1).addPoint(-6,5).close().setOutSide(SIDE_RIGHT).scale(0.1);
			Path p;
			p.addPoint(-10,0,0).addPoint(5,0,0).addPoint(10,0,0).addPoint(15,0,5).addPoint(20,0,10);
			Path p2;
			p2.addPoint(10,0,0).addPoint(15,0,-5).addPoint(20,0,-10);
			MultiPath mp;
			mp.addPath(p).addPath(p2);
			putMesh(Extruder().setShapeToExtrude(&s).setExtrusionPath(&mp).realizeMesh(),1);
			}*/

			{
				Path p = Path().addPoint(-30,0,5).addPoint(5,0,0).addPoint(10,0,0).addPoint(15,0,5).addPoint(30,0,15);
				Path p2 = Path().addPoint(10,0,-30).addPoint(10,0,-5).addPoint(10,0,0).addPoint(10,0,10).addPoint(10,0,30);
				//Path p2 = Path().addPoint(10,0,0).addPoint(15,0,-5).addPoint(20,0,-10);

				MultiPath mp = MultiPath().addPath(p).addPath(p2);
				//Path p = Path().addPoint(-10,0,0).addPoint(10,0,0);
				Shape s = Procedural::Shape().addPoint(-1.2f,.2f).addPoint(-1.f,.2f).addPoint(-.9f,.1f).addPoint(0,.1f).addPoint(.9f,.1f).addPoint(1.f,.2f).addPoint(1.2f,.2f).scale(2).setOutSide(Procedural::SIDE_LEFT);
				//Track textureTrack = Procedural::Track(Procedural::Track::AM_POINT).addKeyFrame(0,0).addKeyFrame(2,.2f).addKeyFrame(4,.8f).addKeyFrame(6,1);
				Track textureTrack = Procedural::Track(Procedural::Track::AM_POINT).addKeyFrame(0,0).addKeyFrame(2,.2f).addKeyFrame(3,.51f).addKeyFrame(4,.2f).addKeyFrame(6,0);
				Extruder().setExtrusionPath(&mp).setShapeToExtrude(&s).setShapeTextureTrack(&textureTrack).setUTile(20.f).realizeMesh("extrudedMesh");
				putMesh("extrudedMesh", 2);
			}
		}
	};
	/* --------------------------------------------------------------------------- */
	class Test_Extruder : public Unit_Test
	{
	public:
		Test_Extruder(SceneManager* sn) : Unit_Test(sn) {}

		String getDescription()
		{
			return "Extruder";
		}

		void initImpl()
		{
			{
				Shape shape = Shape().addPoint(0,0).addPoint(0,1).addPoint(1,1).addPoint(1,0).setOutSide(SIDE_RIGHT).close();
				Shape shape2 = Shape().addPoint(1,0).addPoint(1,1).addPoint(0,1).addPoint(0,0).setOutSide(SIDE_LEFT).close();
				Path line = LinePath().betweenPoints(Vector3::ZERO, Vector3(1,10,0)).setNumSeg(2).realizePath();
				Path line2 = LinePath().betweenPoints(Vector3(1,10,0), Vector3::ZERO).setNumSeg(2).realizePath();
				Extruder e;
				e.setCapped(false);

				// linear extrusion
				putMesh(e.setShapeToExtrude(&shape).setExtrusionPath(&line).realizeMesh(),1);
				putMesh(e.setShapeToExtrude(&shape2).setExtrusionPath(&line).realizeMesh(),1);
				putMesh(e.setShapeToExtrude(&shape).setExtrusionPath(&line2).realizeMesh(),1);
				putMesh(e.setShapeToExtrude(&shape2).setExtrusionPath(&line2).realizeMesh(),1);
			}

			// extrusion with rotation and scale track
			{
				Path l = LinePath().betweenPoints(Vector3(0,10,0),Vector3::ZERO).setNumSeg(20).realizePath();
				Track t = Track().addKeyFrame(2,0).addKeyFrame(5,1.5f).addKeyFrame(8,0);
				Track t2 = Track().addKeyFrame(0,.8f).addKeyFrame(10,1.2f);
				Shape s = RectangleShape().realizeShape();
				Extruder ex;
				putMesh(ex.setShapeToExtrude(&s).setExtrusionPath(&l).setRotationTrack(&t).setScaleTrack(&t2).realizeMesh(),1);
			}

			// tests different addressing modes for the (scale) track
			{
				Path l = LinePath().betweenPoints(Vector3(0,10,0),Vector3::ZERO).setNumSeg(20).realizePath();
				Path l2 = LinePath().betweenPoints(Vector3(0,10,0),Vector3::ZERO).realizePath();
				Track t = Track(Track::AM_ABSOLUTE_LINEIC).addKeyFrame(0,1).addKeyFrame(5,1.5f).addKeyFrame(8,1);
				Shape s = RectangleShape().realizeShape();
				Extruder ex;
				// Absolute lineic
				//putMesh(ex.setShapeToExtrude(&s).setExtrusionPath(&l).setScaleTrack(&t).realizeMesh(),1);
				putMesh(ex.setShapeToExtrude(&s).setExtrusionPath(&l2).setScaleTrack(&t).realizeMesh(),1);
				// Relative Lineic
				Track t2 = Track(Track::AM_RELATIVE_LINEIC).addKeyFrame(0,1).addKeyFrame(0.5f,1.5f).addKeyFrame(1,1);
				//putMesh(ex.setScaleTrack(&t2).realizeMesh(),1);
				putMesh(ex.setScaleTrack(&t2).realizeMesh(),1);
				// On path point
				Extruder ex3;
				ex3.setShapeToExtrude(&s).setExtrusionPath(&l);
				Track t3 = Track(Track::AM_POINT).addKeyFrame(0,1).addKeyFrame(3,1.5f).addKeyFrame(4,1);
				//putMesh(ex3.setScaleTrack(&t3).realizeMesh(),1);

				Path l3 = LinePath().betweenPoints(Vector3(0,10,0),Vector3::ZERO).setNumSeg(20).realizePath();
				Shape unaxed = CircleShape().setNumSeg(8).realizeShape().translate(Vector2(2,0));
				Shape unaxed2 = CircleShape().setNumSeg(8).realizeShape().translate(Vector2(-2,0));
				MultiShape ms2 = MultiShape().addShape(unaxed).addShape(unaxed2);
				Track rotation = Track(Track::AM_RELATIVE_LINEIC).addKeyFrame(0,0).addKeyFrame(1,-Math::TWO_PI);
				Track rotation2 = Track(Track::AM_RELATIVE_LINEIC).addKeyFrame(0,0).addKeyFrame(1,Math::TWO_PI);
				Extruder ex2;
				putMesh(ex2.setMultiShapeToExtrude(&ms2).setExtrusionPath(&l3).setRotationTrack(&rotation).realizeMesh(),1);
				putMesh(ex2.setMultiShapeToExtrude(&ms2).setExtrusionPath(&l3).setRotationTrack(&rotation2).realizeMesh(),1);

				Shape unaxed3 = CircleShape().setNumSeg(8).realizeShape().translate(Vector2(.4f,0));
				Track r = Track(Track::AM_RELATIVE_LINEIC).addKeyFrame(0,0).addKeyFrame(1,-3*Math::TWO_PI);
				Track r2 = Track(Track::AM_RELATIVE_LINEIC).addKeyFrame(0,0).addKeyFrame(1,3*Math::TWO_PI);
				putMesh(ex2.setShapeToExtrude(&unaxed3).setExtrusionPath(&l3).setRotationTrack(&r).realizeMesh(),1);
				putMesh(ex2.setShapeToExtrude(&unaxed3).setExtrusionPath(&l3).setRotationTrack(&r2).realizeMesh(),1);
			}

			{
				// Test irregular texture distribution on shape
				Shape circle = CircleShape().setNumSeg(20).realizeShape();
				//Path line = LinePath().betweenPoints(Vector3(0,10,0),Vector3::ZERO).setNumSeg(20).realizePath();
				Path line = RoundedCornerSpline3().addPoint(0,0,0).addPoint(0,4,0).addPoint(0,6,2).addPoint(0,10,2).realizePath();
				Track t = Track(Track::AM_RELATIVE_LINEIC).addKeyFrame(0,0).addKeyFrame(0.5f,0.2f).addKeyFrame(1,1);
				Extruder ex;
				putMesh(ex.setShapeToExtrude(&circle).setExtrusionPath(&line).setShapeTextureTrack(&t).realizeMesh(),1);

				// Test irregular texture distribution on path
				putMesh(ex.setShapeTextureTrack(0).setPathTextureTrack(&t).realizeMesh(), 1);

				// Test texture UV switch
				putMesh(ex.setPathTextureTrack(0).setSwitchUV(true).realizeMesh(),1);
			}

			{
				Shape s = Shape().addPoint(-1,-1).addPoint(1,-1).addPoint(1,1).addPoint(0,0).addPoint(-1,1).close();
				Path p = RoundedCornerSpline3().addPoint(-10,5,-2.5).addPoint(-5,0,-2.5).addPoint(0,0,2.5).addPoint(5,0,-2.5).setRadius(2.).realizePath();
				MeshPtr mp = Extruder().setShapeToExtrude(&s).setExtrusionPath(&p).realizeMesh();
				putMesh(mp, 1);

				Shape s2 = RectangleShape().setHeight(.5).realizeShape();
				Track t = Track(Track::AM_RELATIVE_LINEIC).addKeyFrame(0,0).addKeyFrame(0.5,0.0).addKeyFrame(1.0,1.0);
				Path p2 = LinePath().betweenPoints(Vector3(-5,0,0),Vector3(5,0,0)).setNumSeg(10).realizePath();
				mp = Extruder().setShapeToExtrude(&s2).setExtrusionPath(&p2).setRotationTrack(&t).realizeMesh();
				putMesh(mp, 1);
			}

			{
				MeshPtr mp;
				Shape s2 = RectangleShape().setHeight(.5).realizeShape().switchSide();
				Shape s3 = s2;
				s3.scale(1.5).switchSide();
				MultiShape ms = MultiShape(2, &s2, &s3);
				Path p3 = CatmullRomSpline3().addPoint(0,5,-5).addPoint(0,0,0).addPoint(0,0,5).realizePath();
				mp = Extruder().setMultiShapeToExtrude(&ms).setExtrusionPath(&p3).realizeMesh();
				putMesh(mp, 1);
			}

			{
				MeshPtr mp;
				Path p;
				for (int i=0; i<32*32; i++)
				{
					Radian r1 = (Radian)i/1024.*Math::TWO_PI;
					Radian r2 = (Radian)i/32.*Math::TWO_PI;
					Vector3 v (1+.5*Math::Cos(r2), .5*Math::Sin(r2), 0);
					Quaternion q;
					q.FromAngleAxis(r1, Vector3::UNIT_Y);
					p.addPoint(q*v);
				}
				Shape s = RectangleShape().setWidth(.1f).setHeight(.05f).realizeShape();
				mp = Extruder().setExtrusionPath(&p).setShapeToExtrude(&s).realizeMesh();
				putMesh(mp,1);
			}

			{
				MeshPtr mp;
				Shape s2 = RectangleShape().setHeight(.5).realizeShape();
				MultiShape ms = MultiShape(2, &(s2.switchSide()), &(Shape(s2).scale(1.5)));
				Path p3 = CatmullRomSpline3().addPoint(0,5,-5).addPoint(0,0,0).addPoint(0,0,5).realizePath();
				mp = Extruder().setMultiShapeToExtrude(&ms).setExtrusionPath(&p3).realizeMesh();
				putMesh(mp,1);
			}

		}
	};

	/* --------------------------------------------------------------------------- */
	class Test_Lathe : public Unit_Test
	{
	public:
		Test_Lathe(SceneManager* sn) : Unit_Test(sn) {}

		String getDescription()
		{
			return "Lathe";
		}

		void initImpl()
		{
			CatmullRomSpline2 cs;
			cs.addPoint(0,0).addPoint(1,0).addPoint(3,5).addPoint(1,10).addPoint(0,11);
			Shape s = cs.realizeShape();
			Lathe l = Lathe().setShapeToExtrude(&s);
			putMesh(l.realizeMesh(),1);

			l.setAngleBegin(Degree(90)).setAngleEnd(Degree(200));
			l.setCapped(false);
			putMesh(l.realizeMesh(),1);

			l.setCapped(true);
			putMesh(l.realizeMesh(),1);

			l.setAngleBegin(Degree(200)).setAngleEnd(Degree(90));
			putMesh(l.realizeMesh(),1);

			Procedural::Shape outerCircleShape = Procedural::CircleShape().setRadius(4.f).setNumSeg(50).realizeShape().translate(10,0);
			Procedural::Shape innerCircleShape = Procedural::CircleShape().setRadius(3.8f).setNumSeg(50).realizeShape().translate(10,0);

			Procedural::MultiShape tubeMultiShape = outerCircleShape.booleanDifference(innerCircleShape);

			//Procedural::MultiShape tubeMultiShape(2, outerCircleShape, innerCircleShape);

			putMesh(Procedural::Lathe().setMultiShapeToExtrude(&tubeMultiShape).setNumSeg(30).setAngleBegin((Ogre::Radian)0.f).setAngleEnd((Ogre::Radian)Ogre::Math::PI).realizeMesh(), 1);


		}
	};

	/* --------------------------------------------------------------------------- */
	class Test_InvertNormals : public Unit_Test
	{
	public:
		Test_InvertNormals(SceneManager* sn) : Unit_Test(sn) {}

		String getDescription()
		{
			return "Invert Normals";
		}

		void initImpl()
		{
			TriangleBuffer tbuff;
			IcoSphereGenerator().setNumIterations(1).setRadius(5).addToTriangleBuffer(tbuff);
			//tbuff.invertNormals();
			putMesh(tbuff.transformToMesh("sphere"), 1);
			//putMesh(IcoSphereGenerator().setRadius(5).realizeMesh(),1);
		}
	};



	/* --------------------------------------------------------------------------- */
	std::vector<Unit_Test*> mUnitTests;

	int mCurrentTestIndex;

	void switchToTest(int index)
	{
		if (index == mCurrentTestIndex)
			return;
		if (mCurrentTestIndex >= 0)
			mUnitTests[mCurrentTestIndex]->destroy();

		double time = mUnitTests[index]->init();
		String test_description = mUnitTests[index]->getDescription();

		// update text here:
		String txt = "[OgreProcedural Unit Tests] (Use key N/M to switch between tests)\n";
		txt += "[" + StringConverter::toString(index+1) + "/" + StringConverter::toString(mUnitTests.size()) + "] ";

		// and add the description
		txt += test_description;

		// properly print the time ...
		char time_str[255] = {0};
		sprintf(time_str, " (loaded in %6.6f ms)", time);
		txt += String(time_str);

		// and finally set it
		OverlayManager::getSingleton().getOverlayElement("myText")->setCaption(txt);


		mCurrentTestIndex = index;
	}

	void nextTest()
	{
		switchToTest(Utils::modulo(mCurrentTestIndex+1,mUnitTests.size()));
	}
	void previousTest()
	{
		switchToTest(Utils::modulo(mCurrentTestIndex-1,mUnitTests.size()));
	}

protected:
	bool keyReleased( const OIS::KeyEvent& arg )
	{
		if (arg.key == OIS::KC_M || arg.key == OIS::KC_ADD || arg.key == OIS::KC_PGDOWN)
		{
			nextTest();
			return true;
		}
		if (arg.key == OIS::KC_N || arg.key == OIS::KC_SUBTRACT || arg.key == OIS::KC_PGUP)
		{
			previousTest();
			return true;
		}
		return BaseApplication::keyReleased(arg);
	}

	bool keyPressed(const OIS::KeyEvent& arg)
	{
		if (arg.key == OIS::KC_K)
		{
			mUnitTests[mCurrentTestIndex]->rotate(-.3f);
			return true;
		}

		if (arg.key == OIS::KC_L)
		{
			mUnitTests[mCurrentTestIndex]->rotate(.3f);
			return true;
		}

		return BaseApplication::keyPressed(arg);
	}

	virtual void createScene(void);

	virtual void createCamera(void);

	virtual void createViewports(void);

	virtual bool frameStarted(const FrameEvent& evt);

	virtual void destroyScene(void);
public:
	Unit_Tests() : mCurrentTestIndex(-1) // -1 so the first test will always be loaded
	{}

};

#endif // #ifndef __TutorialApplication_h_
