#pragma once
#include "../geometry/Point.hpp"
#include "../geometry/Polygon.hpp"
#include <cstddef>
#include <vector>

namespace footstep {
enum class Layer { Geometry = 1, Kinematic = 2, Dynamic = 3 };

struct Settings {
    double clearance = 0.27;       // body clearance in metres
    double maxStep = 0.82;         // landing foot separation bound
    double stepWidth = 0.24;       // nominal lateral change between feet
    double stepTime = 0.38;        // seconds, fixed single-support interval
    double comHeight = 0.90;       // metres
    double gridSize = 0.20;
    int maxExpansions = 30000;
};
struct Scene {
    double width = 16.0;
    double height = 10.0;
    Point start{1.2, 4.95};
    Point goal{14.45, 4.95};
    std::vector<Polygon> obstacles;
};
struct State {
    Point support{}, other{}, com{}, velocity{};
    double heading = 0.0;
    int nextSide = 1;      // +1 = next landing to the left of heading
    int parent = -1;
    double g = 0.0;
    int depth = 0;
};
struct Result {
    Layer layer = Layer::Geometry;
    bool reachedGoal = false;
    std::size_t expanded = 0, rejectedGeometry = 0, rejectedKinematics = 0, rejectedDynamics = 0;
    std::vector<Point> geometricPath;
    std::vector<State> footsteps; // includes initial support pose
};
// Exact zero clearance = no polygon contact; positive clearance has strict boundary.
// Paths are computed on a finite lattice, so they are not globally shortest continuous paths.
Scene sampleScene(bool obstructed = true);
Result solve(const Scene& scene, const Settings& settings, Layer layer);
// Closed-form LIPM assuming constant CoP at support throughout t.
State propagateLIPM(const State& s, double t, double comHeight);
Point capturePoint(const State& s, double comHeight);
}
