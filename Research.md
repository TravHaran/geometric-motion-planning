# PATHLAB Research Roadmap

**Working title:** *From Dynamics to Geometry: Conservative Feasibility Surrogates for Humanoid Footstep Planning*  
**Status:** Active research direction  
**Last updated:** September 1, 2026

## 1. Purpose

PATHLAB is both:

1. A learning-oriented computational geometry and motion-planning platform.
2. A research platform for studying whether expensive humanoid dynamic-feasibility evaluations can be distilled into inexpensive geometric tests.

The research and software roadmaps progress **in parallel**. Formalization, proofs, literature review, oracle specification, and low-dimensional pilot experiments do not need to wait for every PATHLAB planning milestone. The two programs converge when the geometric feasibility model is integrated into a humanoid footstep planner.

Current engineering implementation status is tracked separately in
[PROJECT_STATE.md](PROJECT_STATE.md).

## 2. Primary Research Question

> Can closed-loop dynamic feasibility of humanoid footstep transitions be distilled into conservative, state-conditioned geometric inner and outer sets that allow a global footstep planner to accept or reject most candidate transitions without querying an expensive dynamics oracle?

The guiding idea is:

```text
expensive offline dynamics/controller evaluations
                        ↓
       geometric model of transition feasibility
                        ↓
        cheap online set-membership queries
                        ↓
             global footstep planner
```

## 3. Central Hypothesis

The dynamically feasible transition space of a humanoid has enough low-dimensional geometric structure that conservative geometric approximations can eliminate a substantial fraction of expensive dynamic-feasibility queries during graph search while producing plans that closely agree with an oracle-based planner.

This is a falsifiable hypothesis. A negative result—such as discovering that the feasible set is too state-dependent, fragmented, or high-dimensional to approximate efficiently—would still be scientifically meaningful.

The terms *substantial fraction* and *closely agree* must be assigned measurable thresholds before confirmatory evaluation; they are not success criteria by themselves.

## 4. Scope and Non-Claims

The initial study is intentionally limited to:

- Flat, known terrain.
- Alternating left/right footsteps.
- Planar foot poses.
- A fixed robot model and fixed downstream controller.
- A small conditioning state.
- Linear inverted pendulum model (LIPM) or angular-momentum linear inverted pendulum (ALIP) dynamics before higher-fidelity models.
- Offline dataset generation and online graph-search evaluation.

The project does **not** initially claim:

- A universal feasibility model for every humanoid.
- Formal safety guarantees from finite empirical data.
- Immediate transfer to rough terrain or multi-contact locomotion.
- Replacement of the downstream locomotion controller.
- Real-robot validation in the first study.
- That a particular geometric representation is best before the data are analyzed.

## 5. Formal Problem Statement

Let:

- $x \in \mathcal X$ denote the current humanoid state.
- $u \in \mathcal U(x)$ denote a candidate footstep transition.
- $\theta$ denote fixed robot, controller, model, and terrain parameters.
- $O_\theta(x,u) \in \{0,1\}$ denote a closed-loop dynamic-feasibility oracle.

The oracle returns:

$$
O_\theta(x,u)=
\begin{cases}
1, & \text{if the specified controller successfully executes }u,\\
0, & \text{otherwise.}
\end{cases}
$$

The exact feasible transition set is:

$$
\mathcal F_\theta(x)
=
\{u\in\mathcal U(x):O_\theta(x,u)=1\}.
$$

The proposed geometric model aims to construct:

$$
\mathcal G_{\mathrm{in},\theta}(x)
\subseteq
\mathcal F_\theta(x)
\subseteq
\mathcal G_{\mathrm{out},\theta}(x).
$$

This exact sandwich is the theoretical target. A model fitted from finite data
will instead produce estimated sets
$\widehat{\mathcal G}_{\mathrm{in},\theta}(x)$ and
$\widehat{\mathcal G}_{\mathrm{out},\theta}(x)$; the exact inclusions must not
be assumed for those estimates and must be evaluated empirically.
The estimated inner set should be constrained to lie inside the estimated outer
set so that the three-way rule is unambiguous. That estimated nesting alone does
not prove that either set bounds $\mathcal F_\theta(x)$.

The corresponding decision rule is:

