# H1
### Orientation
for 3 points
`A = (x1, y1)
B = (x2, y2)
C = (x3, y3)`

compute 
`orientation(A, B, C) = (x2-x1)(y3-y1) - (y2-y1)(x3-x1)`

this is the 2D crosa product of B-A and C-A
if orientation is
 \> 0 ; counterclockwise/left
 < 0 ; clockwise/right
 = 0 ; collinear

for two segments AB and CD compute

o1 = orient(A, B, C)
o2 = orient(A, B, D)
o3 = orient(C, D, A)
o4 = orient(C, D, B)

they intersect when:
o1o2 < 0 
and
o3o4 < 0

C and D must lie on opposite sides of line AB, while A and B must simultaneously lie on opposite sides of line CD.

### On Segment

Suppose A = (0, 0), B = (10, 0), P = (5, 0)

computing orientation gives us: 
`orientation(A,B,P)=0`
so A, B, P are collinear

but consider P(15, 0)
we still get orientation(A,B,P) = 0, even though P is not on AB
so orientation only tells us that P lies on the same infinite line as AB. It doesn't tell us whether P actually lies between A and B.
So to check if a point P is on segment AB, we need:
min(Ax, Bx) <= Px <= max(Ax, Bx)
and
min(Ay, By) <= Py <= max(Ay, By)

assuming that the three points are collinear