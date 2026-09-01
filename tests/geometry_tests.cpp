#include <cassert>
#include <iostream>
#include <cmath>

#include "geometry/Geometry.hpp"
#include "../src/geometry/Triangulation.hpp"


void testOrientation() {
    Point A{0.0, 0.0};
    Point B{4.0, 0.0};

    Point left{2.0, 3.0};
    Point right{2.0, -3.0};
    Point collinear{2.0, 0.0};

    assert(orientation(A, B, left) > 0);
    assert(orientation(A, B, right) < 0);
    assert(orientation(A, B, collinear) == 0);
}

void test_onSegment() {
    Point A{0.0, 0.0};
    Point B{10.0, 0.0};

    Point inside{5.0, 0.0};
    Point endpoint{10.0, 0.0};
    Point outside{15.0, 0.0};
    Point notCollinear{5.0, 2.0};

    assert(onSegment(A, B, inside));
    assert(onSegment(A, B, endpoint));
    assert(!onSegment(A, B, outside));
    assert(!onSegment(A, B, notCollinear));
}

void testSegmentIntersect() {
    // proper crossing
    Segment s1{{0, 0}, {10, 10}};
    Segment s2{{0, 10}, {10, 0}};
    assert(segmentsIntersect(s1, s2));

    // parallel, no intersection
    Segment s3{{0, 0}, {10, 0}};
    Segment s4{{0, 5}, {10, 5}};
    assert(!segmentsIntersect(s3, s4));

    // endpoint touching
    Segment s5{{0, 0}, {10, 0}};
    Segment s6{{10, 0}, {10, 10}};
    assert(segmentsIntersect(s5, s6));

    // collinear & overlapping
    Segment s7{{0, 0}, {10, 0}};
    Segment s8{{5, 0}, {15, 0}};
    assert(segmentsIntersect(s7, s8));

    // collinear but separated
    Segment s9{{0, 0}, {5, 0}};
    Segment s10{{10, 0}, {15, 0}};
    assert(!segmentsIntersect(s9, s10));
}

void testPolygonEdges(){
    Polygon rectangle{{
        {0.0, 0.0},
        {4.0, 0.0},
        {4.0, 3.0},
        {0.0, 3.0}
    }};

    std::vector<Segment> edges = polygonEdges(rectangle);

    assert(edges.size() == 4);

    assert(edges[0].a.x == 0.0 && edges[0].a.y == 0.0);
    assert(edges[0].b.x == 4.0 && edges[0].b.y == 0.0);

    assert(edges[3].a.x == 0.0 && edges[3].a.y == 3.0);
    assert(edges[3].b.x == 0.0 && edges[3].b.y == 0.0);
}

void testSegmentIntersectsPolygon(){
    Polygon rectangle{{
        {0.0, 0.0},
        {4.0, 0.0},
        {4.0, 3.0},
        {0.0, 3.0}
    }};

    // segment crossing
    Segment crossing_seg = {{-2.0, 2.0}, {2.0, 2.0}};
    // segment outside
    Segment outside_seg = {{5.0, 2.0}, {6.0, 2.0}};
    // segment inside
    Segment inside_seg = {{2.0, 2.0}, {3.0, 2.0}};

    assert(segmentIntersectsPolygon(crossing_seg, rectangle));
    assert(!segmentIntersectsPolygon(outside_seg, rectangle));
    assert(!segmentIntersectsPolygon(inside_seg, rectangle));
}

void testPointInPolygon() {

    Polygon rectangle{{
        {0.0, 0.0},
        {4.0, 0.0},
        {4.0, 3.0},
        {0.0, 3.0}
    }};

    Polygon clockwiseRectangle{{
        {0.0, 0.0},
        {0.0, 3.0},
        {4.0, 3.0},
        {4.0, 0.0}
    }};

    Point inside{2.0, 1.5};
    Point outside{5.0, 1.5};

    Point onEdge{4.0, 1.5};
    Point onVertex{4.0, 3.0};

    assert(pointInPolygon(inside, rectangle));
    assert(!pointInPolygon(outside, rectangle));

    assert(pointInPolygon(onEdge, rectangle));
    assert(pointInPolygon(onVertex, rectangle));

    assert(pointInPolygon({2.0, 1.0}, clockwiseRectangle));
    assert(!pointInPolygon({5.0, 1.0}, clockwiseRectangle));
}

