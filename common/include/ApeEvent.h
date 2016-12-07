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

#ifndef APE_EVENT_H
#define APE_EVENT_H

#include <string>

namespace Ape
{
	struct Event
	{
	public:
		enum Group : unsigned int
		{
			SCENEPROPERTY,
			NODE,
			LIGHT,
			GEOMETRY,
			MATERIAL,
			TEXTURE,
			LABEL,
			CAMERA,
			EG_INVALID
		};

		enum Type : unsigned int
		{
			SCENEPROPERTY_AMBIENTCOLOR = (SCENEPROPERTY << 8) + 1,
			SCENEPROPERTY_ADDRESOURCEFOLDER,
			SCENEPROPERTY_SKYBOXMATERIAL,
			NODE_CREATE = (NODE << 8) + 1,
			NODE_DELETE,
			NODE_POSITION,
			NODE_ORIENTATION,
			NODE_SCALE,
			NODE_PARENT,
			LIGHT_CREATE = (LIGHT << 8) + 1,
			LIGHT_DELETE,
			LIGHT_TYPE,
			LIGHT_DIFFUSE,
			LIGHT_SPECULAR,
			LIGHT_ATTENUATION,
			LIGHT_DIRECTION,
			LIGHT_SPOTRANGE,
			GEOMETRY_FILE_CREATE = (GEOMETRY << 8) + 1,
			GEOMETRY_FILE_DELETE,
			GEOMETRY_FILE_FILENAME,
			GEOMETRY_MANUAL_CREATE,
			GEOMETRY_MANUAL_INDICES,
			GEOMETRY_MANUAL_VERTICES,
			GEOMETRY_MANUAL_TEXTURECOORDS,
			GEOMETRY_MANUAL_NORMALS,
			GEOMETRY_MANUAL_TRIANGLES,
			GEOMETRY_MANUAL_DELETE,
			GEOMETRY_TEXT_CREATE,
			GEOMETRY_TEXT_DELETE,
			GEOMETRY_TEXT_VISIBLE,
			GEOMETRY_TEXT_CAPTION,
			GEOMETRY_TEXT_OFFSET,
			GEOMETRY_PRIMITVE_BOX_CREATE,
			GEOMETRY_PRIMITVE_BOX_DIMENSIONS,
			GEOMETRY_PRIMITVE_BOX_DELETE,
			MATERIAL_FILE_CREATE = (MATERIAL << 8) + 1,
			MATERIAL_FILE_DELETE,
			MATERIAL_FILE_FILENAME,
			MATERIAL_MANUAL_CREATE,
			MATERIAL_MANUAL_AMBIENT,
			MATERIAL_MANUAL_DIFFUSE,
			MATERIAL_MANUAL_SPECULAR,
			MATERIAL_MANUAL_SHININESS,
			MATERIAL_MANUAL_TEXTURES,
			MATERIAL_MANUAL_DELETE,
			TEXTURE_CREATE = (TEXTURE << 8) + 1,
			TEXTURE_FILENAME,
			TEXTURE_DELETE,
			CAMERA_CREATE = (CAMERA << 8) + 1,
			CAMERA_DELETE,
			CAMERA_FOCALLENGTH,
			CAMERA_ASPECTRATIO,
			CAMERA_FOVY,
			CAMERA_FRUSTUMOFFSET,
			CAMERA_NEARCLIP,
			CAMERA_FARCLIP,
			CAMERA_POSITIONOFFSET,
			CAMERA_ORIENTATIONOFFSET,
			ET_INVALID
		};
		
		std::string subjectName;

		Event::Type type;

		Event::Group group;

		Event()
		{
			subjectName = "NONE";
			type = Event::Type::ET_INVALID;
			group = Event::Group::EG_INVALID;
		}

		Event(std::string subjectName, Event::Type type)
		{
			this->subjectName = subjectName;
			this->type = type;
			this->group = ((Event::Group)((this->type & 0xFF00) >> 8));
		}

		bool operator ==(const Event& other) const
		{
			return (subjectName == other.subjectName && type == other.type);
		}
	};
}
#endif
