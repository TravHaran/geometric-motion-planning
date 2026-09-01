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
- Dijkstra shortest-path search
- A* shortest-path search
- Euclidean A* heuristic
- Expanded-node instrumentation
- Interactive environment construction
- Interactive planner visualization
- Dijkstra vs A* comparison
- PATHLAB interactive planning visualizer
- Cached and batched PATHLAB scene rendering
- Event-driven, frame-limited PATHLAB application loop
- Search playback with current-expansion and goal-reached highlighting
- Canvas camera with cursor-centered zoom and Option-drag panning

Current demos:

- `demo_2a` — Visibility graph
- `demo_2b` — Dijkstra on a visibility graph
- `demo_2c` — Interactive Dijkstra planning sandbox
- `demo_2d` — Interactive Dijkstra / A* comparison
- `demo_2e` — PATHLAB interactive planning visualizer

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
│   └── fonts/
├── theory/
│   ├── theory.tex
│   ├── sections/
│   └── figures/
├── src/
│   ├── geometry/
│   ├── environment/
│   ├── graph/
│   ├── planners/
│   ├── visualization/
│   └── experiments/
├── demos/
├── tests/
└── build/
