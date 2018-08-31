#include "ApeOgreMovableText.h"

#define POS_TEX_BINDING    0
#define COLOUR_BINDING     1

Ape::OgreMovableText::OgreMovableText(const Ogre::String &name, const Ogre::String &caption, const Ogre::String &fontName, Ogre::Real charHeight, const Ogre::ColourValue &color)
: mpCam(NULL)
, mpWin(NULL)
, mpFont(NULL)
, mName(name)
, mCaption(caption)
, mFontName(fontName)
, mCharHeight(charHeight)
, mColor(color)
, mType("MovableText")
, mTimeUntilNextToggle(0)
, mSpaceWidth(0)
, mUpdateColors(true)
, mOnTop(false)
, mHorizontalAlignment(H_LEFT)
, mVerticalAlignment(V_BELOW)
, mGlobalTranslation(0.0)
, mLocalTranslation(0.0)
{
    if (name == "")
        throw Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS, "Trying to create MovableText without name", "MovableText::MovableText");

    if (caption == "")
        throw Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS, "Trying to create MovableText without caption", "MovableText::MovableText");

    mRenderOp.vertexData = NULL;
    this->setFontName(mFontName);
    this->_setupGeometry();
}

Ape::OgreMovableText::~OgreMovableText()
{
    if (mRenderOp.vertexData)
        delete mRenderOp.vertexData;
    if (!mpMaterial.isNull())
	{
		std::string matName = mpMaterial->getName();
        Ogre::MaterialManager::getSingletonPtr()->remove(matName);
		mpMaterial.setNull();
	}
}

void Ape::OgreMovableText::setFontName(const Ogre::String &fontName)
{
    if((Ogre::MaterialManager::getSingletonPtr()->resourceExists(mName + "Material"))) 
    { 
        Ogre::MaterialManager::getSingleton().remove(mName + "Material"); 
    }

    if (mFontName != fontName || mpMaterial.isNull() || !mpFont)
    {
        mFontName = fontName;
		Ogre::ResourceManager::ResourceCreateOrRetrieveResult result = Ogre::FontManager::getSingletonPtr()->createOrRetrieve("MovableTextFont", "General");
		mpFont = (Ogre::Font*)result.first.getPointer();
		mpFont->setParameter("type","truetype");
		mpFont->setParameter("source","solo5.ttf");
		mpFont->setParameter("size","26");
		mpFont->setParameter("resolution","96");

        mpFont->load();
        if (!mpMaterial.isNull())
        {
            Ogre::MaterialManager::getSingletonPtr()->remove(mpMaterial->getName());
            mpMaterial.setNull();
        }

        mpMaterial = mpFont->getMaterial()->clone(mName + "Material");
        if (!mpMaterial->isLoaded())
            mpMaterial->load();

        mpMaterial->setDepthCheckEnabled(!mOnTop);
        mpMaterial->setDepthBias(1.0,1.0);
        mpMaterial->setDepthWriteEnabled(mOnTop);
        mpMaterial->setLightingEnabled(false);
        mNeedUpdate = true;
    }
}

void Ape::OgreMovableText::setCaption(const Ogre::String &caption)
{
    if (caption != mCaption)
    {
        mCaption = caption;
        mNeedUpdate = true;
    }
}

void Ape::OgreMovableText::setColor(const Ogre::ColourValue &color)
{
    if (color != mColor)
    {
        mColor = color;
        mUpdateColors = true;
    }
}

void Ape::OgreMovableText::setCharacterHeight(Ogre::Real height)
{
    if (height != mCharHeight)
    {
        mCharHeight = height;
        mNeedUpdate = true;
    }
}

void Ape::OgreMovableText::setSpaceWidth(Ogre::Real width)
{
    if (width != mSpaceWidth)
    {
        mSpaceWidth = width;
        mNeedUpdate = true;
    }
}

void Ape::OgreMovableText::setTextAlignment(const HorizontalAlignment& horizontalAlignment, const VerticalAlignment& verticalAlignment)
{
    if(mHorizontalAlignment != horizontalAlignment)
    {
        mHorizontalAlignment = horizontalAlignment;
        mNeedUpdate = true;
    }
    if(mVerticalAlignment != verticalAlignment)
    {
        mVerticalAlignment = verticalAlignment;
        mNeedUpdate = true;
    }
}

void Ape::OgreMovableText::setGlobalTranslation( Ogre::Vector3 trans )
{
    mGlobalTranslation = trans;
}

