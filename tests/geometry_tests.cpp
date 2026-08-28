#include <cassert>
#include <iostream>

#include "geometry/Geometry.hpp"


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

    Point inside{2.0, 1.5};
    Point outside{5.0, 1.5};

    Point onEdge{4.0, 1.5};
    Point onVertex{4.0, 3.0};

    assert(pointInPolygon(inside, rectangle));
    assert(!pointInPolygon(outside, rectangle));

    assert(pointInPolygon(onEdge, rectangle));
    assert(pointInPolygon(onVertex, rectangle));
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
}


int main() {
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
    std::cout << "All geometry tests passed!\n";

    return 0;
}