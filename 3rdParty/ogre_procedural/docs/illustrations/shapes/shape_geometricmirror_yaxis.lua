s = Procedural.Shape():addPoint(0, 0):addPoint(-0.5, -1.0):addPoint(-0.75, 1):addPoint(0, 0.5)
s:mirrorAroundAxis(Procedural.Vector2_UNIT_Y)
s:close()
tests:addShape(s)