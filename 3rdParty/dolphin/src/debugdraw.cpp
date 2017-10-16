#include <iostream>
#include "debugdraw.h"

#ifndef M_PI
#define M_PI    3.14159265358979323846f
#endif
CDebugDraw::CDebugDraw( Ogre::SceneManager* Scene, btDynamicsWorld* World )
: World( World ), IsActive( true )
{

	// generate a unit sphere
	std::size_t CircleSubdivs = 12;
	UnitCircle.reserve( CircleSubdivs+1 );

	for ( std::size_t i = 0; i < CircleSubdivs; ++i )
	{
		float Angle = (M_PI * 2.f * i) / CircleSubdivs;
		UnitCircle.push_back( Ogre::Vector2( std::cos( Angle ), std::sin( Angle ) ) );
	}
	UnitCircle.push_back( UnitCircle.front() );

	// setup rendering properties
	DebugLineDrawer = new CDynamicLineDrawer();
	if ( !Scene->hasSceneNode("DebugDrawer") )
		DebugDrawerNode = Scene->getRootSceneNode()->createChildSceneNode("DebugDrawer");
	else
	{
		DebugDrawerNode = Scene->getSceneNode("DebugDrawer");
		Ogre::SceneNode::ObjectIterator Iter = DebugDrawerNode->getAttachedObjectIterator();
		while( Iter.hasMoreElements() )
			Scene->destroyEntity( dynamic_cast<Ogre::Entity*>( Iter.getNext() ) );
	}
	DebugDrawerNode->attachObject( DebugLineDrawer );

	DebugLineDrawer->setMaterial("DebugLines");

	World->setDebugDrawer( this );
}

CDebugDraw::~CDebugDraw()
{
	delete DebugLineDrawer;
	World->setDebugDrawer(NULL);
}

void CDebugDraw::Update()
{
	if ( IsActive )
	{
		World->debugDrawWorld();
		DebugLineDrawer->Update();
		DebugDrawerNode->needUpdate();
		DebugLineDrawer->Clear();
	}
	else
	{
		DebugLineDrawer->Clear();
		DebugLineDrawer->Update();
		DebugDrawerNode->needUpdate();		
	}
}

void CDebugDraw::setDebugMode( int DebugMode )
{
	this->IsActive = DebugMode!=0;

	if ( !IsActive )
	{
		DebugLineDrawer->Clear();
	}
}

int CDebugDraw::getDebugMode() const
{
	return IsActive ? 1 : 0;
}

void CDebugDraw::drawSphere( const btVector3& p, btScalar radius, const btVector3& BColor )
{
	/*Ogre::ColourValue Color( BColor[0], BColor[1], BColor[2] );

	ManualObject.begin( "", Ogre::RenderOperation::OT_LINE_LIST );
	for ( std::size_t i = 1; i < UnitCircle.size(); ++i )
	{
		Ogre::Vector2& c0 = UnitCircle[i-1];
		ManualObject.position( p[0]+c0[0]*radius, p[1]+c0[1]*radius, p[2] );
		ManualObject.colour( Color );
		Ogre::Vector2& c1 = UnitCircle[i];
		ManualObject.position( p[0]+c1[0]*radius, p[1]+c1[1]*radius, p[2] );
		ManualObject.colour( Color );
	}
	
	for ( std::size_t i = 1; i < UnitCircle.size(); ++i )
	{
		Ogre::Vector2& c0 = UnitCircle[i-1];
		ManualObject.position( p[0]+c0[0]*radius, p[1], p[2]+c0[1]*radius );
		ManualObject.colour( Color );
		Ogre::Vector2& c1 = UnitCircle[i];
		ManualObject.position( p[0]+c1[0]*radius, p[1], p[2]+c1[1]*radius );
		ManualObject.colour( Color );
	}

	for ( std::size_t i = 1; i < UnitCircle.size(); ++i )
	{
		Ogre::Vector2& c0 = UnitCircle[i-1];
		ManualObject.position( p[0], p[1]+c0[1]*radius, p[2]+c0[0]*radius );
		ManualObject.colour( Color );
		Ogre::Vector2& c1 = UnitCircle[i];
		ManualObject.position( p[0], p[1]+c1[1]*radius, p[2]+c1[0]*radius );
		ManualObject.colour( Color );
	}
	ManualObject.end();*/
}

void CDebugDraw::drawLine( const btVector3& from, const btVector3& to,
						  const btVector3& color )
{
	DebugLineDrawer->AddPoint( Ogre::Vector3(from.getX(), from.getY(), from.getZ()), Ogre::ColourValue(color.getX(), color.getY(), color.getZ()) );
	DebugLineDrawer->AddPoint( Ogre::Vector3(to.getX(), to.getY(), to.getZ()), Ogre::ColourValue(color.getX(), color.getY(), color.getZ()) );
}


void CDebugDraw::drawTriangle( const btVector3& v0, const btVector3& v1, const btVector3& v2,
					   const btVector3& color, btScalar alpha )
{
	/*Ogre::ColourValue OgreColor( color.x(), color.y(), color.z(), alpha );

	ManualObject.begin( "", Ogre::RenderOperation::OT_LINE_LIST );
		ManualObject.position( Ogre::Vector3( v0 ) );
		ManualObject.colour( OgreColor );
		ManualObject.position( Ogre::Vector3( v1 ) );
		ManualObject.colour( OgreColor );
		ManualObject.position( Ogre::Vector3( v1 ) );
		ManualObject.colour( OgreColor );
		ManualObject.position( Ogre::Vector3( v2 ) );
		ManualObject.colour( OgreColor );
		ManualObject.position( Ogre::Vector3( v2 ) );
		ManualObject.colour( OgreColor );
		ManualObject.position( Ogre::Vector3( v0 ) );
		ManualObject.colour( OgreColor );
	ManualObject.end();*/
}

void CDebugDraw::drawContactPoint( const btVector3& PointOnB, const btVector3& normalOnB,
	btScalar distance, int lifeTime, const btVector3& color )
{
}

void CDebugDraw::reportErrorWarning( const char* WarningString )
{
	std::cout << WarningString << std::flush;
}

void CDebugDraw::draw3dText( const btVector3& location, const char* textString )
{
}