$$
D(x,u)=
\begin{cases}
\text{accept}, & u\in\mathcal G_{\mathrm{in},\theta}(x),\\
\text{query the oracle},
& u\in\mathcal G_{\mathrm{out},\theta}(x)
\setminus\mathcal G_{\mathrm{in},\theta}(x),\\
\text{reject}, & u\notin\mathcal G_{\mathrm{out},\theta}(x).
\end{cases}
$$

The uncertainty region is:

$$
\mathcal B_\theta(x)
=
\mathcal G_{\mathrm{out},\theta}(x)
\setminus
\mathcal G_{\mathrm{in},\theta}(x).
$$

For an exact sandwich, the rule introduces no false acceptances or false
rejections relative to the oracle labels. For estimated sets, the central
computational objective is to make the uncertainty region small—under the
distribution of transitions that a planner actually considers—while measuring
and controlling both error types.

## 6. Parallel Research Program

### Track A — Formal Mathematics

**Goal:** Establish a precise mathematical foundation before committing to an implementation-specific solution.

- [ ] Fix the notation for state, transition, controller parameters, and oracle.
- [ ] Define closed-loop transition success and failure.
- [ ] State the assumptions under which the oracle is deterministic.
- [ ] Define exact inner and outer conservatism.
- [ ] Define empirical or probabilistic conservatism.
- [ ] Define false acceptance, false rejection, and uncertainty-band size.
- [ ] Prove the exact-sandwich planner-preservation proposition.
- [ ] Determine which results require a fixed planner graph and which extend to implicit graphs.
- [ ] Formalize planner-level completeness and optimality consequences.
- [ ] Define geometric complexity and online membership-query cost.
- [ ] Formulate the surrogate-construction problem as a constrained optimization problem.
- [ ] Maintain a theorem, conjecture, and counterexample ledger.

#### First target proposition

If

$$
\mathcal G_{\mathrm{in}}(x)
\subseteq
\mathcal F(x)
\subseteq
\mathcal G_{\mathrm{out}}(x)
$$

holds for every planner state, the oracle is deterministic for the fixed
experiment, and transitions in the uncertainty region are evaluated by that
exact oracle, then the filtered planner classifies every candidate edge exactly
as the oracle-only planner. If both planners also use the same state
representation, candidate-edge generation, and edge costs, they search the same
feasible weighted graph. Subject to the usual assumptions of the underlying
graph-search algorithm, the filtered planner therefore preserves completeness
and optimality relative to the oracle-only planner.

#### Empirical error events

Suppressing the fixed parameter $\theta$ for readability, a false acceptance
is:

$$
u\in\widehat{\mathcal G}_{\mathrm{in}}(x)
\quad\text{and}\quad
O(x,u)=0.
$$

A false rejection is:

$$
u\notin\widehat{\mathcal G}_{\mathrm{out}}(x)
\quad\text{and}\quad
O(x,u)=1.
$$

These errors have different consequences and must be reported separately.

**Deliverables**

- [ ] Formal Problem Statement v0.1
- [ ] Definitions and Notation Sheet
- [ ] Exact-Sandwich Proposition and Proof
- [ ] Empirical Error Definitions
- [ ] Candidate Optimization Objective
- [ ] Theorem and Conjecture Ledger

### Track B — Geometry of the Feasible Set

**Goal:** Determine what geometric structure, if any, makes dynamic feasibility cheaply representable.

#### Structural questions

- [ ] Is the feasible set convex?
- [ ] Is it connected or composed of multiple components?
- [ ] Is it star-shaped with respect to a meaningful nominal step?
- [ ] Does it exhibit left/right reflection symmetry?
- [ ] Is feasibility monotone along any transition variable?
- [ ] How does center-of-mass velocity deform the boundary?
- [ ] How quickly does the set vary with the conditioning state?
- [ ] Is the boundary smooth, piecewise smooth, or irregular?
- [ ] Does conditioning produce simpler low-dimensional slices?
- [ ] Which geometric features correlate with planner-critical transitions?

#### Candidate representations

- [ ] Axis-aligned boxes
- [ ] Oriented boxes
- [ ] Ellipsoids
- [ ] Convex hulls and halfspace polytopes
- [ ] Unions of convex polytopes
- [ ] Convex decompositions
- [ ] Occupancy grids or adaptive cells
- [ ] Signed-distance representations
- [ ] Support-function representations
- [ ] A learned classifier as a non-geometric baseline

Each representation should be evaluated by:

