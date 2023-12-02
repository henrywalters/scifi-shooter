//
// Created by henry on 12/2/23.
//

#ifndef SCIFISHOOTER_LIGHT_H
#define SCIFISHOOTER_LIGHT_H

#include <hagame/graphics/mesh.h>
#include <hagame/core/component.h>
#include <hagame/math/interval.h>
#include <hagame/graphics/color.h>
#include <hagame/graphics/tilemap.h>
#include <hagame/math/functions.h>

class LightComponent : public hg::Component {
public:

    LightComponent():
        mesh(&triangles)
    {}

    float fov = M_PI * 2;
    hg::graphics::Color color = hg::graphics::Color::white();
    float attenuation = 0.05f;
    bool dynamic = true;
    hg::graphics::Mesh triangles;
    hg::graphics::MeshInstance mesh;

    // Compute all of the shadows based on the level & other collider geometry
    void computeMesh(std::vector<hg::math::Polygon> geometry);

protected:
    OBJECT_NAME(LightComponent);
};

#endif //SCIFISHOOTER_LIGHT_H