void testPointInConcavePolygon() {

    Polygon concave{{
        {0.0, 0.0},
        {5.0, 0.0},
        {5.0, 5.0},
        {3.0, 3.0},
        {0.0, 5.0}
    }};

    Point inside{1.0, 2.0};
    Point outsideConcavity{4.0, 4.5};
    Point onConcaveEdge{4.0, 4.0};

    assert(pointInPolygon(inside, concave));
    assert(!pointInPolygon(outsideConcavity, concave));
    assert(pointInPolygon(onConcaveEdge, concave));
}

void testIsSegmentCollisionFree() {

    Polygon rectangle{{
        {0.0, 0.0},
        {4.0, 0.0},
        {4.0, 3.0},
        {0.0, 3.0}
    }};

    std::vector<Polygon> obstacles{rectangle};

    // 1. Completely outside
    Segment freeSeg{{5.0, 1.0}, {8.0, 1.0}};
    assert(isSegmentCollisionFree(freeSeg, obstacles));

    // 2. Crosses obstacle
    Segment crossingSeg{{-2.0, 1.5}, {6.0, 1.5}};
    assert(!isSegmentCollisionFree(crossingSeg, obstacles));

    // 3. Completely inside obstacle
    Segment insideSeg{{1.0, 1.0}, {3.0, 2.0}};
    assert(!isSegmentCollisionFree(insideSeg, obstacles));

    // 4. Starts inside, ends outside
    Segment startsInside{{2.0, 1.0}, {6.0, 1.0}};
    assert(!isSegmentCollisionFree(startsInside, obstacles));

    // 5. Touches polygon edge
    Segment touchesEdge{{-2.0, 0.0}, {2.0, 0.0}};
    assert(!isSegmentCollisionFree(touchesEdge, obstacles));

    // 6. Touches polygon vertex
    Segment touchesVertex{{-2.0, -2.0}, {0.0, 0.0}};
    assert(!isSegmentCollisionFree(touchesVertex, obstacles));

    // 7. No obstacles at all
    std::vector<Polygon> noObstacles;
    Segment anySeg{{0.0, 0.0}, {10.0, 10.0}};
    assert(isSegmentCollisionFree(anySeg, noObstacles));
}

void testMultipleObstacles() {

    Polygon obstacle1{{
        {0.0, 0.0},
        {2.0, 0.0},
        {2.0, 2.0},
        {0.0, 2.0}
    }};

    Polygon obstacle2{{
        {5.0, 0.0},
        {7.0, 0.0},
        {7.0, 2.0},
        {5.0, 2.0}
    }};

    std::vector<Polygon> obstacles{
        obstacle1,
        obstacle2
    };

    // Passes safely between the two obstacles
    Segment freeSeg{{3.0, -1.0}, {3.0, 3.0}};
    assert(isSegmentCollisionFree(freeSeg, obstacles));

    // Hits the second obstacle
    Segment blockedSeg{{3.0, 1.0}, {6.0, 1.0}};
    assert(!isSegmentCollisionFree(blockedSeg, obstacles));
}

