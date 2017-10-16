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

#ifndef APE_H
#define APE_H

#include <vector>
#include <string>
#include <map>
#include <memory>

namespace Ape
{	
	class INode;

	typedef std::shared_ptr<INode> NodeSharedPtr;

	typedef std::weak_ptr<INode> NodeWeakPtr;

	typedef std::vector<NodeSharedPtr> NodeSharedPtrVector;

	typedef std::vector<NodeWeakPtr> NodeWeakPtrVector;

	typedef std::map<std::string, NodeWeakPtr> NodeWeakPtrNameMap;

	typedef std::map<std::string, NodeSharedPtr> NodeSharedPtrNameMap;

	class ILight;

	typedef std::weak_ptr<ILight> LightWeakPtr;

	class Entity;

	typedef std::shared_ptr<Entity> EntitySharedPtr;

	typedef std::weak_ptr<Entity> EntityWeakPtr;

	typedef std::vector<EntitySharedPtr> EntitySharedPtrVector;

	typedef std::vector<EntityWeakPtr> EntityWeakPtrVector;

	typedef std::map<std::string, EntityWeakPtr> EntityWeakPtrNameMap;

	typedef std::map<std::string, EntitySharedPtr> EntitySharedPtrNameMap;

	class Geometry;

	typedef std::weak_ptr<Geometry> GeometryWeakPtr;

	typedef std::shared_ptr<Geometry> GeometrySharedPtr;

	class ITextGeometry;

	typedef std::weak_ptr<ITextGeometry> TextGeometryWeakPtr;

	typedef std::shared_ptr<ITextGeometry> TextGeometrySharedPtr;

	class IBoxGeometry;

	typedef std::weak_ptr<IBoxGeometry> BoxGeometryWeakPtr;

	typedef std::shared_ptr<IBoxGeometry> BoxGeometrySharedPtr;

	class IFileGeometry;

	typedef std::weak_ptr<IFileGeometry> FileGeometryWeakPtr;

	typedef std::shared_ptr<IFileGeometry> FileGeometrySharedPtr;

	class IIndexedFaceSetGeometry;

	typedef std::weak_ptr<IIndexedFaceSetGeometry> IndexedFaceSetGeometryWeakPtr;

	typedef std::shared_ptr<IIndexedFaceSetGeometry> IndexedFaceSetGeometrySharedPtr;

	class IIndexedLineSetGeometry;

	typedef std::weak_ptr<IIndexedLineSetGeometry> IndexedLineSetGeometryWeakPtr;

	typedef std::shared_ptr<IIndexedLineSetGeometry> IndexedLineSetGeometrySharedPtr;

	class IManualMaterial;

	typedef std::weak_ptr<IManualMaterial> ManualMaterialWeakPtr;

	typedef std::shared_ptr<IManualMaterial> ManualMaterialSharedPtr;

	class SubGeometry;

	class Pass;

	typedef std::weak_ptr<Pass> PassWeakPtr;

	typedef std::shared_ptr<Pass> PassSharedPtr;

	class IPbsPass;

	typedef std::weak_ptr<IPbsPass> PbsPassWeakPtr;

	typedef std::shared_ptr<IPbsPass> PbsPassSharedPtr;

	class IManualPass;

	typedef std::weak_ptr<IManualPass> ManualPassWeakPtr;

	typedef std::shared_ptr<IManualPass> ManualPassSharedPtr;

	class Material;

	typedef std::weak_ptr<Material> MaterialWeakPtr;

	typedef std::shared_ptr<Material> MaterialSharedPtr;

	class Texture;

	typedef std::weak_ptr<Texture> TextureWeakPtr;

	typedef std::shared_ptr<Texture> TextureSharedPtr;

	class IManualTexture;

	typedef std::weak_ptr<IManualTexture> ManualTextureWeakPtr;

	typedef std::shared_ptr<IManualTexture> ManualTextureSharedPtr;

	class IBrowser;

	typedef std::weak_ptr<IBrowser> BrowserWeakPtr;

	typedef std::shared_ptr<IBrowser> BrowserSharedPtr;

	class IUnitTexture;

	typedef std::weak_ptr<IUnitTexture> UnitTextureWeakPtr;

	typedef std::shared_ptr<IUnitTexture> UnitTextureSharedPtr;

	class IRayGeometry;

	typedef std::weak_ptr<IRayGeometry> RayGeometryWeakPtr;

	typedef std::shared_ptr<IRayGeometry> RayGeometrySharedPtr;
}

#endif
