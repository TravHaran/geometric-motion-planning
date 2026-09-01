#include "Triangulation.hpp"

#include <cmath>
#include <cstddef>
#include <vector>


namespace{


constexpr double EPSILON =
    1e-9;


double cross(
    const Point& a,
    const Point& b,
    const Point& c
){
    return
        (b.x - a.x)
            * (c.y - a.y)
        -
        (b.y - a.y)
            * (c.x - a.x);
}


double signedPolygonArea(
    const Polygon& polygon
){
    double area =
        0.0;


    const std::size_t n =
        polygon.vertices.size();


    for(std::size_t i = 0;
        i < n;
        ++i){

        const Point& a =
            polygon.vertices[i];

        const Point& b =
            polygon.vertices[
                (i + 1) % n
            ];


        area +=
            a.x * b.y
            -
            b.x * a.y;
    }


    return
        area / 2.0;
}


bool pointInTriangle(
    const Point& point,
    const Triangle& triangle
){
    const double c1 =
        cross(
            triangle.a,
            triangle.b,
            point
        );

    const double c2 =
        cross(
            triangle.b,
            triangle.c,
            point
        );

    const double c3 =
        cross(
            triangle.c,
            triangle.a,
            point
        );


    const bool hasNegative =
        c1 < -EPSILON
        ||
        c2 < -EPSILON
        ||
        c3 < -EPSILON;


    const bool hasPositive =
        c1 > EPSILON
        ||
        c2 > EPSILON
        ||
        c3 > EPSILON;


    return
        !(hasNegative &&
          hasPositive);
}


bool isConvexVertex(
    const Point& previous,
    const Point& current,
    const Point& next,
    bool counterClockwise
){
    const double value =
        cross(
            previous,
            current,
            next
        );


    if(counterClockwise){

        return
            value > EPSILON;
    }


    return
        value < -EPSILON;
}


bool isEar(
    const Polygon& polygon,
    const std::vector<std::size_t>& indices,
    std::size_t earPosition,
    bool counterClockwise
){
    const std::size_t count =
        indices.size();


    const std::size_t previousPosition =
        (earPosition + count - 1)
        % count;

    const std::size_t nextPosition =
        (earPosition + 1)
        % count;


    const std::size_t previousIndex =
        indices[
            previousPosition
        ];

    const std::size_t currentIndex =
        indices[
            earPosition
        ];

    const std::size_t nextIndex =
        indices[
            nextPosition
        ];


    const Point& previous =
        polygon.vertices[
            previousIndex
        ];

    const Point& current =
        polygon.vertices[
            currentIndex
        ];

    const Point& next =
        polygon.vertices[
            nextIndex
        ];


    if(!isConvexVertex(
        previous,
        current,
        next,
        counterClockwise
    )){

        return false;
    }


    const Triangle candidate{
        previous,
        current,
        next
    };


    for(const std::size_t index :
        indices){

        if(index == previousIndex ||
           index == currentIndex ||
           index == nextIndex){

            continue;
        }


        if(pointInTriangle(
            polygon.vertices[index],
            candidate
        )){

            return false;
        }
    }


    return true;
}


}


std::vector<Triangle> triangulatePolygon(
    const Polygon& polygon
){
    std::vector<Triangle> triangles;


    const std::size_t n =
        polygon.vertices.size();


    if(n < 3){

        return triangles;
    }


    if(n == 3){

        triangles.push_back(
            Triangle{
                polygon.vertices[0],
                polygon.vertices[1],
                polygon.vertices[2]
            }
        );

        return triangles;
    }


    const bool counterClockwise =
        signedPolygonArea(
            polygon
        ) > 0.0;


    std::vector<std::size_t> indices;

    indices.reserve(
        n
    );


    for(std::size_t i = 0;
        i < n;
        ++i){

        indices.push_back(
            i
        );
    }


    while(indices.size() > 3){

        bool earFound =
            false;


        for(std::size_t i = 0;
            i < indices.size();
            ++i){

            if(!isEar(
                polygon,
                indices,
                i,
                counterClockwise
            )){

                continue;
            }


            const std::size_t count =
                indices.size();


            const std::size_t previousIndex =
                indices[
                    (i + count - 1)
                    % count
                ];

            const std::size_t currentIndex =
                indices[
                    i
                ];

            const std::size_t nextIndex =
                indices[
                    (i + 1)
                    % count
                ];


            triangles.push_back(
                Triangle{
                    polygon.vertices[
                        previousIndex
                    ],
                    polygon.vertices[
                        currentIndex
                    ],
                    polygon.vertices[
                        nextIndex
                    ]
                }
            );


            indices.erase(
                indices.begin()
                + static_cast<
                    std::ptrdiff_t
                >(i)
            );


            earFound =
                true;

            break;
        }


        if(!earFound){

            triangles.clear();

            return triangles;
        }
    }


    triangles.push_back(
        Triangle{
            polygon.vertices[
                indices[0]
            ],
            polygon.vertices[
                indices[1]
            ],
            polygon.vertices[
                indices[2]
            ]
        }
    );


    return triangles;
}