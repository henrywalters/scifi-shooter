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
        triangles(std::make_shared<hg::graphics::Mesh>()),
        mesh(triangles.get())
    {}

    float fov = M_PI * 2;
    hg::graphics::Color color = hg::graphics::Color::white();
    float attenuation = 0.005f;
    bool dynamic = true;

    std::shared_ptr<hg::graphics::Mesh> triangles;
    hg::graphics::MeshInstance mesh;

    // Compute all of the shadows based on the level & other collider geometry
    void computeMesh(std::vector<hg::math::Polygon> geometry);

protected:

    OBJECT_NAME(LightComponent)

};

HG_COMPONENT(Graphics, LightComponent)
HG_FIELD(LightComponent, float, attenuation)
HG_FIELD(LightComponent, float, fov)
HG_FIELD(LightComponent, bool, dynamic)
HG_FIELD(LightComponent, hg::graphics::Color, color)

#endif //SCIFISHOOTER_LIGHT_H