- Expressiveness.
- Inner/outer conservatism.
- Construction cost.
- Membership-query cost.
- Storage complexity.
- Scaling with dimension.
- Interpretability.
- Robustness to conditioning-state changes.

> Important: the convex hull of feasible samples is not automatically an inner approximation. If the true feasible set is nonconvex, the hull may include infeasible transitions.

**Deliverables**

- [ ] Representation Comparison Matrix
- [ ] Synthetic Feasible-Set Benchmarks
- [ ] Initial Lemmas and Counterexamples
- [ ] Inner/Outer/Uncertainty Visualizations
- [ ] Representation-Selection Criteria

### Track C — Literature and Novelty

**Goal:** Position the proposed geometric-distillation approach precisely against existing humanoid planning research.

Organize related work into:

- Kinematic footstep planning.
- Capturability and balance recovery.
- Reduced-order dynamic planning.
- Whole-body trajectory optimization and model-predictive control (MPC).
- Learned feasibility and dynamics prediction.
- Multi-fidelity planning.
- Feasible wrench and actuation regions.
- Reachability analysis.
- Geometric set approximation.
- Planner-aware or active data acquisition.

- [ ] Maintain a living literature matrix.
- [ ] Write a short summary of every core paper.
- [ ] Record the model, state variables, planner, guarantees, and limitations of each method.
- [ ] Identify the closest work and state the difference carefully.
- [ ] Track which novelty claims are supported, provisional, or unresolved.
- [ ] Ask a computational geometry researcher to identify missing geometry literature.
- [ ] Ask a robotics researcher to review the oracle and locomotion assumptions.

**Deliverables**

- [ ] Related-Work Matrix
- [ ] Closest-Work Comparison
- [ ] Provisional Novelty Statement
- [ ] Open Literature Questions

### Track D — Oracle and Pilot Experiments

**Goal:** Build the smallest scientifically meaningful experiment that exposes the geometry of dynamic feasibility.

#### Oracle specification

Define before implementation:

- Robot or reduced-order model.
- Controller.
- Initial dynamic state.
- Candidate footstep parameterization.
- Step duration.
- Terrain assumptions.
- Simulation horizon.
- Success and failure criteria.
- Numerical tolerances.
- Reproducibility settings.

An initial transition vector may be:

$$
z=[\Delta x,\Delta y,v_x],
$$

where $\Delta x$ and $\Delta y$ are relative foot-placement coordinates and
$v_x$ is forward center-of-mass velocity. Later extensions may include:

$$
\Delta\theta,\;v_y,\;L_x,\;L_y,\;T,\;\text{payload}.
$$

Here $\Delta\theta$ is relative foot yaw, $v_y$ is lateral center-of-mass
velocity, $L_x$ and $L_y$ are angular-momentum components, and $T$ is step
duration. The exact coordinates and reference frames must be fixed in the
oracle specification.

#### Pilot procedure

- [ ] Select only two or three transition variables.
- [ ] Define physically meaningful bounds for each variable.
- [ ] Implement and independently validate a LIPM- or ALIP-based oracle.
- [ ] Sample the domain broadly.
- [ ] Label each transition using the oracle.
- [ ] Add boundary-focused or active sampling.
- [ ] Visualize feasible and infeasible samples.
- [ ] Analyze convexity, connectivity, symmetry, and boundary shape.
- [ ] Construct initial inner and outer approximations.
- [ ] Evaluate on held-out states and transitions.
- [ ] Compare geometric models with a non-geometric classifier.
- [ ] Record negative results and failure cases.

#### Synthetic precursor

Before the humanoid oracle is ready, validate the geometric pipeline on sets with known boundaries:

- [ ] Convex ellipsoid
- [ ] Nonconvex crescent
- [ ] Disconnected set
- [ ] Thin corridor
- [ ] Polytope
- [ ] Smooth state-conditioned deformation

Synthetic experiments validate the approximation machinery, not the robotics hypothesis.

**Deliverables**

- [ ] Oracle Specification v0.1
- [ ] Oracle Validation Tests
- [ ] Synthetic Benchmark Results
- [ ] Pilot Dataset
- [ ] Geometric Structure Analysis
- [ ] Pilot Results Memo

### Track E — PATHLAB Engineering

**Goal:** Continue building the eventual integration platform using a theory-first, incremental workflow.

#### Current status

