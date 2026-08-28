# Geometric Motion Planning

A C++ research and learning platform for exploring **computational geometry, classical motion planning, and geometric approximations of humanoid dynamic feasibility**.

The project begins with computational geometry algorithms implemented from scratch and incrementally develops them into a complete 2D motion-planning toolkit. The long-term goal is to use the same platform to investigate whether geometric representations can provide computationally inexpensive approximations of more expensive humanoid dynamic-feasibility calculations.

---

## Project Motivation

Motion planning sits at the intersection of robotics, computational geometry, graph algorithms, optimization, and dynamics.

Many robotics libraries provide highly optimized implementations of these algorithms, but using them directly can hide much of the geometry and algorithmic reasoning involved.

This project takes a bottom-up approach.

Instead of beginning with an existing planning framework, the core geometric algorithms and planners are implemented directly in C++. Each stage is unit tested and visualized through an interactive SFML interface.

The project progresses through:

**Computational Geometry → Classical Motion Planning → Configuration Space → Sampling-Based Planning → Humanoid Footstep Planning → Reduced-Order Dynamics → Geometric Dynamic-Feasibility Approximation**

This allows the same codebase to serve both as a learning platform and eventually as an experimental robotics research platform.

---

# Long-Term Research Question

The eventual research question motivating the project is:

> **Can computational geometry provide extremely cheap approximations of expensive humanoid dynamic feasibility?**

Humanoid planners must reason not only about obstacle avoidance but also about whether candidate footsteps and transitions are dynamically feasible.

High-fidelity dynamic checks can be computationally expensive, particularly when thousands or millions of candidate transitions must be evaluated during planning.

The long-term goal of this project is to investigate whether the dynamically feasible transition space has geometric structure that can be approximated using representations such as:

- bounding boxes
- ellipsoids
- convex hulls
- convex decompositions
- polytopes
- signed distance fields

If conservative geometric approximations can reject infeasible transitions extremely quickly while retaining valid ones, they could potentially act as inexpensive filters inside humanoid planning algorithms.

---

# Development Roadmap

The project is intentionally developed incrementally. Every major stage introduces new algorithms, unit tests, experiments, and visualization capabilities.

## Phase 1 — Computational Geometry Foundations

Implement the geometric primitives and predicates required by later planning algorithms.

### Geometry

- [x] 2D points
- [x] line segments
- [x] polygons
- [x] paths
- [x] orientation / 2D cross product
- [x] point-on-segment test
- [x] segment intersection
- [x] polygon edge generation
- [x] segment-polygon boundary intersection
- [x] point-in-polygon using winding numbers
- [x] segment collision checking
- [x] path collision checking

### Visualization

- [x] SFML visualization framework
- [x] interactive segment creation
- [x] segment-intersection visualization
- [ ] interactive polygon construction
- [ ] point-in-polygon visualization
- [ ] path collision visualization

---

## Phase 2 — Classical Geometric Motion Planning

Build complete planners using the geometry kernel.

### Visibility Graph

Construct a graph containing:

- start position
- goal position
- polygon obstacle vertices

Edges connect mutually visible vertices.

Planned components:

- [ ] visibility predicate
- [ ] visibility graph construction
- [ ] Euclidean edge weights
- [ ] graph visualization
- [ ] Dijkstra shortest-path search
- [ ] A* search
- [ ] path reconstruction

The resulting planner will compute shortest collision-free paths through polygonal environments.

### Clearance-Based Planning

Implement planning methods that prefer paths farther from obstacles.

Planned topics include:

- [ ] Voronoi diagrams
- [ ] generalized Voronoi diagrams / medial-axis concepts
- [ ] maximum-clearance paths
- [ ] shortest-path vs clearance tradeoffs

---

## Phase 3 — Configuration Space

The initial planner treats the robot as a point.

Real robots have physical dimensions.

Configuration-space methods will account for robot geometry using techniques such as:

- [ ] robot footprints
- [ ] Minkowski sums
- [ ] configuration-space obstacle construction
- [ ] obstacle inflation
- [ ] collision checking for finite-sized robots
- [ ] SE(2) configuration spaces

