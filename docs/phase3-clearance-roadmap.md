# Phase 3.1 — Obstacle-clearance geometry (incremental track)

This branch intentionally diverges from the experimental `feature/clearance-geometry` proof-of-concept. Start from `main`, keep `demo_2e` as the baseline, and build geometric feasibility before footstep planning.

## Completed here: integrated scrollable sidebar (frontend only)

Run `./run.sh demo_2e`, then choose **Load Demo**.

- The **Run Planner** button now stays fixed at the **top** of the sidebar.
- Scroll the sidebar with the mouse wheel or trackpad. Scrolling over the sidebar no longer zooms the world canvas.
- Click any section header to expand/collapse it. Available sections: Planner, Geometric Feasibility, Visualization, Scene, Results, Performance, Kinematic Reachability (future), and LIPM Dynamics (future).
- The algorithm selector remains in the Planner section; its dropdown expands inline and participates in scrolling.
- The **Geometric Feasibility** section contains the clearance preview toggle and radius slider (0–120 PATHLAB world units). Drag the thumb or click the track to adjust it.
- The existing keyboard shortcuts remain: **C** toggles the preview; **[** and **]** decrease/increase the radius by 5.
- Geometry, visualization, and result state are preserved when a section is collapsed.
- This overlay is **not yet enforced** by BFS, Dijkstra, or A*: it only shows translucent bands surrounding polygonal obstacles. The shortest-path algorithms are unchanged.

UI source: `src/ui/PathlabSidebar.cpp` and `src/ui/PathlabSidebar.hpp` contain the shared layout, section geometry, clipping, and hit-testing. `src/app/PathlabApp.cpp` holds UI state and wheel/slider events. Regression tests exercise scrolling, collapsible sections, run-button placement, and algorithm selection.

Because world coordinates in `demo_2e` are display-scaled values rather than physical metres, don't interpret 20 world units as 20 metres. Decide and document an explicit world-to-metre scale before implementing the biped.

## Student-owned backend implementation order

### Step 1. Distance between a point and a segment
Write `pointSegmentDistance(Point p, Segment edge)` in `src/geometry/` (place it in a distinct `Clearance` module, not in the UI).
Contract: return the minimum Euclidean distance from p to the closed segment. Address zero-length segments. Test an interior projection, projection before/after endpoints, and a point exactly on the segment.

### Step 2. Distance between a segment and a polygon
Write `segmentPolygonDistance(Segment, Polygon)`. A segment entirely inside the polygon has distance zero, as do intersections/touches; otherwise find minimum distance among the segment and polygon boundary. Support both convex and simple concave polygons. Test near-vertex, near-edge, tangent, fully-contained, and completely disjoint inputs.

### Step 3. Clearance predicate
For robot-body radius r>=0, return collision-free only if every point on a candidate route edge maintains at least r distance from each obstacle, with an explicitly chosen touch policy. We recommend treating contact as **blocked**. Validate negative or non-finite radii.

### Step 4. Re-plan, don't merely reject
Implement a planning method that can reroute around clearance-expanded obstacles. An A* search on a grid with *continuous edge-clearance checks* is an acceptable intermediate baseline, but its shortest-path claim is only with respect to its discretized graph. A mathematically shortest path among disk-inflated polygonal obstacles involves line/arc offset geometry and deserves separate evaluation.

### Step 5. Integrate and compare
Connect the radius control to the new backend and re-plan when the value changes. Display clearance-violating edges and failures truthfully. Compare baseline and clearance-aware routes under identical start, goal, and obstacle layouts. Include tests and search/runtime metrics.

## Definition of done for geometry
- A path, when returned, meets its specified clearance at every point on each piecewise-linear segment, including corners.
- The goal is rejected with a meaningful status if within a forbidden clearance region.
- The planner can find a detour where one exists within its search representation.
- No path is claimed when there is no solution; no unsafe result is relabeled as feasible.
- Exact versus approximate offset geometry and the corresponding optimality guarantees are clearly distinguished.
- Default PATHLAB planners stay available as baselines.

## Future phases (not implemented on this branch)
1. Kinematic foothold polygon: support-foot pose, heading, side, step width/length; later posture/IK reachability and swing-trajectory collision.
2. LIPM-based dynamic feasibility: CoM position/velocity, support schedule and CoP trajectory, step duration, terminal viability.
3. Research experiments on state-dependent feasible foothold-region geometry, accelerated intersection/query, conservative inner/outer approximations.

The end-to-end exploratory prototype remains untouched for reference in its separate draft PR.
