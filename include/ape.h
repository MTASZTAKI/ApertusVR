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

#ifndef APE_H
#define APE_H

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace ape
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

	class IPlaneGeometry;

	typedef std::weak_ptr<IPlaneGeometry> PlaneGeometryWeakPtr;

	typedef std::shared_ptr<IPlaneGeometry> PlaneGeometrySharedPtr;

	class IFileGeometry;

	typedef std::weak_ptr<IFileGeometry> FileGeometryWeakPtr;

	typedef std::shared_ptr<IFileGeometry> FileGeometrySharedPtr;

	class IFileTexture;

	typedef std::weak_ptr<IFileTexture> FileTextureWeakPtr;

	typedef std::shared_ptr<IFileTexture> FileTextureSharedPtr;


	class IIndexedFaceSetGeometry;

	typedef std::weak_ptr<IIndexedFaceSetGeometry> IndexedFaceSetGeometryWeakPtr;

	typedef std::shared_ptr<IIndexedFaceSetGeometry> IndexedFaceSetGeometrySharedPtr;

	class IIndexedLineSetGeometry;

	typedef std::weak_ptr<IIndexedLineSetGeometry> IndexedLineSetGeometryWeakPtr;

	typedef std::shared_ptr<IIndexedLineSetGeometry> IndexedLineSetGeometrySharedPtr;

	class ICloneGeometry;

	typedef std::weak_ptr<ICloneGeometry> CloneGeometryWeakPtr;

	typedef std::shared_ptr<ICloneGeometry> CloneGeometrySharedPtr;

	class IManualMaterial;

	typedef std::weak_ptr<IManualMaterial> ManualMaterialWeakPtr;

	typedef std::shared_ptr<IManualMaterial> ManualMaterialSharedPtr;

	class SubGeometry;

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

	class IWater;

	typedef std::weak_ptr<IWater> WaterWeakPtr;

	typedef std::shared_ptr<IWater> WaterSharedPtr;

	class ISky;

	typedef std::weak_ptr<ISky> SkyWeakPtr;

	typedef std::shared_ptr<ISky> SkySharedPtr;

	class IPointCloud;

	typedef std::weak_ptr<IPointCloud> PointCloudWeakPtr;

	typedef std::shared_ptr<IPointCloud> PointCloudSharedPtr;

	class IRigidBody;

	typedef std::weak_ptr<IRigidBody> RigidBodyWeakPtr;

	typedef std::shared_ptr<IRigidBody> RigidBodySharedPtr;
}

#endif
