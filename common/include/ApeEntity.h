/*MIT License

Copyright (c) 2018 MTA SZTAKI

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

#ifndef APE_ENTITY_H
#define APE_ENTITY_H

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Ape
{
	class Entity
	{
	public:
		enum Type
		{
			LIGHT,
			CAMERA,
			GEOMETRY_FILE,
			GEOMETRY_INDEXEDFACESET,
			GEOMETRY_INDEXEDLINESET,
			GEOMETRY_TEXT,
			GEOMETRY_BOX,
			GEOMETRY_PLANE,
			GEOMETRY_TUBE,
			GEOMETRY_CYLINDER,
			GEOMETRY_SPHERE,
			GEOMETRY_TORUS,
			GEOMETRY_CONE,
			GEOMETRY_RAY,
			MATERIAL_MANUAL,
			MATERIAL_FILE,
			PASS_PBS,
			PASS_MANUAL,
			TEXTURE_MANUAL,
			TEXTURE_UNIT,
			BROWSER,
			WATER,
			SKY,
			POINT_CLOUD,
			INVALID
		};

	protected:
		Entity(std::string name, Type type) : mName(name), mType(type) {};

		virtual ~Entity() {};

		std::string mName;

		Type mType;

	public:
		std::string getName()
		{
			return mName;
		};

		Type getType()
		{
			return mType;
		};
	};
}

#endif
