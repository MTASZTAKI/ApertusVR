s = Procedural.Shape():addPoint(0,-3):addPoint(1,-3):addPoint(1,0):addPoint(.8,1):addPoint(.8,2):addPoint(1.5,3):addPoint(0,4)
tb = Procedural.Lathe():setShapeToExtrude(s):setAngleBegin(Procedural.Radian(2)):setAngleEnd(Procedural.Radian(0)):buildTriangleBuffer()
tests:addTriangleBuffer(tb)