void Ape::OgreMovableText::setLocalTranslation( Ogre::Vector3 trans )
{
    mLocalTranslation = trans;
}

void Ape::OgreMovableText::showOnTop(bool show)
{
    if( mOnTop != show && !mpMaterial.isNull() )
    {
        mOnTop = show;
        mpMaterial->setDepthBias(1.0,1.0);
        mpMaterial->setDepthCheckEnabled(!mOnTop);
        mpMaterial->setDepthWriteEnabled(mOnTop);
    }
}

void Ape::OgreMovableText::_setupGeometry()
{
    assert(mpFont);
    assert(!mpMaterial.isNull());

    unsigned int vertexCount = static_cast<unsigned int>(mCaption.size() * 6);

	if (vertexCount == 0)
		return;

    if (mRenderOp.vertexData)
    {
		delete mRenderOp.vertexData;
		mRenderOp.vertexData = NULL;
		mUpdateColors = true;
    }

    if (!mRenderOp.vertexData)
        mRenderOp.vertexData = new Ogre::VertexData();

    mRenderOp.indexData = 0;
    mRenderOp.vertexData->vertexStart = 0;
    mRenderOp.vertexData->vertexCount = vertexCount;
    mRenderOp.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST; 
    mRenderOp.useIndexes = false; 

    Ogre::VertexDeclaration  *decl = mRenderOp.vertexData->vertexDeclaration;
    Ogre::VertexBufferBinding   *bind = mRenderOp.vertexData->vertexBufferBinding;
    size_t offset = 0;

    if (!decl->findElementBySemantic(Ogre::VES_POSITION))
        decl->addElement(POS_TEX_BINDING, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);

    offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

    if (!decl->findElementBySemantic(Ogre::VES_TEXTURE_COORDINATES))
        decl->addElement(POS_TEX_BINDING, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 0);

    Ogre::HardwareVertexBufferSharedPtr ptbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(decl->getVertexSize(POS_TEX_BINDING),
        mRenderOp.vertexData->vertexCount,
        Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
    bind->setBinding(POS_TEX_BINDING, ptbuf);

    if (!decl->findElementBySemantic(Ogre::VES_DIFFUSE))
        decl->addElement(COLOUR_BINDING, 0, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);

    Ogre::HardwareVertexBufferSharedPtr cbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(decl->getVertexSize(COLOUR_BINDING),
        mRenderOp.vertexData->vertexCount,
        Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
    bind->setBinding(COLOUR_BINDING, cbuf);

    size_t charlen = mCaption.size();
    float *pPCBuff = static_cast<float*>(ptbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));

    float largestWidth = 0;
    float left = 0 * 2.0 - 1.0;
    float top = -((0 * 2.0) - 1.0);

    Ogre::Real spaceWidth = mSpaceWidth;
    if (spaceWidth == 0)
        spaceWidth = mpFont->getGlyphAspectRatio('A') * mCharHeight * 2.0f;

    Ogre::Vector3 min, max, currPos;
    Ogre::Real maxSquaredRadius;
    bool first = true;

    Ogre::String::iterator i, iend;
    iend = mCaption.end();
    bool newLine = true;
    Ogre::Real len = 0.0f;

    Ogre::Real verticalOffset = 0;
    switch (mVerticalAlignment)
    {
    case OgreMovableText::V_ABOVE:
        verticalOffset = mCharHeight;
        break;
    case OgreMovableText::V_CENTER:
        verticalOffset = 0.5f*mCharHeight;
        break;
    case OgreMovableText::V_BELOW:
        verticalOffset = 0;
        break;
    }
    top += verticalOffset;
    for (i = mCaption.begin(); i != iend; ++i)
    {
        if (*i == '\n')
            top += verticalOffset * 2.0f;
    }

    for (i = mCaption.begin(); i != iend; ++i)
    {
        if (newLine)
        {
            len = 0.0f;
            for (Ogre::String::iterator j = i; j != iend && *j != '\n'; j++)
            {
                if (*j == ' ')
                    len += spaceWidth;
                else 
                    len += mpFont->getGlyphAspectRatio((unsigned char)*j) * mCharHeight * 2.0f;
            }
            newLine = false;
        }

        if (*i == '\n')
        {
            left = 0 * 2.0 - 1.0;
            top -= mCharHeight * 2.0f;
            newLine = true;
            continue;
        }

        if (*i == ' ')
        {
            left += spaceWidth;
            mRenderOp.vertexData->vertexCount -= 6;
            continue;
        }

        Ogre::Real horiz_height = mpFont->getGlyphAspectRatio((unsigned char)*i);
        Ogre::Real u1, u2, v1, v2; 
        Ogre::Font::UVRect utmp;
        utmp = mpFont->getGlyphTexCoords((unsigned char)*i);
        u1 = utmp.left;
        u2 = utmp.right;
        v1 = utmp.top;
        v2 = utmp.bottom;

        if(mHorizontalAlignment == OgreMovableText::H_LEFT)
            *pPCBuff++ = left;
        else
            *pPCBuff++ = left - (len / 2);
        *pPCBuff++ = top;
        *pPCBuff++ = -1.0;
        *pPCBuff++ = u1;
        *pPCBuff++ = v1;

        if(mHorizontalAlignment == OgreMovableText::H_LEFT)
            currPos = Ogre::Vector3(left, top, -1.0);
        else
            currPos = Ogre::Vector3(left - (len / 2), top, -1.0);
        if (first)
        {
            min = max = currPos;
            maxSquaredRadius = currPos.squaredLength();
            first = false;
        }
        else
        {
            min.makeFloor(currPos);
            max.makeCeil(currPos);
            maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());
        }

        top -= mCharHeight * 2.0f;

        if(mHorizontalAlignment == OgreMovableText::H_LEFT)
            *pPCBuff++ = left;
        else
            *pPCBuff++ = left - (len / 2);
        *pPCBuff++ = top;
        *pPCBuff++ = -1.0;
        *pPCBuff++ = u1;
        *pPCBuff++ = v2;

        if(mHorizontalAlignment == OgreMovableText::H_LEFT)
            currPos = Ogre::Vector3(left, top, -1.0);
        else
            currPos = Ogre::Vector3(left - (len / 2), top, -1.0);
        min.makeFloor(currPos);
        max.makeCeil(currPos);
        maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

        top += mCharHeight * 2.0f;
        left += horiz_height * mCharHeight * 2.0f;

        if(mHorizontalAlignment == OgreMovableText::H_LEFT)
            *pPCBuff++ = left;
        else
            *pPCBuff++ = left - (len / 2);
        *pPCBuff++ = top;
        *pPCBuff++ = -1.0;
        *pPCBuff++ = u2;
        *pPCBuff++ = v1;

        if(mHorizontalAlignment == OgreMovableText::H_LEFT)
            currPos = Ogre::Vector3(left, top, -1.0);
        else
            currPos = Ogre::Vector3(left - (len / 2), top, -1.0);
        min.makeFloor(currPos);
        max.makeCeil(currPos);
        maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

        if(mHorizontalAlignment == OgreMovableText::H_LEFT)
            *pPCBuff++ = left;
        else
            *pPCBuff++ = left - (len / 2);
        *pPCBuff++ = top;
        *pPCBuff++ = -1.0;
        *pPCBuff++ = u2;
        *pPCBuff++ = v1;

        currPos = Ogre::Vector3(left, top, -1.0);
        min.makeFloor(currPos);
        max.makeCeil(currPos);
        maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

        top -= mCharHeight * 2.0f;
        left -= horiz_height  * mCharHeight * 2.0f;

        if(mHorizontalAlignment == OgreMovableText::H_LEFT)
            *pPCBuff++ = left;
        else
            *pPCBuff++ = left - (len / 2);
        *pPCBuff++ = top;
        *pPCBuff++ = -1.0;
        *pPCBuff++ = u1;
        *pPCBuff++ = v2;

        currPos = Ogre::Vector3(left, top, -1.0);
        min.makeFloor(currPos);
        max.makeCeil(currPos);
        maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

        left += horiz_height  * mCharHeight * 2.0f;

        if(mHorizontalAlignment == OgreMovableText::H_LEFT)
            *pPCBuff++ = left;
        else
            *pPCBuff++ = left - (len / 2);
        *pPCBuff++ = top;
        *pPCBuff++ = -1.0;
        *pPCBuff++ = u2;
        *pPCBuff++ = v2;

        currPos = Ogre::Vector3(left, top, -1.0);
        min.makeFloor(currPos);
        max.makeCeil(currPos);
        maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

        top += mCharHeight * 2.0f;

        float currentWidth = (left + 1)/2 - 0;
        if (currentWidth > largestWidth)
            largestWidth = currentWidth;
    }
    ptbuf->unlock();

    mAABB = Ogre::AxisAlignedBox(min, max);
    mRadius = Ogre::Math::Sqrt(maxSquaredRadius);

    if (mUpdateColors)
        this->_updateColors();

    mNeedUpdate = false;
}

