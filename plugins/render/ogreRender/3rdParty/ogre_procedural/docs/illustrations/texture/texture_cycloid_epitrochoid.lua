buffer = Procedural.TextureBuffer(128)
Procedural.Solid(buffer):setColour(Procedural.ColourValue(0.0, 0.5, 1.0, 1.0)):process()
Procedural.Cycloid(buffer):setColour(Procedural.ColourValue_Red):setPenSize(2):setType(Procedural.Cycloid_EPITROCHOID):process()
tests:addTextureBuffer(buffer)
