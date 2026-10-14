#include "OffsetWaypoints.hpp"

#include "Clearance.hpp"
#include "Geometry.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace {
constexpr double PI = 3.14159265358979323846264338327950288;
constexpr double MAX_ARC_STEP = PI / 12.0; // At most 15 degrees

double signedAreaTwice(const Polygon& polygon){
    double sum = 0.0;
    const auto& v = polygon.vertices;
    for(std::size_t i = 0; i < v.size(); ++i){
        const Point& a = v[i];
        const Point& b = v[(i + 1) % v.size()];
        sum += a.x * b.y - b.x * a.y;
    }
    return sum;
}

// The right-hand normal is outward on counterclockwise polygon edges.
// For clockwise winding, use the left-hand normal instead.
Point outwardNormal(const Point& a, const Point& b, double winding){
    const double dx = b.x - a.x;
    const double dy = b.y - a.y;
    const double len = std::hypot(dx, dy);
    if(len == 0.0)return {0.0,0.0};
    return winding > 0.0 ? Point{dy / len, -dx / len}
                         : Point{-dy / len, dx / len};
}

bool finitePoint(const Point& p){
    return std::isfinite(p.x) && std::isfinite(p.y);
}
void addIfSafe(std::vector<Point>& output, const Point& p,
               const std::vector<Polygon>& obstacles, double radius){
    // The zero-length segment means checking clearance at a single point.
    // The check is against the UNION, not only the vertex's source obstacle.
    if(finitePoint(p) && segmentHasClearance({p,p}, obstacles, radius)){
        output.push_back(p);
    }
}
}

std::vector<Point> generateClearanceWaypoints(
    const std::vector<Polygon>& obstacles, double radius
){
    if(!std::isfinite(radius) || radius < 0.0){
        throw std::invalid_argument("Waypoints require a finite nonnegative radius");
    }
    if(radius == 0.0)return {};

    // The expansion beyond r accommodates the strict contact policy.
    // Dividing by cos(step/2) puts straight chords connecting consecutive
    // samples outside the radius-r disk around a convex corner.
    // Clearance against OTHER obstacle features is validated separately.
    const double epsilon = std::max(1e-4, radius * 1e-5);
    const double inflatedRadius = radius + epsilon;

    std::vector<Point> candidates;
    for(const Polygon& polygon : obstacles){
        const auto& vertices = polygon.vertices;
        if(vertices.size() < 3)continue;
        const double area = signedAreaTwice(polygon);
        if(!std::isfinite(area) || std::abs(area) < 1e-12)continue;
        const double winding = area > 0.0 ? 1.0 : -1.0;

        for(std::size_t i = 0; i < vertices.size(); ++i){
            const Point& prev = vertices[(i + vertices.size() - 1) % vertices.size()];
            const Point& v = vertices[i];
            const Point& next = vertices[(i + 1) % vertices.size()];
            const double prevLen = std::hypot(v.x-prev.x, v.y-prev.y);
            const double nextLen = std::hypot(next.x-v.x, next.y-v.y);
            if(prevLen == 0.0 || nextLen == 0.0)continue;

            const Point nPrev = outwardNormal(prev, v, winding);
            const Point nNext = outwardNormal(v, next, winding);
            const double normalizedTurn =
                winding * orientation(prev,v,next) / (prevLen * nextLen);

            if(normalizedTurn > 1e-12){
                // Convex corner: the disk offset contains a circular arc
                // swept from the incoming edge normal to the outgoing one.
                const double initial = std::atan2(nPrev.y,nPrev.x);
                double final = std::atan2(nNext.y,nNext.x);
                if(winding > 0.0){
                    while(final < initial)final += 2.0 * PI;
                }else{
                    while(final > initial)final -= 2.0 * PI;
                }
                const double sweep = final - initial;
                const int intervals = std::max(
                    1, static_cast<int>(std::ceil(std::abs(sweep) / MAX_ARC_STEP))
                );
                const double step = sweep / static_cast<double>(intervals);
                const double sampleRadius =
                    inflatedRadius / std::cos(0.5 * std::abs(step));

                for(int j = 0; j <= intervals; ++j){
                    const double angle = initial + static_cast<double>(j) * step;
                    const Point p{
                        v.x + sampleRadius * std::cos(angle),
                        v.y + sampleRadius * std::sin(angle)
                    };
                    addIfSafe(candidates, p, obstacles, radius);
                }
            }else if(normalizedTurn < -1e-12){
                // Reflex corner: adjacent outward-shifted supporting lines
                // intersect on the free-space side of this vertex.
                //
                // M = V + (r + epsilon)(nPrev + nNext)/(1 + dot),
                // since dot(nPrev, M - V) == dot(nNext, M - V) == r + eps.
                const double dot = nPrev.x*nNext.x + nPrev.y*nNext.y;
                if(1.0 + dot < 1e-10)continue; // near-opposite normals
                const Point miter{
                    v.x + inflatedRadius * (nPrev.x+nNext.x) / (1.0+dot),
                    v.y + inflatedRadius * (nPrev.y+nNext.y) / (1.0+dot)
                };
                addIfSafe(candidates, miter, obstacles, radius);
            }
            // Collinear points contribute no distinct offset corner.
        }
    }
    return candidates;
}