void Ape::OgreMovableText::_updateColors(void)
{
    assert(mpFont);
    assert(!mpMaterial.isNull());

    Ogre::RGBA color;
    Ogre::Root::getSingleton().convertColourValue(mColor, &color);
    Ogre::HardwareVertexBufferSharedPtr vbuf = mRenderOp.vertexData->vertexBufferBinding->getBuffer(COLOUR_BINDING);
    Ogre::RGBA *pDest = static_cast<Ogre::RGBA*>(vbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));
    for (int i = 0; i < (int)mRenderOp.vertexData->vertexCount; ++i)
        *pDest++ = color;
    vbuf->unlock();
    mUpdateColors = false;
}

const Ogre::Quaternion& Ape::OgreMovableText::getWorldOrientation(void) const
{
    assert(mpCam);
    return const_cast<Ogre::Quaternion&>(mpCam->getDerivedOrientation());
}

void Ape::OgreMovableText::visitRenderables(Ogre::Renderable::Visitor* visitor, 
        bool debugRenderables)
{
}

const Ogre::Vector3& Ape::OgreMovableText::getWorldPosition(void) const
{
    assert(mParentNode);
    return mParentNode->_getDerivedPosition();
}

void Ape::OgreMovableText::getWorldTransforms(Ogre::Matrix4 *xform) const 
{
    if (this->isVisible() && mpCam)
    {
        Ogre::Matrix3 rot3x3, scale3x3 = Ogre::Matrix3::IDENTITY;
		mpCam->getDerivedOrientation().ToRotationMatrix(rot3x3);
        Ogre::Vector3 ppos = mParentNode->_getDerivedPosition() + (mParentNode->_getDerivedOrientation() * (mParentNode->_getDerivedScale() * mLocalTranslation)); //+ Vector3::UNIT_Y * mGlobalTranslation;

        // apply scale
        //scale3x3[0][0] = mParentNode->_getDerivedScale().x / 2;
        //scale3x3[1][1] = mParentNode->_getDerivedScale().y / 2;
        //scale3x3[2][2] = mParentNode->_getDerivedScale().z / 2;

        // apply all transforms to xform       
        //*xform = (rot3x3 * scale3x3);
		*xform = (rot3x3);
        xform->setTrans(ppos);
		
    }
}

