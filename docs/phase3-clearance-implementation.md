# Phase 3.1: obstacle clearance — implementation notes

## What this branch delivers

The **radius** in the Geometric Feasibility sidebar is now the minimum Euclidean
distance that each planned straight path segment must maintain from the original,
**filled** polygon obstacles. This models a circular point-robot footprint.
The preview checkbox controls **rendering only**, not whether clearance is enforced.
A radius change invalidates the previously computed graph and path; press
**Run Planner** again to build and search with the new radius.

A radius of **zero** deliberately delegates to the original visibility graph,
preserving its historic behavior (which allows some obstacle-boundary contact).
For **positive** radius, boundary contact at distance exactly `r` is rejected.

## Algorithms and architecture

1. `Clearance.cpp`: point-to-closed-segment projection distance;
   segment-to-segment distance; segment-to-*filled polygon* distance;
   `segmentHasClearance` over **all** obstacles.
2. `OffsetWaypoints.cpp`: candidate configurations around obstacles.
   Convex vertices get discretized circular-offset arc samples. Reflex corners
   get outward-offset-line intersection candidates. Each candidate is checked
   against the original obstacle union using `segmentHasClearance({p,p},...)`.
3. `ClearanceVisibilityGraph.cpp`: for radius > 0, creates graph nodes at
   start, goal, and safe candidate configurations, then tests *each entire*
   candidate straight edge via `segmentHasClearance`. Unsafe edges never enter
   the graph. The original `VisibilityGraph.cpp` is unchanged.
4. `PathlabApp.cpp`: routes planning through this graph builder and clears
   stale route/search playback when the slider value changes.
5. BFS, Dijkstra, and A* implementations have **not changed**. BFS optimizes
   edge count; Dijkstra/A* optimize graph-path distance under their existing
   assumptions.

### Deliberate design decisions

- No new geometry dependency or external polygon clipping library.
- The **safety validator is applied to the original obstacles**, not a raster,
  preview mesh, or polygon approximation.
- The graph implicitly handles the union of multiple expanded obstacles by
  validating every node and edge against all source polygons.
- Offset-waypoint samples are **not** assembled into expanded polygons and
  intersected/unioned. This avoids the complexity and failure modes of
  polygon-union preprocessing, but is **not the exact Minkowski-sum
  polygonization** contemplated in the original design outline.
- The waypoint graph is a finite approximation. It can return *no route*
  when one exists, or a route longer than the continuous shortest path;
  narrower passage scenarios are especially sensitive to candidate density.
  No optimality or completeness guarantee is claimed.
- The segment-clearance computations currently reuse PATHLAB's epsilon-based
  segment-intersection and point-in-polygon tests. Floating-point edge cases
  such as nearly tangential or very high-magnitude coordinates are **not**
  formally certified. Before using the simulator for robot safety or
  hardware control, use robust predicates and independent trajectory checking.
- Existing UI clearance preview is an illustrative disk/strip rendering,
  not the actual conservative computational obstacle boundary.

## Test plan

Build and run on macOS:

```bash
git fetch origin
git switch feature/geometric-clearance
git pull --ff-only origin feature/geometric-clearance
./run.sh demo_2e
```

The GitHub macOS CI workflow builds all demo and test targets and runs CTest.
To run just the clearance tests locally:

```bash
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --target clearance_tests offset_waypoint_tests clearance_graph_tests
ctest --test-dir build-release -R '^(clearance|offset_waypoints|clearance_graph)$' --output-on-failure
```

Manually check the following in `demo_2e`:

1. With clearance **0**, existing routes behave as in `main`.
2. Put a square between start and goal, set radius **20**, and run
   Dijkstra/A*: the result should detour around the amber clearance envelope.
3. Increase radius to **60**: prior path disappears immediately; hit
   Run Planner again to compute the revised path.
4. Uncheck **Show clearance preview** and run: the planned route must still
   obey the selected radius.
5. Place start/goal within the clearance band: planner reports no path.
6. Create two obstacles with overlapping clearance envelopes: routes must
   avoid their combined forbidden zone.
7. Compare BFS, Dijkstra, and A*: all use exactly the same clearance graph.
   Their *chosen* paths can differ because BFS minimizes hop count.
