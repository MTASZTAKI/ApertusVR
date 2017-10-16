#ifndef DEBUG_DRAW_HPP
#define DEBUG_DRAW_HPP

#include <btBulletDynamicsCommon.h>
#include <Ogre.h>

#include "dynamiclinedrawer.h"

/** Implements the bullet debug draw interface for drawing with ogre.
	\ingroup Misc
*/
class CDebugDraw :
	public btIDebugDraw
{
public:
	CDebugDraw( Ogre::SceneManager* Scene, btDynamicsWorld* World );
	~CDebugDraw();

	/** Moves the data from the world to the ogre object.
		Should be called once every frame.
	*/
	void		Update();
	void setDebugMode(int DebugMode);
protected:
	
	int getDebugMode() const;

	// these implement the debug/drawing interface, no need to call them from the outside
	void drawLine( const btVector3& from, const btVector3& to, const btVector3& color );
	void drawContactPoint( const btVector3& PointOnB, const btVector3& normalOnB,
		btScalar distance, int lifeTime, const btVector3& color );
	void reportErrorWarning( const char *warningString );
	void draw3dText( const btVector3& location, const char* textString );
	void drawSphere( const btVector3& p, btScalar radius, const btVector3& color );
	void drawTriangle( const btVector3&v0, const btVector3 &v1, const btVector3 &v2,
					   const btVector3 &color, btScalar alpha );


private:
	std::vector<Ogre::Vector2> UnitCircle;

	CDynamicLineDrawer* DebugLineDrawer;
	Ogre::SceneNode*	DebugDrawerNode;
	btDynamicsWorld*	World;
	bool				IsActive;
};

#endif
