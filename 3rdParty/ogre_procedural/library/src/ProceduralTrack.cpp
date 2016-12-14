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
#include "ProceduralTrack.h"

using namespace Ogre;

namespace Procedural
{
Real Track::getValue(Real pos) const
{
	std::map<Real, Real>::const_iterator itAfter = _getKeyValueAfter(pos);
	std::map<Real, Real>::const_iterator itBefore = _getKeyValueBefore(pos);

	if (itAfter==itBefore)
		return itBefore->second;
	if (itAfter==mKeyFrames.begin())
		return itAfter->second;

	Real x1 = itBefore->first;
	Real y1 = itBefore->second;
	Real x2 = itAfter->first;
	Real y2 = itAfter->second;
	return (pos-x1)/(x2-x1)*(y2-y1)+y1;
}

std::map<Real, Real>::const_iterator Track::_getKeyValueBefore(Real pos) const
{
	std::map<Real, Real>::const_iterator it = mKeyFrames.upper_bound(pos);
	if (it==mKeyFrames.begin())
		return it;
	else
		return --it;
}

std::map<Real, Real>::const_iterator Track::_getKeyValueAfter(Real pos) const
{
	std::map<Real, Real>::const_iterator it = mKeyFrames.upper_bound(pos);
	if (it==mKeyFrames.end())
		return --it;
	else
		return it;
}

Real Track::getValue(Real absPos, Real relPos, unsigned int index) const
{
	if (mAddressingMode == AM_ABSOLUTE_LINEIC)
		return getValue(absPos);
	if (mAddressingMode == AM_RELATIVE_LINEIC)
		return getValue(relPos);
	return getValue((Real)index);
}

std::map<Real, Real>::const_iterator Track::_getKeyValueBefore(Real absPos, Real relPos, unsigned int index) const
{
	if (mAddressingMode == AM_ABSOLUTE_LINEIC)
		return _getKeyValueBefore(absPos);
	if (mAddressingMode == AM_RELATIVE_LINEIC)
		return _getKeyValueBefore(relPos);
	return _getKeyValueBefore((Real)index);
}

std::map<Real, Real>::const_iterator Track::_getKeyValueAfter(Real absPos, Real relPos, unsigned int index) const
{
	if (mAddressingMode == AM_ABSOLUTE_LINEIC)
		return _getKeyValueAfter(absPos);
	if (mAddressingMode == AM_RELATIVE_LINEIC)
		return _getKeyValueAfter(relPos);
	return _getKeyValueAfter((Real)index);
}

}
