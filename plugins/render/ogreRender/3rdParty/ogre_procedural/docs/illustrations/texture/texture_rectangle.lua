buffer = Procedural.TextureBuffer(128)

Procedural.Solid(buffer):setColour(Procedural.ColourValue(0.0, 0.5, 1.0, 1.0)):process()
Procedural.RectangleTexture(buffer):setColour(Procedural.ColourValue_Red):setRectangle(0.25, 0.25, 0.75, 0.75):process()
tests:addTextureBuffer(buffer)
dotfile = tests:getDotFile("texture_24", "Rectangle_Demo")
dotfile:set("Solid", "texture_solid", "Rectangle", "texture_rectangle")