This phase will demonstrate the difference between planning for a point robot and planning for a robot with a physical footprint.

---

## Phase 4 — Sampling-Based Motion Planning

Implement and compare widely used sampling-based planners.

- [ ] Probabilistic Roadmap (PRM)
- [ ] Rapidly-exploring Random Tree (RRT)
- [ ] RRT*
- [ ] nearest-neighbor search
- [ ] random configuration sampling
- [ ] local planners
- [ ] path reconstruction

These planners will run in the same environments as the classical geometric planners, allowing direct comparisons.

---

## Phase 5 — Planner Benchmarking

Develop an experimental framework for comparing planning algorithms.

Metrics will include:

- path length
- minimum obstacle clearance
- planning time
- graph size
- nodes expanded
- collision checks
- success rate

Randomized environments will allow planners to be evaluated across many trials rather than isolated examples.

Potential comparisons include:

**Visibility Graph vs Voronoi vs PRM vs RRT vs RRT\***

---

## Phase 6 — Geometric Motion Constraints

Introduce constraints beyond simple collision avoidance.

Possible extensions include:

- [ ] bounded turning angles
- [ ] path smoothness
- [ ] minimum turning radius
- [ ] curvature constraints
- [ ] Dubins-style motion

One motivation is to explore how theoretical computational-geometry problems involving bounded-angle paths relate to physically meaningful robotic motion constraints.

---

# Humanoid Robotics Extension

Once the general planning framework is mature, the project will extend from mobile-robot paths to **humanoid footstep planning**.

## Phase 7 — Geometric Footstep Planning

A humanoid state may contain information such as:

```text
Left foot pose
Right foot pose
Next stepping foot
```

A footstep planner will use A* or related graph-search methods to search possible step sequences.

Initial feasibility checks will be purely geometric:

- step length
- step width
- relative foot orientation
- collision avoidance
- support-foot constraints
- geometric reachability regions

This establishes a baseline geometric footstep planner before dynamics are introduced.

---

## Phase 8 — Reduced-Order Humanoid Dynamics

Introduce progressively richer models of humanoid balance and locomotion.

Planned models include:

- [ ] Linear Inverted Pendulum Model (LIPM)
- [ ] Zero Moment Point (ZMP)
- [ ] Capture Point
- [ ] Divergent Component of Motion (DCM)
- [ ] Angular-Momentum Linear Inverted Pendulum (ALIP)

These models will provide increasingly realistic dynamic-feasibility checks for candidate footstep transitions.

---

## Phase 9 — Dynamic-Feasibility Dataset

Once a sufficiently accurate dynamic checker exists, generate a large dataset of candidate humanoid transitions.

Each transition can be labeled:

```text
Dynamically feasible
```

or:

```text
Dynamically infeasible
```

with associated state variables and transition geometry.

This dataset becomes the experimental foundation for studying the structure of humanoid dynamic feasibility.

---

## Phase 10 — Geometry of Dynamic Feasibility

Analyze the topology and geometry of the dynamically feasible transition set.

Candidate representations include:

- axis-aligned bounding boxes
- oriented bounding boxes
- ellipsoids
- convex hulls
- convex polytopes
- convex decompositions
- signed distance fields

The objective is to determine how accurately these representations approximate the ground-truth dynamic-feasibility region.

Of particular interest are **conservative approximations** that can guarantee that accepted transitions remain dynamically feasible.

---

## Phase 11 — Geometric Dynamic-Feasibility Filter

The final research stage will integrate the resulting approximation into the humanoid planner.

Instead of evaluating every candidate transition using the expensive dynamic model:

```text
Candidate Footstep
        |
        v
Cheap Geometric Filter
        |
   +----+----+
   |         |
Reject     Possibly
Immediately Feasible
             |
             v
      Dynamic Checker
```

The geometric filter should be extremely inexpensive compared with the full dynamic-feasibility calculation.

Experiments can then measure:

- computation-time reduction
- false rejection rate
- false acceptance rate
- planning success rate
- resulting path/footstep quality

