s = Procedural.Shape():addPoint(0, 0):addPoint(-0.5, -1.0):addPoint(-0.75, 1):addPoint(0, 0.5)
s:mirror(0.5, 0.5)
tests:addShape(s)