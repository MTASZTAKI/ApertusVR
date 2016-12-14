s = Procedural.CatmullRomSpline2():addPoint(0,0):addPoint(1,0):addPoint(1,1):addPoint(2,1):addPoint(2,0):addPoint(3,0):addPoint(3,1):addPoint(4,1):realizeShape():translate(-2, 0)
tests:addShape(s)