This will directly investigate whether computational geometry can accelerate humanoid motion planning.

---

# Interactive Visualization

The project uses **SFML** to visualize algorithms as they are developed.

Rather than building the GUI only after the algorithms are complete, visualization is developed alongside the planning system.

Planned visualization milestones include:

### Demo 1 — Geometry Sandbox

Interactive visualization of:

- segment intersection
- polygon construction
- point-in-polygon queries
- path collision checking

### Demo 2 — Visibility Graph

Display obstacle vertices and all valid visibility edges.

### Demo 3 — Shortest-Path Planner

Select a start and goal and visualize the shortest collision-free path.

### Demo 4 — Configuration Space

Compare point-robot planning with finite-sized robot planning.

### Demo 5 — Clearance Planning

Visualize shortest paths versus maximum-clearance paths.

### Demo 6 — Sampling-Based Planning

Visualize PRM and RRT construction in real time.

### Demo 7 — Planner Comparison

Run multiple planners on the same environment and compare their results and performance.

Future humanoid extensions will visualize footsteps, reachability regions, dynamically feasible transitions, and geometric approximations of feasibility regions.

---

# Current Demo

The current interactive demo implements **segment-intersection visualization**.

The user selects four points:

```text
A → B

C → D
```

which define two line segments.

The program evaluates the segments using the project's computational-geometry implementation and visualizes the result:

```text
Green → no intersection
Red   → intersection
```

Press `R` to reset the environment.

---

# Building the Project

## Requirements

- C++17
- CMake
- SFML 3

On macOS with Homebrew:

```bash
brew install sfml
```

## Build and Run

A helper script is provided:

```bash
./run.sh
```

This configures the CMake project when necessary, builds the application, and launches the SFML demo.

---

# Running Unit Tests

Geometry algorithms are tested independently from the visualization layer.

Run:

```bash
./test.sh
```

The geometry library intentionally has no dependency on SFML.

This keeps the core planning algorithms independent from the user interface.

---

# Project Architecture

```text
src/
├── geometry/
│   ├── Point.hpp
│   ├── Segment.hpp
│   ├── Polygon.hpp
│   ├── Path.hpp
│   ├── Geometry.hpp
│   └── Geometry.cpp
│
├── environment/
│
├── graph/
│
├── planners/
│
├── visualization/
│   ├── Renderer.hpp
│   └── Renderer.cpp
│
├── experiments/
│
└── main.cpp

tests/
└── geometry_tests.cpp
```

The project follows a layered design:

```text
Geometry
   ↓
Environment
   ↓
Planning Algorithms
   ↓
Experiments

        ↑
        |
Visualization
```

The geometry and planning layers remain independent of SFML. The visualization layer consumes their results and renders them.

---

# Design Philosophy

This project prioritizes:

**Understanding over abstraction.**  
Core geometric and planning algorithms are implemented directly where practical rather than hidden behind large geometry libraries.

**Correctness before optimization.**  
Algorithms are unit tested before being integrated into planners.

**Algorithms before aesthetics.**  
The GUI exists to expose and understand algorithm behavior rather than hide it.

**Incremental development.**  
Every major algorithm produces a working visual milestone before the next major stage begins.

**Experimental reproducibility.**  
Planning algorithms will eventually be benchmarked using common environments, metrics, and datasets.

---

# Status

🚧 **Active Development**

Current focus:

> Computational geometry foundations and interactive visualization.

Current milestone:

> **Demo 1A — Segment Intersection Sandbox**

Next milestone:

> **Demo 1B — Interactive Polygon and Point-in-Polygon Visualization**

---

# Future Research Direction

The long-term objective is to connect classical computational geometry with modern humanoid robotics.

The project asks whether geometric structure hidden inside dynamically feasible humanoid motion can be exploited to produce planning algorithms that are substantially cheaper while remaining useful and interpretable.

The same computational-geometry foundations used to answer simple questions such as:

> "Do these two segments intersect?"

will eventually be used to investigate a considerably harder question:

> **"Can geometry tell us whether a humanoid motion is likely to be dynamically feasible before we perform the expensive dynamics calculation?"**