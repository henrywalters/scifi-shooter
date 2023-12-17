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
                entity->transform.rotation = Quat(initTime * -1, Vec3::Face());

                entity->transform.position = Vec3(cos(initTime) * 10, sin(initTime) * 10, 0);
            }
        }
    }

    void TestScript_close(Scene* scene) {

    }
};

#endif //SCIFISHOOTER_TEST_CPP
