//
// Created by henry on 12/10/23.
//

#ifndef SCIFISHOOTER_TEST_CPP
#define SCIFISHOOTER_TEST_CPP

#include <typeinfo>
#include <hagame/core/scene.h>
#include <hagame/graphics/windows.h>
#include <hagame/graphics/components/sprite.h>
#include "../../src/scifiGame.h"
#include "../../src/systems/renderer.h"
#include "../../src/scenes/runtime.h"

using namespace hg;

extern "C" {

#define GET_SYSTEM(Name) scene->getSystem<Name>(#Name)

    double initTime = 0;

    void TestScript_init(Scene* scene) {
        initTime = 0;
    }

    void TestScript_update(Scene* scene, double dt) {
        initTime += dt;
        for (const auto& entity : scene->entities.root->children()) {
            if (entity->name.substr(0, 8) == "RotateMe") {
                entity->transform.rotation = Quat(initTime * sin(initTime / 4) * 2, Vec3::Face());
                entity->transform.position = Vec3(tan(initTime) * 2, sin(initTime) * 2, 0);

                for (const auto& child : entity->children()) {
                    child->transform.rotation = Quat(dt, Vec3::Face()) * child->transform.rotation;
                }
            }

            if (entity->name == "BG") {
                auto quad = ((hg::Entity*) entity)->getComponent<hg::graphics::Quad>();
                quad->color = hg::graphics::Color((float)(sin(initTime / 4.0f) + 1.0f) / 2.0f, (float)(sin(initTime + M_PI / 2.0f) + 1.0f) / 2.0f, (float)(sin(initTime * 2) + 1.0f) / 2.0f);
            }
        }
    }

    void TestScript_close(Scene* scene) {

    }
};

#endif //SCIFISHOOTER_TEST_CPP
