tb = Procedural.BoxGenerator():setSize(Procedural.Vector3(10,10,10)):buildTriangleBuffer()
Procedural.BoxUVModifier():setInputTriangleBuffer(tb):setBoxSize(Procedural.Vector3(10,10,10)):setMappingType(Procedural.BoxUVModifier_MT_PACKED):modify()
tests:addTriangleBufferTexture(tb,"test.png")