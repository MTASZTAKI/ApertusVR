/*MIT License

Copyright (c) 2016 MTA SZTAKI

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

#ifndef APE_OGREMOVABLETEXT_H
#define APE_OGREMOVABLETEXT_H

#include "OgreFontManager.h"
#include "OgreMovableObject.h"
#include "OgreMaterialManager.h"
#include "OgreHardwareBufferManager.h"
#include "OgreRoot.h"
#include "OgreCamera.h"
#include "OgreNode.h"

namespace Ape {


	class OgreMovableTextFactory : public Ogre::MovableObjectFactory
	{
	private:
		Ogre::String mTypeName;

	protected:
		Ogre::MovableObject* createInstanceImpl( const Ogre::String& name, const Ogre::NameValuePairList* params = 0);
		
	public:
		OgreMovableTextFactory();

		~OgreMovableTextFactory();

		const Ogre::String& getType(void) const;

		Ogre::MovableObject* createInstance( const Ogre::String& name, Ogre::SceneManager* manager, const Ogre::NameValuePairList* params = 0);

		void destroyInstance(Ogre::MovableObject* obj);
	};

	class OgreMovableText : public Ogre::MovableObject, public Ogre::Renderable
	{
	public:
		enum HorizontalAlignment    {H_LEFT, H_CENTER};

		enum VerticalAlignment      {V_BELOW, V_ABOVE, V_CENTER};

	protected:
		Ogre::String mFontName;
		
		Ogre::String mType;
		
		Ogre::String mName;
		
		Ogre::String mCaption;
		
		HorizontalAlignment	mHorizontalAlignment;
		
		VerticalAlignment mVerticalAlignment;

		Ogre::ColourValue mColor;
		
		Ogre::RenderOperation mRenderOp;
		
		Ogre::AxisAlignedBox mAABB;
		
		Ogre::LightList	mLList;
		
		Ogre::Real mCharHeight;
		
		Ogre::Real mSpaceWidth;

		bool mNeedUpdate;
		
		bool mUpdateColors;
		
		bool mOnTop;

		Ogre::Real mTimeUntilNextToggle;
		
		Ogre::Real mRadius;

		Ogre::Vector3 mGlobalTranslation;
		
		Ogre::Vector3 mLocalTranslation;

		Ogre::Camera* mpCam;
		
		Ogre::RenderWindow* mpWin;
		
		Ogre::Font * mpFont;
		
		Ogre::MaterialPtr mpMaterial;
		
		Ogre::MaterialPtr mpBackgroundMaterial;

	public:
		OgreMovableText(const Ogre::String &name, const Ogre::String &caption, const Ogre::String &fontName = "BlueHighway", Ogre::Real charHeight = 1.0, const Ogre::ColourValue &color = Ogre::ColourValue::White);

		virtual ~OgreMovableText();

		virtual void visitRenderables(Ogre::Renderable::Visitor* visitor, bool debugRenderables = false);

		void    setFontName(const Ogre::String &fontName);

		void    setCaption(const Ogre::String &caption);

		void    setColor(const Ogre::ColourValue &color);

		void    setCharacterHeight(Ogre::Real height);

		void    setSpaceWidth(Ogre::Real width);

		void    setTextAlignment(const HorizontalAlignment& horizontalAlignment, const VerticalAlignment& verticalAlignment);

		void    setGlobalTranslation( Ogre::Vector3 trans );

		void    setLocalTranslation( Ogre::Vector3 trans );

		void    showOnTop(bool show=true);

		const   Ogre::String          &getFontName() const {return mFontName;}

		const   Ogre::String          &getCaption() const {return mCaption;}

		const   Ogre::ColourValue     &getColor() const {return mColor;}

		Ogre::Real    getCharacterHeight() const {return mCharHeight;}

		Ogre::Real    getSpaceWidth() const {return mSpaceWidth;}

		Ogre::Vector3    getGlobalTranslation() const {return mGlobalTranslation;}

		Ogre::Vector3    getLocalTranslation() const {return mLocalTranslation;}

		bool    getShowOnTop() const {return mOnTop;}

		Ogre::AxisAlignedBox	        GetAABB(void) { return mAABB; }

		std::string getMaterialName();

	protected:
		void	_setupGeometry();

		void	_updateColors();

		void    getWorldTransforms(Ogre::Matrix4 *xform) const;

		Ogre::Real    getBoundingRadius(void) const {return mRadius;};

		Ogre::Real    getSquaredViewDepth(const Ogre::Camera *cam) const {return 0;};

		const   Ogre::Quaternion        &getWorldOrientation(void) const;

		const   Ogre::Vector3           &getWorldPosition(void) const;

		const   Ogre::AxisAlignedBox    &getBoundingBox(void) const {return mAABB;};

		const   Ogre::String            &getName(void) const {return mName;};

		const   Ogre::String            &getMovableType(void) const {static Ogre::String movType = "MovableText"; return movType;};

		void    _notifyCurrentCamera(Ogre::Camera *cam);

		void    _updateRenderQueue(Ogre::RenderQueue* queue);

		void    getRenderOperation(Ogre::RenderOperation &op);

		const   Ogre::MaterialPtr       &getMaterial(void) const {assert(!mpMaterial.isNull());return mpMaterial;};

		const   Ogre::LightList         &getLights(void) const {return mLList;};
	};
}

#endif
