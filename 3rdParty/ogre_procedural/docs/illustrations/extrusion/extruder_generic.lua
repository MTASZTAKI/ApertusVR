s = Procedural.Shape():addPoint(-1,-1):addPoint(1,-1):addPoint(1,1):addPoint(0,0):addPoint(-1,1):close()
p = Procedural.RoundedCornerSpline3():addPoint(-6,2.5,-2.5):addPoint(-5,0,-2.5):addPoint(0,0,2.5):addPoint(5,0,-2.5):setRadius(1.):realizePath()
tb = Procedural.Extruder():setShapeToExtrude(s):setExtrusionPath(p):buildTriangleBuffer()
tests:addTriangleBuffer(tb)