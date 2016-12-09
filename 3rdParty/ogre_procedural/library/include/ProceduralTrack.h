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
#ifndef PROCEDURAL_TRACK_INCLUDED
#define PROCEDURAL_TRACK_INCLUDED

#include "ProceduralPlatform.h"
#include "OgreMath.h"

namespace Procedural
{
/**
 * Represents a curve by interpolating between a list of key/values.
 * It always refers to a "principal" curve (a path, atm), so the keys to either its point index or lineic position.
 */
class _ProceduralExport Track
{
public:
	/// Defines addressing mode for the track
	/// ABSOLUTE_LINEIC : use the distance from the start of the principal curve
	/// RELATIVE_LINEIC : use the relative distance from the start of the principal curve, considering the total length of main curve is 1.
	/// POINT : right on the principal curve's key
	enum AddressingMode
	{
	    AM_ABSOLUTE_LINEIC, AM_RELATIVE_LINEIC, AM_POINT
	};
protected:
	/// Adressing mode of the track (see the enum definition for more details)
	AddressingMode mAddressingMode;

	/// Tells whether we should add new points to principal curve if a key is defined here but not on principal curve
	bool mInsertPoint;

	/// Key frames
	std::map<Ogre::Real, Ogre::Real> mKeyFrames;
public:
	/// Default constructor.
	/// Point insertion default to true, and addressing to relative lineic
	Track(AddressingMode addressingMode=AM_RELATIVE_LINEIC, bool insertPoint=true) : mAddressingMode(addressingMode), mInsertPoint(insertPoint) {}

	/// Gets addressing mode of the curve
	AddressingMode getAddressingMode() const
	{
		return mAddressingMode;
	}

	/// Inserts a new Key/Value couple anywhere on the track (it is auto-sorted anyway)
	inline Track& addKeyFrame(Ogre::Real pos, Ogre::Real value)
	{
		mKeyFrames[pos] = value;
		return *this;
	}

	/// @copydoc Track::mInsertPoint
	inline bool isInsertPoint() const
	{
		return mInsertPoint;
	}

	/// Gets the value on the current point, taking into account the addressing mode
	Ogre::Real getValue(Ogre::Real absPos, Ogre::Real relPos, unsigned int index) const;

	/// Gets the value on the current point
	Ogre::Real getValue(Ogre::Real pos) const;

	/// Get the key value couple before current point, taking into account addressing mode.
	/// If current point is below minimum key, issues minimum key
	std::map<Ogre::Real, Ogre::Real>::const_iterator _getKeyValueBefore(Ogre::Real absPos, Ogre::Real relPos, unsigned int index) const;

	/// Get the key value couple before current point.
	/// If current point is below minimum key, issues minimum key/value
	std::map<Ogre::Real, Ogre::Real>::const_iterator _getKeyValueBefore(Ogre::Real pos) const;

	/// Get the key value couple after current point, taking into account addressing mode.
	/// If current point is above maximum key, issues maximum key/value
	std::map<Ogre::Real, Ogre::Real>::const_iterator _getKeyValueAfter(Ogre::Real absPos, Ogre::Real relPos, unsigned int index) const;

	/// Get the key value couple after current point.
	/// If current point is above maximum key, issues maximum key/value
	std::map<Ogre::Real, Ogre::Real>::const_iterator _getKeyValueAfter(Ogre::Real pos) const;

	/// Gets the first value in the track
	Ogre::Real getFirstValue() const
	{
		return mKeyFrames.begin()->second;
	}

	/// Gets the last value in the track
	Ogre::Real getLastValue() const
	{
		return (--mKeyFrames.end())->second;
	}
};
//---------------------------------------------------


}
#endif
