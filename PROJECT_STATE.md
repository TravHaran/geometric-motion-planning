# Geometric Motion Planning — Project State

This document is the living engineering record for the
`geometric-motion-planning` project.

It records the current development state, architectural decisions,
implementation conventions, known limitations, and upcoming milestones.

Update this document whenever a significant milestone, architectural
decision, or project-wide convention changes.

---

# 1. Project Goal

The long-term goal of this project is to investigate the research question:

> Can computational geometry provide extremely cheap approximations of
> expensive humanoid dynamic feasibility?

The project begins as a computational geometry and classical motion-planning
codebase and gradually evolves toward humanoid footstep planning and
reduced-order dynamic feasibility.

The development philosophy is:

**Theory → Implementation → Tests → Visualization → Experiment**

The project intentionally implements important geometry and planning
algorithms directly where practical rather than hiding the underlying
concepts behind large external geometry libraries.

---

# 2. Development Roadmap

## Phase 1 — Computational Geometry Foundations

Status: **Complete**

Implemented:

- Point representation
- Segment representation
- Polygon representation
- Path representation
- Orientation predicate
- Point-on-segment testing
- Segment intersection
- Proper segment intersection
- Polygon edge generation
- Point-in-polygon testing
- Segment-polygon intersection
- Segment collision checking
- Path collision checking
- Visibility-specific geometric predicates

Visualization demos:

- `demo_1a` — Segment intersection
- `demo_1b` — Point-in-polygon
- `demo_1c` — Path collision

---

## Phase 2 — Classical 2D Motion Planning

Status: **In Progress**

Completed:

- Visibility graph construction
- Euclidean graph edge weights
- Breadth-First Search (BFS)
- Dijkstra shortest-path search
- A* shortest-path search
- Euclidean A* heuristic
- Expanded-node instrumentation
- Interactive environment construction
- Interactive planner visualization
- Dijkstra vs A* comparison
- PATHLAB interactive planning visualizer
- BFS, Dijkstra, and A* selection and comparison in PATHLAB
- Data-driven PATHLAB planner selector with indexed UI selection and focused
  selector regression tests
- Architecture-aligned CMake libraries for geometry, graph, planners,
  visualization, and UI, with centralized CTest registration
- Visibility regression coverage for segments that enter obstacle interiors
  through a vertex and terminate on another boundary edge
- Cached and batched PATHLAB scene rendering
- Event-driven, frame-limited PATHLAB application loop
- Search playback with current-expansion and goal-reached highlighting in a
  contextual floating canvas dock
- Canvas camera with cursor-centered zoom, Option-drag panning, and a
  screen-space reset control
- Cohesive rounded UI controls, vector icons, hover/pressed feedback, and
  responsive sidebar spacing
- Collapsible planner sidebar with sidebar-aware canvas, camera controls, and
  playback-dock layout
- Shared quarter-resolution separable Gaussian backdrop blur for selected
  PATHLAB glass surfaces, with event-driven cache invalidation and a
  shader-unavailable fallback
- Contextual screen-space Help / Controls overlay with modal input blocking
- Contextual control availability and truthful planner-status feedback
- Deterministic four-obstacle showcase scene for demos and planner comparison

Current demos:

- `demo_2a` — Visibility graph
- `demo_2b` — Dijkstra on a visibility graph
- `demo_2c` — Interactive Dijkstra planning sandbox
- `demo_2d` — Interactive Dijkstra / A* comparison
- `demo_2e` — PATHLAB interactive BFS / Dijkstra / A* planning visualizer

Next major milestone:

- Voronoi / Generalized Voronoi Diagram
- Maximum-clearance planning
- Comparison between shortest-path and clearance-oriented planning

---

## Phase 3 — Configuration-Space Geometry

Status: **Not Started**

Planned topics:

- Robot footprints
- Configuration space
- Minkowski sums
- Obstacle inflation
- Translational configuration spaces
- SE(2) configuration spaces
- Orientation-dependent collision geometry

---

## Phase 4 — Sampling-Based Motion Planning

Status: **Not Started**

Planned algorithms:

- PRM
- RRT
- RRT*

---

## Phase 5 — Humanoid Footstep Planning

Status: **Not Started**

Planned topics:

- Left/right foot states
- Footstep state representation
- Geometric step reachability
- Footstep A*
- Collision-aware foot placement
- Terrain constraints

---

## Phase 6 — Reduced-Order Humanoid Dynamics

Status: **Not Started**

Planned models/concepts:

- LIPM
- ZMP
- Capture Point
- DCM
- ALIP

---

## Phase 7 — Dynamic Feasibility Ground Truth

Status: **Not Started**

Goal:

Develop a higher-fidelity dynamic feasibility checker that can determine
whether candidate humanoid transitions are dynamically feasible.

This checker will serve as ground truth for later approximation experiments.

---

## Phase 8 — Feasibility Dataset Generation

Status: **Not Started**

Goal:

Generate a large dataset of candidate humanoid transitions labeled using
the dynamic feasibility checker.

---

## Phase 9 — Geometry of Dynamically Feasible Sets

Status: **Not Started**

Potential representations:

- Axis-aligned boxes
- Ellipsoids
- Convex hulls
- Convex decompositions
- Polytopes
- Signed distance fields

Goal:

Study the geometry and topology of dynamically feasible transition sets.

---

## Phase 10 — Geometric Dynamic-Feasibility Approximation

Status: **Not Started**

Goal:

Construct conservative and extremely cheap geometric approximations of
dynamic feasibility and evaluate them inside humanoid planning.

Important evaluation criteria will eventually include:

- Computational cost
- False-positive rate
- False-negative rate
- Conservativeness
- Planning success rate
- Planning speed
- Approximation complexity

---

# 3. Current Repository Structure

```text
geometric-motion-planning/
├── README.md
├── PROJECT_STATE.md
├── CMakeLists.txt
├── run.sh
├── test.sh
├── theory.sh
├── assets/
│   ├── fonts/
│   └── shaders/
├── docs/
│   └── theory.md
├── theory/
│   ├── theory.tex
│   ├── sections/
│   └── figures/
├── src/
│   ├── geometry/
│   ├── graph/
│   ├── planners/
│   ├── visualization/
│   ├── ui/
│   └── app/
├── demos/
├── tests/
├── build/
└── build-release/
```
