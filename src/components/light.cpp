//
// Created by henry on 12/2/23.
//
#include <hagame/math/lineIntersection.h>
#include <hagame/graphics/debug.h>
#include "light.h"

using namespace hg;
using namespace hg::math;
using namespace hg::graphics;

void LightComponent::computeMesh(std::vector<Polygon> geometry) {

    if (geometry.size() == 0) { // If there is nothing to worry about, then we can just draw rectangle
        triangles = std::make_shared<primitives::Disc>(50000, 10);
        mesh.update(triangles.get());
        return;
    } else {
        triangles = std::make_shared<Mesh>();
    }

    Vec2 origin = entity->transform.position.resize<2>();
    std::vector<Vec2> points;

    triangles->vertices.clear();
    triangles->indices.clear();

    for (const auto& poly : geometry) {
        for (const auto& edge : poly) {
            points.push_back(edge.a);
            points.push_back(edge.b);
        }
    }

    std::sort(points.begin(), points.end(), [&](Vec2 a, Vec2 b) {
        Vec2 deltaA = a - origin;
        Vec2 deltaB = b - origin;
        float thetaA = std::atan2(deltaA.y(), deltaA.x());
        float thetaB = std::atan2(deltaB.y(), deltaB.x());
        return thetaA < thetaB;
    });

    std::vector<std::array<Vec2, 3>> endpoints;

    int pointIdx = 0;

    for (const auto& pt : points) {

        std::array<Vec2, 3> directions;
        std::array<Vec2, 3> hits;
        directions[1] = pt - origin;
        directions[0] = rotate(directions[1].normalized(), -0.00001) * 10000;
        directions[2] = rotate(directions[1].normalized(), 0.00001) * 10000;

        for (int i = 0; i < directions.size(); i++) {
            Vec2 bestPt;
            float distance = 0;
            bool hasBest = false;

            for (const auto& poly : geometry) {
                for (const auto& edge : poly) {
                    LineIntersection hit;
                    if (
                            linesIntersect(origin, origin + directions[i], edge.a, edge.b, hit)
                            ) {
                        if (hit.collinear() || hit.parallel()) {
                            continue;
                        }
                        if (!hasBest || (hit.pos() - origin).magnitudeSq() < distance) {
                            hasBest = true;
                            distance = (hit.pos() - origin).magnitudeSq();
                            bestPt = hit.pos();
                        }
                    }
                }
            }

            if (hasBest) {
                Debug::DrawLine(origin.x(), origin.y(), bestPt.x(), bestPt.y(), Color((float)(pointIdx++) / (3 * points.size()), 0.0f, 0.0f), 0.01);
                hits[i] = bestPt;
            }
        }

        endpoints.push_back(hits);
    }

    for (int i = 1; i <= endpoints.size(); i++) {

        Vec2 p1 = endpoints[i - 1][2];
        Vec2 p2 = endpoints[i % endpoints.size()][0];
        hg::graphics::Triangle tri(p1.resize<3>(), origin.resize<3>(), p2.resize<3>());

        tri.insert(triangles->vertices, triangles->indices);
    }

    mesh.update(triangles.get());
}