void testIsPathCollisionFree() {

    Polygon rectangle{{
        {0.0, 0.0},
        {4.0, 0.0},
        {4.0, 3.0},
        {0.0, 3.0}
    }};

    std::vector<Polygon> obstacles{rectangle};

    // 1. Entire path is outside obstacle
    Path freePath{{
        {-2.0, -1.0},
        {2.0, -1.0},
        {6.0, -1.0}
    }};

    assert(isPathCollisionFree(freePath, obstacles));

    // 2. One middle segment crosses obstacle
    Path crossingPath{{
        {-2.0, 1.0},
        {-1.0, 1.0},
        {5.0, 1.0},
        {6.0, 1.0}
    }};

    assert(!isPathCollisionFree(crossingPath, obstacles));

    // 3. One waypoint lies inside obstacle
    Path waypointInside{{
        {-2.0, 1.0},
        {2.0, 1.0},
        {6.0, 1.0}
    }};

    assert(!isPathCollisionFree(waypointInside, obstacles));

    // 4. Path touches obstacle boundary
    Path touchesObstacle{{
        {-2.0, 0.0},
        {2.0, 0.0},
        {6.0, 0.0}
    }};

    assert(!isPathCollisionFree(touchesObstacle, obstacles));

    // 5. Empty path
    Path emptyPath{{}};
    assert(isPathCollisionFree(emptyPath, obstacles));

    // 6. Single-point path outside obstacle
    Path singlePoint{{{6.0, 6.0}}};
    assert(isPathCollisionFree(singlePoint, obstacles));

    // 7. No obstacles
    std::vector<Polygon> noObstacles;

    Path anyPath{{
        {0.0, 0.0},
        {5.0, 5.0},
        {10.0, 0.0}
    }};

    assert(isPathCollisionFree(anyPath, noObstacles));

    Path singlePointInside{{{2.0, 1.0}}};
    assert(!isPathCollisionFree(singlePointInside, obstacles));
}

void testInvalidPolygonEdges()
{
    Polygon empty{{}};
    Polygon onePoint{{
        {0.0, 0.0}
    }};
    Polygon twoPoints{{
        {0.0, 0.0},
        {1.0, 1.0}
    }};

    assert(polygonEdges(empty).empty());
    assert(polygonEdges(onePoint).empty());
    assert(polygonEdges(twoPoints).empty());
}

void testPointOnPolygonBoundary()
{
    Polygon rectangle{{
        {0.0, 0.0},
        {4.0, 0.0},
        {4.0, 3.0},
        {0.0, 3.0}
    }};

    assert(pointOnPolygonBoundary( //edge
        {2.0, 0.0},
        rectangle
    ));

    assert(pointOnPolygonBoundary( //vertex
        {0.0, 0.0},
        rectangle
    ));

    assert(!pointOnPolygonBoundary( //interior
        {2.0, 1.0},
        rectangle
    ));

    assert(!pointOnPolygonBoundary( //outside
        {5.0, 1.0},
        rectangle
    ));
}

void testPointStrictlyInsidePolygon()
{
    Polygon rectangle{{
        {0.0, 0.0},
        {4.0, 0.0},
        {4.0, 3.0},
        {0.0, 3.0}
    }};

    assert(pointStrictlyInsidePolygon( //interior
        {2.0, 1.0},
        rectangle
    ));

    assert(!pointStrictlyInsidePolygon( //edge
        {2.0, 0.0},
        rectangle
    ));

    assert(!pointStrictlyInsidePolygon( //vertex
        {0.0, 0.0},
        rectangle
    ));

    assert(!pointStrictlyInsidePolygon( //outside
        {5.0, 1.0},
        rectangle
    ));
}

void testSegmentsProperlyIntersect()
{
    Segment crossing1{
        {0.0, 0.0},
        {4.0, 4.0}
    };

    Segment crossing2{
        {0.0, 4.0},
        {4.0, 0.0}
    };

    assert(segmentsProperlyIntersect(
        crossing1,
        crossing2
    ));

    Segment touching1{
        {0.0, 0.0},
        {2.0, 2.0}
    };

    Segment touching2{
        {2.0, 2.0},
        {4.0, 0.0}
    };

    assert(!segmentsProperlyIntersect(
        touching1,
        touching2
    ));

    Segment overlap1{
        {0.0, 0.0},
        {4.0, 0.0}
    };

    Segment overlap2{
        {2.0, 0.0},
        {6.0, 0.0}
    };

    assert(!segmentsProperlyIntersect(
        overlap1,
        overlap2
    ));

    assert(segmentsIntersect(
        touching1,
        touching2
    ));

    assert(segmentsIntersect(
        overlap1,
        overlap2
    ));
}

