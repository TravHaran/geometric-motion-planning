#include "../src/footstep/PrototypePlanner.hpp"
#include "../src/geometry/Clearance.hpp"
#include <cassert>
#include <cmath>

int main() {
    using namespace footstep;
    Settings cfg;
    Scene open=sampleScene(false);
    Result geometry=solve(open,cfg,Layer::Geometry);
    assert(geometry.reachedGoal);
    assert(geometry.geometricPath.size()>1);
    for(std::size_t i=1;i<geometry.geometricPath.size();++i)
        assert(isSegmentClearanceFree({geometry.geometricPath[i-1],geometry.geometricPath[i]},
                                     open.obstacles,cfg.clearance));
    Scene blocked=sampleScene(true);
    Result detour=solve(blocked,cfg,Layer::Geometry);
    assert(detour.reachedGoal);
    for(std::size_t i=1;i<detour.geometricPath.size();++i)
        assert(isSegmentClearanceFree({detour.geometricPath[i-1],detour.geometricPath[i]},
                                     blocked.obstacles,cfg.clearance));
    // Tiny goal allows a complete, independently checkable one-step solution.
    // Regression: the showcase should actually be navigable by both step planners.
    Result kinDetour=solve(blocked,cfg,Layer::Kinematic);
    assert(kinDetour.reachedGoal);
    Result dynDetour=solve(blocked,cfg,Layer::Dynamic);
    assert(dynDetour.reachedGoal);
    open.goal={2.18,5.18};
    Result kin=solve(open,cfg,Layer::Kinematic);
    assert(kin.reachedGoal && kin.footsteps.size()>1);
    for(std::size_t i=1;i<kin.footsteps.size();++i){
        const auto& before=kin.footsteps[i-1];
        const auto& after=kin.footsteps[i];
        assert(std::hypot(before.support.x-after.support.x,
                          before.support.y-after.support.y) <= cfg.maxStep+1e-9);
        assert(before.nextSide==-after.nextSide);
    }
    Result dyn=solve(open,cfg,Layer::Dynamic);
    assert(dyn.reachedGoal && dyn.footsteps.size()>1);
    State s{};
    s.support={0,0};s.com={0,0};s.velocity={1,0};
    const State after=propagateLIPM(s,.2,1.0);
    const double w=std::sqrt(9.81);
    assert(std::abs(after.com.x-std::sinh(w*.2)/w)<1e-10);
    assert(std::abs(after.velocity.x-std::cosh(w*.2))<1e-10);
    assert(std::abs(capturePoint(s,1.0).x-1.0/w)<1e-10);
}
