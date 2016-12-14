buffer = Procedural.TextureBuffer(128)
Procedural.Noise(buffer):setType(Procedural.Noise_NOISE_WHITE):process()
tests:addTextureBuffer(buffer)