void testIsVisible()
{
    Polygon rectangle{{
        {2.0, 1.0},
        {6.0, 1.0},
        {6.0, 4.0},
        {2.0, 4.0}
    }};

    std::vector<Polygon> obstacles{
        rectangle
    };

    // Completely outside the obstacle.
    assert(isVisible(
        {0.0, 0.0},
        {1.0, 0.0},
        obstacles
    ));

    // Terminates at an obstacle vertex.
    assert(isVisible(
        {0.0, 1.0},
        {2.0, 1.0},
        obstacles
    ));

    // Travels along an obstacle boundary.
    assert(isVisible(
        {2.0, 1.0},
        {6.0, 1.0},
        obstacles
    ));

    // Diagonal through obstacle interior.
    assert(!isVisible(
        {2.0, 1.0},
        {6.0, 4.0},
        obstacles
    ));

    // Properly crosses the obstacle.
    assert(!isVisible(
        {0.0, 2.0},
        {8.0, 2.0},
        obstacles
    ));

    // Endpoint strictly inside obstacle.
    assert(!isVisible(
        {3.0, 2.0},
        {8.0, 2.0},
        obstacles
    ));

    // Starts at a vertex and immediately enters obstacle interior.
    assert(!isVisible(
        {2.0, 1.0},
        {4.0, 2.0},
        obstacles
    ));

    // Enters through a vertex and ends on another boundary edge.
    // The segment interior lies inside the obstacle even though neither
    // endpoint is strictly inside and neither crossing is proper.
    assert(!isVisible(
        {0.0, 0.0},
        {6.0, 3.0},
        obstacles
    ));
}

void testSegmentParameter()
{
    // Diagonal segment
    Point a{2.0, 3.0};
    Point b{10.0, 7.0};
    Point v{4.0, 4.0};

    assert(
        std::abs(segmentParameter(a, b, v) - 0.25)
        < 1e-9
    );

    // Vertical segment: must use y-coordinate
    Point verticalA{2.0, 3.0};
    Point verticalB{2.0, 10.0};
    Point verticalV{2.0, 5.0};

    assert(
        std::abs(
            segmentParameter(
                verticalA,
                verticalB,
                verticalV
            ) - (2.0 / 7.0)
        ) < 1e-9
    );

    // Horizontal segment: uses x-coordinate
    Point horizontalA{2.0, 5.0};
    Point horizontalB{6.0, 5.0};
    Point horizontalV{5.0, 5.0};

    assert(
        std::abs(
            segmentParameter(
                horizontalA,
                horizontalB,
                horizontalV
            ) - 0.75
        ) < 1e-9
    );

    // Point at the end of the segment
    assert(
        std::abs(
            segmentParameter(a, b, b) - 1.0
        ) < 1e-9
    );

    // Point on the same line, but before A.
    // This demonstrates that the function calculates t;
    // it does not validate segment membership.
    Point beforeA{-2.0, 1.0};

    assert(
        std::abs(
            segmentParameter(a, b, beforeA) - (-0.5)
        ) < 1e-9
    );
}

void testPointAtSegmentParameter()
{
    Point a{2.0, 3.0};
    Point b{10.0, 7.0};

    Point midpoint =
        pointAtSegmentParameter(a, b, 0.5);

    assert(std::abs(midpoint.x - 6.0) < 1e-9);
    assert(std::abs(midpoint.y - 5.0) < 1e-9);

    Point quarter =
        pointAtSegmentParameter(a, b, 0.25);

    assert(std::abs(quarter.x - 4.0) < 1e-9);
    assert(std::abs(quarter.y - 4.0) < 1e-9);

    Point start =
        pointAtSegmentParameter(a, b, 0.0);

    assert(std::abs(start.x - a.x) < 1e-9);
    assert(std::abs(start.y - a.y) < 1e-9);

    Point end =
        pointAtSegmentParameter(a, b, 1.0);

    assert(std::abs(end.x - b.x) < 1e-9);
    assert(std::abs(end.y - b.y) < 1e-9);
}

