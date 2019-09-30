tb = Procedural.BoxGenerator():buildTriangleBuffer()
Procedural.CalculateNormalsModifier():setInputTriangleBuffer(tb):setComputeMode(Procedural.CalculateNormalsModifier_NCM_VERTEX):modify()
--Procedural.WeldVerticesModifier():setInputTriangleBuffer(tb):modify()
tests:addTriangleBuffer(tb)