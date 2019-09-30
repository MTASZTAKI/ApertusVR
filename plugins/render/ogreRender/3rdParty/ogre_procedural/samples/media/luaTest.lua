tb = Procedural.TextureBuffer(1024)
Procedural.Cell(tb):setRegularity(233):setDensity(10):process()
mesh = Procedural.SphereGenerator():setNumRings(8):setRadius(4.0):buildTriangleBuffer()
tests:addTriangleTextureBuffer(mesh, tb)