void testIsVisibleConcavePolygon()
{
    Polygon concave{{
        {2.0, 1.0},
        {6.0, 1.0},
        {6.0, 3.0},
        {4.0, 3.0},
        {4.0, 5.0},
        {2.0, 5.0}
    }};

    std::vector<Polygon> obstacles{
        concave
    };

    // Completely outside the polygon.
    assert(isVisible(
        {0.0, 0.0},
        {1.0, 0.0},
        obstacles
    ));

    // Along a polygon boundary edge.
    assert(isVisible(
        {2.0, 1.0},
        {6.0, 1.0},
        obstacles
    ));

    // Starts at a polygon vertex and immediately enters the interior.
    assert(!isVisible(
        {2.0, 1.0},
        {3.0, 2.0},
        obstacles
    ));

    // Crosses through the obstacle.
    assert(!isVisible(
        {0.0, 2.0},
        {8.0, 2.0},
        obstacles
    ));

    // Passes exactly through the concave reflex vertex.
    // The segment approaches the vertex from outside, then enters
    // the obstacle interior after passing through the vertex.
    assert(!isVisible(
        {6.0, 5.0},
        {2.0, 1.0},
        obstacles
    ));

    // Approaches the reflex vertex entirely from outside and stops there.
    assert(isVisible(
        {6.0, 5.0},
        {4.0, 3.0},
        obstacles
    ));
}

void testTriangulateTriangle()
{
    Polygon polygon{{
        {0.0, 0.0},
        {4.0, 0.0},
        {2.0, 3.0}
    }};


    const std::vector<Triangle> triangles =
        triangulatePolygon(
            polygon
        );


    assert(
        triangles.size() == 1
    );
}


void testTriangulateConvexPolygon()
{
    Polygon polygon{{
        {0.0, 0.0},
        {4.0, 0.0},
        {5.0, 3.0},
        {2.0, 5.0},
        {-1.0, 3.0}
    }};


    const std::vector<Triangle> triangles =
        triangulatePolygon(
            polygon
        );


    assert(
        triangles.size() == 3
    );
}


void testTriangulateConcavePolygon()
{
    Polygon polygon{{
        {0.0, 0.0},
        {5.0, 0.0},
        {5.0, 4.0},
        {3.0, 2.0},
        {0.0, 4.0}
    }};


    const std::vector<Triangle> triangles =
        triangulatePolygon(
            polygon
        );


    assert(
        triangles.size() == 3
    );
}


void testTriangulateClockwisePolygon()
{
    Polygon polygon{{
        {0.0, 4.0},
        {3.0, 2.0},
        {5.0, 4.0},
        {5.0, 0.0},
        {0.0, 0.0}
    }};


    const std::vector<Triangle> triangles =
        triangulatePolygon(
            polygon
        );


    assert(
        triangles.size() == 3
    );
}


int main() {
    //phase 1
    testOrientation();
    test_onSegment();
    testSegmentIntersect();
    testPolygonEdges();
    testSegmentIntersectsPolygon();
    testPointInPolygon();
    testPointInConcavePolygon();
    testIsSegmentCollisionFree();
    testMultipleObstacles();
    testIsPathCollisionFree();
    testInvalidPolygonEdges();

    //phase 2
    testPointOnPolygonBoundary();
    testPointStrictlyInsidePolygon();
    testSegmentsProperlyIntersect();
    testIsVisible();
    testSegmentParameter();
    testPointAtSegmentParameter();
    testIsVisibleConcavePolygon();

    // Triangulation
    testTriangulateTriangle();
    testTriangulateConvexPolygon();
    testTriangulateConcavePolygon();
    testTriangulateClockwisePolygon();
    
    std::cout << "All geometry tests passed!\n";

    return 0;
}
