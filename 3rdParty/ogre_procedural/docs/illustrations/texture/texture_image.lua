buffer = Procedural.TextureBuffer(128)
Procedural.Image(buffer):setFile("red_brick.jpg"):process()
tests:addTextureBuffer(buffer)
