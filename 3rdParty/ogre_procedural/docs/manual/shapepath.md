Shapes, paths and tracks {#shapepath}
========================

[TOC]

In Ogre %Procedural, shapes, paths and tracks all are made of line connected points.

The difference between them is :

* Tracks are 1D
* Shapes are 2D
* Paths are 3D


## Orientation and closedness

Both shapes, tracks and paths can be closed or not : if closed, there will be an automatic junction between the last point and the first point.

Shapes have an outside and an inside : you can define whether the left or the right side is the outside.
It also makes sense with non-closed shape (for example, when extruded, the outside corresponds to where the face normal heads).
There's also a function to guess where is the outside and where is the inside.

## Splines

Splines are a few helper classes used to generate shapes or paths, by interpolating between a bunch of control points.

TIP: You can also generate a track by first generating a shape, then calling the method convertToTrack(). 
      Shape's X are used as keys and Y as values.

### Cubic Hermite Spline

This is often referred as 'Bezier Spline'.
With Cubic Hermite Spline, you can choose the points and the tangents of the curves that goes through control points.

![Figure 1. Cubic Hermite Spline](spline_cubichermite.png)

~~~~~~~~~~~~~~~~
CubicHermiteSpline2().addPoint(Vector2(0,0), AT_CATMULL)
                     .addPoint(Vector2(1,0), AT_CATMULL)
                     .addPoint(Vector2(1,1), Vector2(0,2), Vector2(0,-2))
                     .addPoint(Vector2(2,1), AT_CATMULL)
                     .addPoint(2,0).addPoint(3,0)
                     .addPoint(3,1).addPoint(4,1).setNumSeg(16).realizeShape()
~~~~~~~~~~~~~~~~

You can either define the tangents of your choice, as demonstrated for the 3rd point, or auto-generate them as Catmull-Rom spline, or even straight lines.

See Procedural::CubicHermiteSpline2 for a shape and Procedural::CubicHermiteSpline3 for a path.

### Catmull-Rom Spline

It's a particular case of Cubic Hermite Spline, in which tangents are automatically calculated.
Note that its the equivalent of _Ogre::SimpleSpline_, and there's even a conversion function between the two.

![Figure 2. Catmull-Rom Spline](spline_catmull.png)

~~~~~~~~~~~~~~~~
CatmullRomSpline2().addPoint(0,0).addPoint(1,0)
                   .addPoint(1,1).addPoint(2,1)
                   .addPoint(2,0).addPoint(3,0)
                   .addPoint(3,1).addPoint(4,1).realizeShape()
~~~~~~~~~~~~~~~~

See Procedural::CatmullRomSpline2 for a shape and Procedural::CatmullRomSpline3 for a path.

### Kochanek Bartels Spline

Kochanek-Bartels spline is defined by control points and 3 parameters : tension, bias and continuity.

Here's a description of what they do :

Parameter  | +1              | -1
-----------|-----------------|------------
Tension    |Tight            |Round
Bias       |Post Shoot       |Pre shoot
Continuity |Inverted corners |Box corners


![Figure 3. Kochanek Bartels Spline](spline_kochanekbartels.png)

See Procedural::KochanekBartelsSpline2 for a shape.

### Rounded Corner Spline

This one consists in straight lines joining the control points, with corners replaced by circle arcs.

![Figure 4. Rounded Corner Spline](spline_roundedcorner.png)

See Procedural::RoundedCornerSpline2 for a shape and Procedural::RoundedCornerSpline3 for a path.

### Bezier Curve

Splines and bezier curves are a few helper classes used to generate shapes or paths, by interpolating between a bunch of control points.

The bezier curve is not going through all control points.

![Figure 5. Bezier Curve](spline_beziercurve.png)

See Procedural::BezierCurve2 for a shape and Procedural::BezierCurve3 for a path.

## Track specifics

Tracks are used to represent a variable that varies along a path or a shape.

For that reason, the keys of the track are defined relatively to the points in the main curve.

There are 3 different addressing modes :

* AM_ABSOLUTE_LINEIC : the key represents a distance from the beginning of the curve.
* AM_RELATIVE_LINEIC : the key represents a relative distance inside the [0;1] segment, 0 being the beginning and 1 the end of the curve.
* AM_POINT : the key represents the index of a point in the main curve.

~~~~~~~~~~~~~~~~
Track t = Track(Track::AM_RELATIVE_LINEIC).addKeyFrame(0,0).addKeyFrame(1.0,-1.0);
~~~~~~~~~~~~~~~~

## Shape specifics

### Primitive transformations

You can do some standard transformations on you shapes.

As an exemple, let's say we have this shape:

![Figure 6. Random shape for geometry transformation](shape_geometricsetup.png)

~~~~~~~~~~~~~~~~
Shape s = Shape().addPoint(0.0f, 0.0f).addPoint(-0.5f, -1.0f).addPoint(-0.75f, 1.0f).addPoint(0.0f, 0.5f);
~~~~~~~~~~~~~~~~

- Translation

![Figure 7. Translation](shape_geometrictranslate.png)

~~~~~~~~~~~~~~~~
s.translate(1, 2);
~~~~~~~~~~~~~~~~

- Scale

![Figure 8. Scale](shape_geometricscale.png)

~~~~~~~~~~~~~~~~
s.scale(2.0f, 2.0f);
~~~~~~~~~~~~~~~~

- Rotation

![Figure 9. Rotation](shape_geometricrotate.png)

~~~~~~~~~~~~~~~~
s.rotate(Ogre::Degree(45));
~~~~~~~~~~~~~~~~

- Mirror

![Figure 10a. Mirror at a point](shape_geometricmirror_point.png)

~~~~~~~~~~~~~~~~
s.mirror(0.5f, 0.5f);
~~~~~~~~~~~~~~~~

![Figure 10b. Mirror at y axis](shape_geometricmirror_yaxis.png)

~~~~~~~~~~~~~~~~
s.mirror(Shape::MIRROR_Y_AXIS);
~~~~~~~~~~~~~~~~

### 2D CSG

You can combine shapes together in order to produce new shapes, using boolean operations.
Technically, the output is a multishape, because it doesn't always resolve to a single line.

As an exemple, let's say we have these 2 shapes :

![Figure 11. Random shape for geometry transformation](shape_booleansetup.png)

~~~~~~~~~~~~~~~~
Shape s1 = RectangleShape().realizeShape();
Shape s2 = s1;
s2.translate(.5f,.5f);
~~~~~~~~~~~~~~~~

Supported boolean operations are :

* Union : the result contains everything inside A plus evertyhing inside B

![Figure 12. Union](shape_booleanunion.png)

~~~~~~~~~~~~~~~~
s1.booleanUnion(s2)
~~~~~~~~~~~~~~~~

* Intersection : the result contains everything that is inside A and B

![Figure 13. Intersection](shape_booleanintersection.png)

~~~~~~~~~~~~~~~~
s1.booleanIntersection(s2)
~~~~~~~~~~~~~~~~

* Difference : the result contains everything that is in A but not in B

![Figure 14. Difference](shape_booleandifference.png)

~~~~~~~~~~~~~~~~
s1.booleanDifference(s2)
~~~~~~~~~~~~~~~~

### Thicken

A "thin" shape can be made "thick" by using the thicken operation.

#### before 
![Figure 15a. before thicken](shape_thick1.png) 

~~~~~~~~~~~~~~~~
Shape s;
s.addPoint(-1,-1).addPoint(0.5,0).addPoint(-0.5,0).addPoint(1,1)
~~~~~~~~~~~~~~~~

#### after 
![Figure 15b. after thicken](shape_thick2.png)

~~~~~~~~~~~~~~~~
s.thicken(.2f)
~~~~~~~~~~~~~~~~

## Triangulation

A constrained triangulation can be performed on a shape or a multishape. It means that points from the shape(s) are joined together to form triangles.

The algorithm used is Bowyer-Watson, which is an implementation of a Delaunay triangulation (Delaunay simply means that triangulation is 'best quality', ie has as few thin triangles as possible)

The main use of triangulation in OgreProcedural is just for extrusion tips, but you can use it for your own purposes.

![Figure 16. Triangulation](shape_triangulation.png)

## Shape Primitives

There are a couple of pre-existing shape primitives. After creation it's possible to receive a Shape object by calling realizeShape()

### CircleShape

![Figure 17. CircleShape](shape_circle.png)

See Procedural::CircleShape

### EllipseShape

![Figure 18. EllipseShape](shape_ellipse.png)

See Procedural::EllipseShape

### RectangleShape

![Figure 19. RectangleShape](shape_rectangle.png)

See Procedural::RectangleShape

### TriangleShape

![Figure 20. TriangleShape](shape_triangle.png)

See Procedural::TriangleShape

## MultiShapes

A MultiShape is a collection of normal shapes.

### TextShape

Creates shapes from characters of a given text (FreeType required).

![Figure 21. TextShape](shape_text.png)

See Procedural::TextShape

## Path primitives

At the moment there is only one path primitive.
After creation it's possible to receive a Path object by calling realizePath().

### HelixPath

![Figure 22. HelixPath](spline_helix.png)

See Procedural::HelixPath

## SVG

It is really easy to import SVG files and extrude their shapes:

~~~~~~~~~~~~~~~~
Path p;
MultiShape out;
SvgLoader svg;
// Create extrusion path
p.addPoint(0, 0, 0);
p.addPoint(0, 50, 0);
// Load svg file
svg.parseSvgFile(out, "test.svg", "Essential", 16);
// Extrude all shapes of file
Extruder().setMultiShapeToExtrude(&out).setExtrusionPath(&p).setScale(.07).realizeMesh("svg");
~~~~~~~~~~~~~~~~

### Example

The Sample_SVG project demonstrates how to load various shapes from a SVG file and extrude them.

Here is the code to build it :

\include SVG.cpp