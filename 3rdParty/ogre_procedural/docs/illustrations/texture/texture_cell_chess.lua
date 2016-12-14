buffer = Procedural.TextureBuffer(128)
Procedural.Cell(buffer):setDensity(4):setMode(Procedural.Cell_MODE_CHESSBOARD):setPattern(Procedural.Cell_PATTERN_CONE):process()
tests:addTextureBuffer(buffer)