- [x] Computational geometry foundations
- [x] Visibility-graph construction
- [x] Euclidean graph weights
- [x] BFS
- [x] Dijkstra's algorithm
- [x] A* with Euclidean heuristic
- [x] Path reconstruction
- [x] Expanded-node instrumentation
- [x] Planner-trace visualization and comparison
- [ ] Voronoi/generalized Voronoi diagram (GVD) maximum-clearance planning
- [ ] Shortest-path-versus-clearance experiment

#### Subsequent engineering sequence

- [ ] Complete remaining classical planning baselines.
- [ ] Implement configuration-space geometry.
- [ ] Implement Minkowski sums and obstacle expansion.
- [ ] Support polygonal robot footprints.
- [ ] Introduce planar position-and-orientation planning in SE(2).
- [ ] Implement and time-box probabilistic roadmaps (PRM), rapidly exploring random trees (RRT), and RRT*.
- [ ] Implement flat-ground alternating-foot A*.
- [ ] Integrate the oracle and geometric feasibility filter.

#### Engineering workflow

For each major technical milestone:

1. Learn and document the theory.
2. Inspect the existing architecture.
3. Implement incrementally.
4. Add tests, including degeneracies and failure cases.
5. Visualize the algorithm.
6. Run controlled experiments.
7. Document intentional design decisions.
8. Commit the completed milestone.

### Track F — Planner-Level Theory and Evaluation

**Goal:** Determine whether the geometric approximation improves planning rather than merely achieving good pointwise classification.

- [ ] Define the oracle-only reference planner.
- [ ] Define the three-way filtered planner.
- [ ] Prove behavior under exact inner/outer inclusions.
- [ ] Analyze false-acceptance and false-rejection consequences.
- [ ] Define a final-path validation and replanning policy if required.
- [ ] Distinguish uniform-data accuracy from planner-weighted accuracy.
- [ ] Define the planner-induced transition distribution.
- [ ] Compare planner decisions with the oracle-only baseline.

#### Planner-level metrics

- Oracle calls avoided.
- Membership-query latency.
- Total planning time.
- Expanded nodes.
- Path cost and step count.
- Planning success.
- Execution success.
- Agreement with the oracle-only planner.
- Optimality loss or regret.
- Replanning frequency.
- Memory and representation complexity.

**Deliverables**

- [ ] Oracle-Only Planner Baseline
- [ ] Three-Way Filtered Planner
- [ ] Planner-Level Evaluation Protocol
- [ ] Ablation Studies
- [ ] Integrated Results Report

## 7. Convergence Gates

### Gate 1 — Professor-Ready Formal Proposal

Does **not** require Footstep A*.

- [ ] Formal problem definition
- [ ] Exact-sandwich proposition and proof sketch
- [ ] Literature matrix
- [ ] Candidate representation comparison
- [ ] Oracle specification
- [ ] Pilot experimental plan
- [ ] Clear limitations and non-claims
- [ ] Questions for faculty review

### Gate 2 — Pilot Evidence

Does **not** require full PATHLAB integration.

- [ ] Validated reduced-order oracle
- [ ] Two- or three-variable dataset
- [ ] Feasible-set visualization
- [ ] At least two inner/outer representations
- [ ] Held-out error measurements
- [ ] Uncertainty-band measurements
- [ ] Pilot conclusions and limitations

### Gate 3 — Planner Demonstration

- [ ] Alternating-foot A*
- [ ] Validated oracle
- [ ] Geometric filter
- [ ] Oracle-only baseline
- [ ] Planner-level comparison
- [ ] Reproducible benchmark scenarios

### Gate 4 — Research-Paper Package

- [ ] Finalized problem statement
- [ ] Strong related-work differentiation
- [ ] Reproducible methodology
- [ ] Statistical analysis
- [ ] Ablation studies
- [ ] Limitations and negative results
- [ ] Paper draft
- [ ] Faculty-reviewed presentation

## 8. Immediate Priorities

### Research priorities

1. Write the formal notation and assumptions.
2. Define the oracle independently of its implementation.
3. Prove the exact-sandwich planner-preservation proposition.
4. Define empirical false acceptance, false rejection, and uncertainty.
5. Complete the related-work matrix.
6. Select the first two- or three-variable pilot.
7. Specify the reduced-order oracle.
8. Build synthetic geometric benchmarks.
9. Review the formulation with Dr. Ahmad Biniaz.
10. Revise the pilot based on faculty feedback.

### PATHLAB priorities