void Ape::OgreMovableText::getRenderOperation(Ogre::RenderOperation &op)
{
    if (this->isVisible())
    {
        if (mNeedUpdate)
            this->_setupGeometry();
        if (mUpdateColors)
            this->_updateColors();
        op = mRenderOp;
    }
}

void Ape::OgreMovableText::_notifyCurrentCamera(Ogre::Camera *cam)
{
    mpCam = cam;
}

void Ape::OgreMovableText::_updateRenderQueue(Ogre::RenderQueue* queue)
{
    if (this->isVisible())
    {
        if (mNeedUpdate)
            this->_setupGeometry();
        if (mUpdateColors)
            this->_updateColors();

		queue->addRenderable(this, mRenderQueueID, OGRE_RENDERABLE_DEFAULT_PRIORITY);
    }
}


Ape::OgreMovableTextFactory::OgreMovableTextFactory()
{
	mTypeName = "MovableText";
}

Ape::OgreMovableTextFactory::~OgreMovableTextFactory()
{

}

const Ogre::String& Ape::OgreMovableTextFactory::getType( void ) const
{
	return mTypeName;
}

Ogre::MovableObject* Ape::OgreMovableTextFactory::createInstance( const Ogre::String& name, Ogre::SceneManager* manager, const Ogre::NameValuePairList* params /*= 0*/ )
{
	return new Ape::OgreMovableText(name, name);
}

void Ape::OgreMovableTextFactory::destroyInstance( Ogre::MovableObject* obj )
{

}

Ogre::MovableObject* Ape::OgreMovableTextFactory::createInstanceImpl( const Ogre::String& name, const Ogre::NameValuePairList* params /*= 0*/ )
{
	return NULL;
}

std::string Ape::OgreMovableText::getMaterialName()
{
	return mpMaterial->getName();
}