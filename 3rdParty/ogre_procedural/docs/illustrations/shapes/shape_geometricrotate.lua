s = Procedural.Shape():addPoint(0, 0):addPoint(-0.5, -1.0):addPoint(-0.75, 1):addPoint(0, 0.5)
s:rotate(Procedural.Radian(Procedural.Math_HALF_PI/2))
tests:addShape(s)