1. Implement and visualize Voronoi/GVD maximum-clearance planning.
2. Compare visibility-graph shortest paths with high-clearance paths.
3. Measure path length, minimum clearance, planning time, expanded nodes, and graph size.
4. Preserve existing BFS, Dijkstra, A*, and visualization behavior.

## 9. Proposed Fall 2026 Targets

### Early September — Formal foundation

- [ ] Formal notation and assumptions
- [ ] Exact-sandwich proposition
- [ ] Research subquestions
- [ ] Oracle Specification v0.1
- [ ] Literature matrix
- [ ] One-page professor briefing

### Late September to mid-October — Mathematical prototype

- [ ] Synthetic feasible-set benchmarks
- [ ] Inner/outer construction prototypes
- [ ] Representation criteria
- [ ] Formal empirical-error definitions
- [ ] Reduced-order oracle implementation started

### Mid-October to mid-November — Feasibility pilot

- [ ] Validated reduced-order oracle
- [ ] Two- or three-variable dataset
- [ ] Boundary-focused sampling
- [ ] First geometric models
- [ ] Held-out evaluation
- [ ] Pilot results memo

### Mid-November to December — Research package

- [ ] Revised mathematical formulation
- [ ] Updated novelty analysis
- [ ] Planner-level experimental design
- [ ] Ten-minute research presentation
- [ ] Semester research report
- [ ] Next-stage experimental plan

If Footstep A* is not ready by the end of the semester, the theoretical and pilot work still count as meaningful research progress. Integration moves forward when both tracks are ready.

## 10. Questions for Faculty Review

1. Is the inner/outer approximation formulation mathematically compelling?
2. Which geometric representation families deserve priority?
3. What approximation guarantees may be possible?
4. Can useful sample-complexity statements be established?
5. How should the complexity of a state-conditioned geometric model be measured?
6. Are there relevant results on approximating parameterized nonconvex sets?
7. Is planner-weighted approximation a defensible computational-geometry objective?
8. Which assumptions are too strong or insufficiently specified?
9. What result would constitute an appropriate first paper?
10. What geometry or approximation literature is currently missing?

## 11. Decision Rules

- Do not increase the state dimension until the low-dimensional pilot is understood.
- Do not select a final representation before examining the feasible-set geometry.
- Do not treat classifier accuracy as sufficient evidence of planner usefulness.
- Do not claim formal safety from empirical test performance.
- Keep the robot, controller, terrain, and success criterion fixed within an experiment.
- Compare every surrogate against the same oracle and planner baseline.
- Record negative and inconclusive results.
- Prefer the simplest model that satisfies the experimental objective.
- Keep research code reproducible even when it is not yet integrated into PATHLAB.

## 12. Progress Log Template

Add one entry after each meaningful research session or milestone.

```markdown
### YYYY-MM-DD — Short title

**Track:** Formal Mathematics / Geometry / Literature / Oracle / PATHLAB / Planner Evaluation

**Question:**
What question was investigated?

**Work completed:**
- Item
- Item

**Result:**
What was learned, proved, implemented, disproved, or left inconclusive?

**Evidence:**
Links to notes, code, tests, figures, datasets, or commits.

**Decision:**
What changed in the research plan, if anything?

**Next action:**
The smallest concrete follow-up task.
```

## 13. Research Decision Log

Use this table for decisions that materially change the project.

| Date | Decision | Evidence or reason | Consequence |
| --- | --- | --- | --- |
| 2026-09-01 | Run research and PATHLAB engineering in parallel | Formalization, literature work, and reduced-order pilots do not require the complete planner stack | Research progress is no longer gated by sequential PATHLAB milestones |

## 14. Definition of Research Progress

Progress includes more than integrated code. Any of the following count when documented and verified:

- A clearer definition or assumption.
- A proof, counterexample, or failed proof attempt.
- A literature comparison that changes the novelty claim.
- A validated oracle component.
- A reproducible experiment.
- A negative result that rules out a representation.
- A better evaluation metric.
- Faculty feedback that changes the scope.
- A tested PATHLAB planning milestone.
- An integration result connecting geometry, dynamics, and planning.

The long-term objective is not simply to implement a humanoid footstep planner. It is to determine whether closed-loop dynamic feasibility possesses exploitable geometric structure—and whether that structure can make explicit global planning substantially cheaper without obscuring how planner decisions are made.
