s = Procedural.Shape():addPoint(-1,-1):addPoint(0.5,0):addPoint(-0.5,0):addPoint(1,1)
s = s:thicken(.2):getShape(0)
tests:addShape(s)