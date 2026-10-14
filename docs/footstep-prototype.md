# PATHLAB Biped Prototype (demo_3a)

This is an **experimental three-layer planner prototype**. It lives alongside the existing PATHLAB demo_2e; it does not replace or silently alter the reference BFS/Dijkstra/A* implementation.

## Quick start

On a Mac with SFML 3 and CMake installed:

    git fetch origin
    git switch feature/clearance-geometry
    ./run.sh demo_3a

The new demo uses world coordinates in **metres**. The body is modeled as a point with a configurable clearance disk; feet are modeled as point placements with a 0.12 m collision radius. Obstacles are infinite-height extrusions of the polygonal footprints.

## Controls

- Click modes 1/2/3, or press keys 1/2/3, to choose geometric / kinematic / LIPM planning.
- Drag the three controls for clearance, maximum step length, and step duration.
- Click or drag the timeline at the bottom of the canvas to scrub.
- Space: play/pause; left/right arrows: single step; R: replan.
- M or the map button: toggle between obstacle scene and open space.

## How it works

**Stage 1 — geometry.** Eight-neighbor A* on a 0.2 m lattice, with continuous segment-to-polygon Euclidean clearance predicates. Every accepted lattice edge has its full segment checked. The output is optimal *on the sampled grid graph*, not a globally shortest Euclidean clearance route. The Minkowski exclusion band displayed in the scene is a *visual overlay*, constructed from disks at obstacle vertices and thick polygon edges.

**Stage 2 — kinematics.** Search states contain support foot, previous foot, heading, and next foot side. Successors are generated from a restricted set of forward lengths, steering angles, and alternating lateral offsets, then checked against maximum step reach, foot placement collision, approximate swing-foot straight-segment collision, and clearance of a support-center segment. This is a demonstrative footstep lattice, **not a joint-space reachability model**. In particular, it doesn't check IK, realistic swing-foot trajectories, heading slew limitations, or body/limb shape throughout a step.

**Stage 3 — dynamics.** States additionally contain horizontal CoM position and velocity. Each support period uses the ideal fixed-CoP linear inverted pendulum model

    c(t) = p + (c0-p) cosh(omega*t) + (v0/omega) sinh(omega*t)
    v(t) = omega*(c0-p) sinh(omega*t) + v0*cosh(omega*t)
    omega = sqrt(g/h)

Predicted CoM movement is collision-screened at 12 sampling intervals with a small extra margin. An adjustable-step-bound based on the DCM xi=c+v/omega prunes candidate landings that are far from the predicted capture point. This is a **screening heuristic**, not a mathematically sufficient viability criterion, torque-feasibility test, or model predictive controller. No stability or collision guarantees between discrete time samples are proven. A reported success means reaching the designated goal neighborhood **under these implemented predicates**, not coming to rest or executing safely on hardware.

The footstep search uses a **weighted A***-style heuristic (1.12 times a Euclidean steps-to-go estimate plus path guidance). Therefore **optimality is not claimed** even over the generated footstep graph. Changing max step length can modify candidate reachability. A failed search may mean insufficient discretization or the search budget was exhausted; it is not proof that no valid humanoid plan exists.

## Evaluation and research roadmap

This is a baseline for exploring:
1. Exact/conservative geometric offsets and robust distance predicates.
2. Ground-height IK reachable polygons under fixed pelvis pose; continuously safe swing trajectories.
3. More principled LIPM support scheduling, viable-set computation, finite-foot CoP, capture-point and stability certification.
4. Sound geometric inner and outer approximations of dynamic feasible next-step sets.
5. Admissible heuristics, better state hashing (including the active CoM dynamics parameter), search completeness relative to discretization.
6. A unified playback renderer in demo_2e once the prototype is numerically trusted.

Known limitations: an SFML standalone demo instead of integration into demo_2e; no per-obstacle edit controls; no whole-body dynamics, no terminal stop, approximate collision sampling during curved CoM trajectories; lattice-dependent search; the optional reachable disk is only a **superset** of the true step-generating geometry.

## Tests and build

    cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
    cmake --build build-release --target demo_3a clearance_tests footstep_tests
    ctest --test-dir build-release --output-on-failure

Run demo_2e separately if you want to compare against the original classical planner.
