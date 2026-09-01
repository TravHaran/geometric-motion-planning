# PATHLAB — Geometric Motion Planning

PATHLAB is an educational computational-geometry and motion-planning project
written in C++17 with SFML 3. It builds classical 2D planning tools from first
principles, pairs them with focused tests and theory notes, and exposes their
behavior through an interactive visualizer.

The current implementation plans shortest paths for a point robot among
polygonal obstacles using visibility graphs, Dijkstra's algorithm, or A*. The
long-term direction is to progress from these foundations toward clearance
planning, configuration spaces, sampling-based methods, and eventually
humanoid motion-planning research. Those later stages are roadmap goals, not
current capabilities.

For detailed implementation status, conventions, and milestones, see
[PROJECT_STATE.md](PROJECT_STATE.md).

## Current features

- Geometry primitives for points, segments, polygons, paths, and triangles
- Orientation, point-on-segment, segment-intersection, point-in-polygon, and
  collision predicates
- Polygonal obstacle environments with boundary-aware collision semantics
- Undirected visibility graphs with Euclidean edge weights
- Dijkstra and A* shortest-path search with path reconstruction
- Planner instrumentation, including expanded-node order and count
- Focused geometry, graph, Dijkstra, and A* regression tests
- A sequence of permanent SFML demos documenting project milestones
- PATHLAB, an interactive planning and search-visualization application

## PATHLAB

PATHLAB is the current primary application (`demo_2e`). It provides an
interactive canvas for constructing polygonal scenes, placing start and goal
positions, selecting Dijkstra or A*, and running the planner.

The interface can display obstacles, the visibility graph, the final path, and
expanded nodes independently. Its metrics panel reports planner status,
obstacle and graph counts, path length and size, **Nodes Expanded**, graph-build
time, search time, and total time.

When a search trace is available, a floating playback dock can reset, play,
pause, or single-step through node expansions. Playback distinguishes the
current node, previously expanded nodes, and completion at the goal. Five
playback speeds are available.

The canvas supports cursor-centered zoom and panning without mixing camera
logic into the geometry or planner layers. Static canvas, obstacle, graph, and
path data are cached in batched SFML vertex arrays. Outside active playback,
the application waits for input instead of continuously redrawing an unchanged
scene.

## PATHLAB controls

All placement actions occur on the canvas. Sidebar rows, buttons, selectors,
and playback controls are operated with the left mouse button.

| Action | Control |
| --- | --- |
| Add an obstacle vertex | Left-click the canvas while in the default obstacle mode |
| Finish an obstacle | `Enter` after placing at least three vertices |
| Place or replace the start | Press `S`, then left-click the canvas |
| Place or replace the goal | Press `G`, then left-click the canvas |
| Cancel placement or discard an unfinished obstacle | `Esc` |
| Open or close Help | Click the top-bar **?** button or press `?` |
| Load the showcase scene | Click **Load Demo** at the upper-right of the canvas |
| Select a planner | Choose **Dijkstra** or **A\*** from the Algorithm selector |
| Run the selected planner | Click **Run Planner** or press `Space` |
| Reset the complete scene | `R` |
| Toggle the visibility graph | `V` or click **Visibility Graph** |
| Toggle obstacles | Click **Obstacles** |
| Toggle the final path | Click **Final Path** |
| Toggle expanded nodes | Click **Explored Nodes** after a search trace exists |
| Pan the canvas | Hold `Option` (`Alt`) and left-drag |
| Zoom around the pointer | Use the mouse wheel or trackpad scroll gesture over the canvas |
| Reset the camera | Click **Reset View** at the upper-right of the canvas |
| Reset search playback | Click the reset control in the playback dock |
| Play or pause playback | Click the play/pause control in the playback dock |
| Advance one expansion | Click the step control in the playback dock |
| Change playback speed | Click the speed control to cycle `0.25×`, `0.5×`, `1×`, `2×`, and `4×` |

`Esc` first closes the open Algorithm menu when applicable. PATHLAB does not
currently provide a separate clear command; `R` removes all obstacles, the
start and goal, and the current planning result.

**Load Demo** replaces the current scene with a deterministic four-obstacle
showcase and resets the camera to its default framing. It preserves the selected
planner and visualization preferences, clears any previous result/playback, and
does not run the planner automatically.

## Build and run

### Requirements

- A C++17 compiler
- CMake 3.16 or newer
- SFML 3 with the Graphics, Window, and System components

On macOS with Homebrew, the dependencies can be installed with:

```bash
brew install cmake sfml
```

Use the repository's runner to configure a Release build in `build-release/`,
build one target, and launch it:

```bash
./run.sh demo_2e
```

The script requires a target name; running `./run.sh` without one only prints
usage information. Available demo targets are:

| Target | Milestone |
| --- | --- |
| `demo_1a` | Segment intersection |
| `demo_1b` | Point in polygon |
| `demo_1c` | Path collision |
| `demo_2a` | Visibility graph |
| `demo_2b` | Dijkstra on a visibility graph |
| `demo_2c` | Interactive Dijkstra planner |
| `demo_2d` | Interactive Dijkstra/A* comparison |
| `demo_2e` | PATHLAB |

## Tests

Run the complete regression suite from the repository root:

```bash
./test.sh
```

This builds and runs the geometry, graph, Dijkstra, and A* test executables.
The mathematical and planning layers remain independent of SFML.

## Project structure

| Path | Responsibility |
| --- | --- |
| `src/geometry/` | Geometry types, predicates, collision logic, and polygon triangulation |
| `src/graph/` | Graph representation and visibility-graph construction |
| `src/planners/` | Dijkstra and A* search implementations |
| `src/visualization/` | Reusable SFML drawing helpers and legacy demo panels |
| `src/ui/` | PATHLAB interface layout, drawing, and hit testing |
| `src/app/` | PATHLAB application state, input, planning orchestration, playback, camera, and rendering |
| `demos/` | Executable milestones from foundational geometry through PATHLAB |
| `tests/` | Lightweight assert-based regression tests |
| `theory/` | LaTeX notes that develop the geometry and planning concepts alongside the code |

The dependency direction is intentionally simple: geometry, graph, and planner
code do not depend on SFML; visualization and application code consume their
results.

## Roadmap

The current classical 2D planning phase remains in progress. Its next major
milestone is Voronoi/generalized Voronoi geometry and maximum-clearance
planning, including comparison with visibility-graph shortest paths.

Later planned stages include:

1. Configuration-space geometry for finite-sized robots
2. Sampling-based planning with PRM, RRT, and RRT*
3. Humanoid footstep planning
4. Reduced-order humanoid dynamics
5. Dynamic-feasibility datasets and geometric approximations

These items describe intended research and development directions. They have
not yet been implemented.

## Development philosophy

The project follows the sequence:

> Theory → Implementation → Tests → Visualization → Experiment

Core algorithms are implemented directly where practical so their geometric
and planning assumptions remain visible. Readability and correctness take
priority over premature optimization, while each demo preserves a working
milestone in the project's progression.
