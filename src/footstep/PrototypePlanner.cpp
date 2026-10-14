#include "PrototypePlanner.hpp"
#include "../geometry/Clearance.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>

namespace footstep {
namespace {
constexpr double G = 9.81;

double norm(Point a){ return std::hypot(a.x,a.y); }
double dist(Point a, Point b){return std::hypot(a.x-b.x,a.y-b.y);}
Point plus(Point a,Point b){return {a.x+b.x,a.y+b.y};}
Point minus(Point a,Point b){return {a.x-b.x,a.y-b.y};}
Point times(Point a,double t){return {a.x*t,a.y*t};}
Point unit(double a){return {std::cos(a),std::sin(a)};}
bool inBounds(const Scene& sc, Point p, double margin){
    return p.x > margin && p.x < sc.width-margin &&
           p.y > margin && p.y < sc.height-margin;
}
bool safe(const Scene& sc,Point p,double r){
    return inBounds(sc,p,r) && isPointClearanceFree(p,sc.obstacles,r);
}
bool safeSegment(const Scene& sc, Point a,Point b,double r){
    return inBounds(sc,a,r) && inBounds(sc,b,r) &&
           isSegmentClearanceFree({a,b},sc.obstacles,r);
}
struct Grid {
    int nx,ny;
    double cell;
    const Scene& scene;
    const Settings& settings;
    int index(int x,int y)const{return y*nx+x;}
    Point at(int idx)const{return {((idx%nx)+0.5)*cell,((idx/nx)+0.5)*cell};}
    int nearest(Point p)const {
        int x=std::clamp(static_cast<int>(p.x/cell),0,nx-1);
        int y=std::clamp(static_cast<int>(p.y/cell),0,ny-1);
        return index(x,y);
    }
};
struct Frontier{double f;int idx;bool operator<(const Frontier& o)const{return f>o.f;}};
std::vector<Point> gridPath(const Scene& sc, const Settings& st, std::size_t& expanded){
    Grid g{static_cast<int>(sc.width/st.gridSize),static_cast<int>(sc.height/st.gridSize),st.gridSize,sc,st};
    if(g.nx<=0||g.ny<=0||!safe(sc,sc.start,st.clearance)||!safe(sc,sc.goal,st.clearance))return {};
    int n=g.nx*g.ny, source=g.nearest(sc.start), dest=g.nearest(sc.goal);
    if(!safe(sc,g.at(source),st.clearance)||!safe(sc,g.at(dest),st.clearance))return {};
    std::vector<double> best(static_cast<std::size_t>(n),std::numeric_limits<double>::infinity());
    std::vector<int> parent(static_cast<std::size_t>(n),-1);
    std::priority_queue<Frontier> pq;best[source]=0;pq.push({dist(g.at(source),sc.goal),source});
    constexpr int offsets[8][2]={{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};
    while(!pq.empty()){
        auto top=pq.top();pq.pop();
        int idx=top.idx;
        if(top.f>best[idx]+dist(g.at(idx),sc.goal)+1e-8)continue;
        ++expanded;
        if(idx==dest)break;
        int x=idx%g.nx,y=idx/g.nx;
        for(auto& off: offsets){
            int xx=x+off[0], yy=y+off[1];
            if(xx<0||yy<0||xx>=g.nx||yy>=g.ny)continue;
            int next=g.index(xx,yy);
            if(!safeSegment(sc,g.at(idx),g.at(next),st.clearance))continue;
            double score=best[idx]+dist(g.at(idx),g.at(next));
            if(score>=best[next])continue;
            best[next]=score;parent[next]=idx;
            pq.push({score+dist(g.at(next),sc.goal),next});
        }
    }
    if(!std::isfinite(best[dest]))return {};
    std::vector<Point> out;
    for(int v=dest;v!=-1;v=parent[v])out.push_back(g.at(v));
    std::reverse(out.begin(),out.end());
    // Exact start/goal connectors must satisfy clearance too.
    if(!safeSegment(sc,sc.start,out.front(),st.clearance) ||
       !safeSegment(sc,out.back(),sc.goal,st.clearance))return {};
    out.insert(out.begin(),sc.start);out.push_back(sc.goal);
    // Remove collinear intermediate lattice points without breaking clearance.
    for(std::size_t i=1;i+1<out.size();){
        Point a=minus(out[i],out[i-1]),b=minus(out[i+1],out[i]);
        double cross=std::abs(a.x*b.y-a.y*b.x);
        if(cross<1e-7 && safeSegment(sc,out[i-1],out[i+1],st.clearance))out.erase(out.begin()+static_cast<std::ptrdiff_t>(i));
        else ++i;
    }
    return out;
}
double routeDistance(Point p,const std::vector<Point>& path){
    if(path.size()<2)return 0;
    double nearest=std::numeric_limits<double>::infinity();
    for(std::size_t i=1;i<path.size();++i){
        Point a=path[i-1],b=path[i],v=minus(b,a),w=minus(p,a);
        double length=v.x*v.x+v.y*v.y;
        double t=length>1e-12?std::clamp((w.x*v.x+w.y*v.y)/length,0.0,1.0):0.0;
        nearest=std::min(nearest,dist(p,plus(a,times(v,t))));
    }
    return nearest;
}
std::string stateKey(const State& s,bool dynamic){
    auto q=[](double v,double cell)->int{return static_cast<int>(std::lround(v/cell));};
    std::string key=std::to_string(q(s.support.x,.25))+":"+std::to_string(q(s.support.y,.25))+
        ":"+std::to_string(q(s.heading,.26))+":"+std::to_string(s.nextSide);
    if(dynamic){
        Point xi=capturePoint(s,.90);
        key+=":"+std::to_string(q(xi.x,.28))+":"+std::to_string(q(xi.y,.28));
    }
    return key;
}
struct QueueItem {
    double f,g;int index;
    bool operator<(const QueueItem& o)const{return f>o.f;}
};
}
Scene sampleScene(bool obstructed){
    Scene scene;
    if(obstructed){
        scene.obstacles={
            Polygon{{{6.50,4.45},{8.55,4.45},{8.55,6.45},{6.50,6.45}}},
            Polygon{{{3.05,7.20},{4.35,7.20},{4.10,8.60},{2.95,8.45}}},
            Polygon{{{10.70,1.9},{12.20,1.9},{12.10,3.25},{10.85,3.10}}}
        };
    }
    return scene;
}
State propagateLIPM(const State& s,double t,double height){
    State result=s;
    const double w=std::sqrt(G/height), c=std::cosh(w*t), sh=std::sinh(w*t);
    const Point offset=minus(s.com,s.support);
    result.com=plus(s.support,plus(times(offset,c),times(s.velocity,sh/w)));
    result.velocity=plus(times(offset,w*sh),times(s.velocity,c));
    return result;
}
Point capturePoint(const State& s,double height){
    const double w=std::sqrt(G/height);
    return plus(s.com,times(s.velocity,1.0/w));
}
Result solve(const Scene& sc,const Settings& st,Layer layer){
    Result out;out.layer=layer;
    if(st.clearance<0 || st.maxStep<0.3 || st.stepTime<=0 || st.comHeight<=0 ||
       st.gridSize<=0 || st.maxExpansions<=0)return out;
    std::size_t gridExpanded=0;
    out.geometricPath=gridPath(sc,st,gridExpanded);
    if(layer==Layer::Geometry){
        out.expanded=gridExpanded;
        out.reachedGoal=!out.geometricPath.empty();
        return out;
    }
    if(out.geometricPath.empty())return out;
    State first;
    first.support=sc.start;
    first.other={sc.start.x-.55,sc.start.y+.24};
    first.com=sc.start;
    const double w0=std::sqrt(G/st.comHeight);
    const double e0=std::exp(w0*st.stepTime);
    // Initialize a periodic, straight-line point-foot LIPM gait:
    // for stride d, r_x=d/(exp(w*T)-1), and alternating width w_f,
    // r_y=w_f/(exp(w*T)+1) (DCM relative to current support).
    first.velocity={.45*w0/(e0-1.0),st.stepWidth*w0/(e0+1.0)};
    first.heading=0;first.nextSide=1;
    if(!safe(sc,first.support,.12)||!safe(sc,first.other,.12))return out;
    std::vector<State> nodes;nodes.reserve(35000);nodes.push_back(first);
    std::priority_queue<QueueItem> open;
    std::unordered_map<std::string,double> best;
    auto heur=[&](Point p){
        return dist(p,sc.goal)/std::max(st.maxStep,.4);
    };
    bool dynamic=layer==Layer::Dynamic;
    auto key0=stateKey(first,dynamic);
    best[key0]=0;
    open.push({heur(first.support),0,0});
    int end=0;double bestGoalDistance=dist(first.support,sc.goal);
    constexpr double turns[]={-.75,-.50,-.25,0,.25,.50,.75};
    constexpr double lengths[]={0,.15,.30,.45,.60,.75};
    // The shared search is deliberately lattice-based; no exact optimality guarantee in R^2.
    while(!open.empty()&&out.expanded<static_cast<std::size_t>(st.maxExpansions)){
        QueueItem next=open.top();open.pop();
        State cur=nodes[static_cast<std::size_t>(next.index)];
        if(next.g>cur.g+1e-9)continue;
        if(cur.depth>50)continue;
        if(dist(cur.support,sc.goal)<.65 &&
           (!dynamic||dist(cur.com,sc.goal)<1.2)){
            end=next.index;out.reachedGoal=true;break;
        }
        if(dist(cur.support,sc.goal)<bestGoalDistance){
            bestGoalDistance=dist(cur.support,sc.goal);end=next.index;
        }
        ++out.expanded;
        State predicted=dynamic?propagateLIPM(cur,st.stepTime,st.comHeight):cur;
        Point xi=dynamic?capturePoint(predicted,st.comHeight):Point{};
        double w=std::sqrt(G/st.comHeight);
        double captureBound=st.maxStep/(std::exp(w*st.stepTime)-1.0)+.13;
        for(double delta: turns)for(double d: lengths){
            double heading=cur.heading+delta;
            Point forward=unit(heading),left{-forward.y,forward.x};
            Point landing=plus(cur.support,plus(times(forward,d),times(left,st.stepWidth*cur.nextSide)));
            double actualLength=dist(cur.support,landing);
            if(actualLength>st.maxStep+1e-9){++out.rejectedKinematics;continue;}
            // No point may be accepted merely by being outside an obstacle:
            // test the entire swing segment (other foot -> new foot).
            if(!safe(sc,landing,.12)||!safeSegment(sc,cur.other,landing,.12)){
                ++out.rejectedGeometry;continue;
            }
            if(!dynamic){
                if(!safeSegment(sc,cur.support,landing,st.clearance)){
                    ++out.rejectedGeometry;continue;
                }
            }else{
                bool bodySafe=true;
                Point prev=cur.com;
                for(int sample=1;sample<=12;++sample){
                    double t=st.stepTime*static_cast<double>(sample)/12.0;
                    Point now=propagateLIPM(cur,t,st.comHeight).com;
                    if(!safeSegment(sc,prev,now,st.clearance+.01)){
                        bodySafe=false;break;
                    }
                    prev=now;
                }
                if(!bodySafe){++out.rejectedGeometry;continue;}
                // One-step DCM bound: necessary-style screening, not a
                // formal infinite-horizon capturability certificate.
                if(dist(xi,landing)>captureBound ||
                   dist(predicted.com,landing)>1.25 ||
                   norm(predicted.velocity)>3.5){
                    ++out.rejectedDynamics;continue;
                }
            }
            State successor=predicted;
            successor.support=landing;
            successor.other=cur.support;
            successor.heading=heading;
            successor.nextSide=-cur.nextSide;
            successor.parent=next.index;
            successor.depth=cur.depth+1;
            successor.g=cur.g+1.0+.10*std::abs(delta);
            std::string key=stateKey(successor,dynamic);
            auto it=best.find(key);
            if(it!=best.end()&&it->second<=successor.g+1e-7)continue;
            best[key]=successor.g;
            int index=static_cast<int>(nodes.size());
            nodes.push_back(successor);
            double guide=.1*routeDistance(landing,out.geometricPath);
            open.push({successor.g+1.12*heur(landing)+guide,successor.g,index});
        }
    }
    std::vector<State> reverse;
    for(int idx=end;idx>=0;idx=nodes[static_cast<std::size_t>(idx)].parent)
        reverse.push_back(nodes[static_cast<std::size_t>(idx)]);
    std::reverse(reverse.begin(),reverse.end());
    out.footsteps=std::move(reverse);
    return out;
}
}
