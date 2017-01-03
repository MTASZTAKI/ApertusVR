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
			NODE,
			LIGHT,
			GEOMETRY,
			MATERIAL,
			TEXTURE,
			LABEL,
			CAMERA,
			PASS,
			EG_INVALID
		};

		enum Type : unsigned int
		{
			NODE_CREATE = (NODE << 8) + 1,
			NODE_DELETE,
			NODE_POSITION,
			NODE_ORIENTATION,
			NODE_SCALE,
			NODE_PARENTNODE,
			LIGHT_CREATE = (LIGHT << 8) + 1,
			LIGHT_DELETE,
			LIGHT_TYPE,
			LIGHT_DIFFUSE,
			LIGHT_SPECULAR,
			LIGHT_ATTENUATION,
			LIGHT_DIRECTION,
			LIGHT_SPOTRANGE,
			LIGHT_PARENTNODE,
			GEOMETRY_FILE_CREATE = (GEOMETRY << 8) + 1,
			GEOMETRY_FILE_DELETE,
			GEOMETRY_FILE_FILENAME,
			GEOMETRY_FILE_PARENTNODE,
			GEOMETRY_MANUAL_CREATE,
			GEOMETRY_MANUAL_PARAMETER,
			GEOMETRY_MANUAL_PARENTNODE,
			GEOMETRY_MANUAL_DELETE,
			GEOMETRY_TEXT_CREATE,
			GEOMETRY_TEXT_DELETE,
			GEOMETRY_TEXT_VISIBLE,
			GEOMETRY_TEXT_CAPTION,
			GEOMETRY_TEXT_OFFSET,
			GEOMETRY_TEXT_PARENTNODE,
			GEOMETRY_PRIMITVE_CREATE,
			GEOMETRY_PRIMITVE_PARAMETERS,
			GEOMETRY_PRIMITVE_MATERIAL,
			GEOMETRY_PRIMITVE_PARENTNODE,
			GEOMETRY_PRIMITVE_DELETE,
			MATERIAL_MANUAL_CREATE = (MATERIAL << 8) + 1,
			MATERIAL_MANUAL_AMBIENT,
			MATERIAL_MANUAL_DIFFUSE,
			MATERIAL_MANUAL_SPECULAR,
			MATERIAL_MANUAL_EMISSIVE,
			MATERIAL_MANUAL_SHININESS,
			MATERIAL_MANUAL_PASS,
			MATERIAL_MANUAL_DELETE,
			MATERIAL_FILE_CREATE,
			MATERIAL_FILE_SETASSKYBOX,
			MATERIAL_FILE_DELETE,
			MATERIAL_FILE_FILENAME,
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
			CAMERA_INITPOSITIONOFFSET,
			CAMERA_INITORIENTATIONOFFSET,
			CAMERA_PARENTNODE,
			PASS_CREATE = (PASS << 8) + 1,
			PASS_DELETE,
			PASS_ALBEDO,
			PASS_ROUGHNESS,
			PASS_LIGHTROUGHNESSOFFSET,
			PASS_F0,
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
