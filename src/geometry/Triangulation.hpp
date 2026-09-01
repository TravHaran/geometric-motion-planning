#pragma once

#include <vector>

#include "Polygon.hpp"
#include "Triangle.hpp"


std::vector<Triangle> triangulatePolygon(
    const Polygon& polygon
);