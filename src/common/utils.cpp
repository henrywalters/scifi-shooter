//
// Created by henry on 8/23/23.
//
#include "utils.h"

#include <hagame/graphics/components/spriteSheetAnimator.h>
#include <hagame/common/components/topDownPlayerController.h>
#include <hagame/common/components/healthBar.h>

#include "../components/actor.h"

using namespace hg;
using namespace hg::graphics;

Entity* AddActor(hg::Scene* scene, hg::Vec3 pos, std::string texture, hg::Vec2 size, float maxSpeed, float maxHealth, float health) {
    auto entity = scene->entities.add();

    auto body = scene->entities.add(entity);
    body->name = "Body";

    auto inventory = scene->entities.add(entity);
    inventory->name = "Inventory";

    entity->transform.position = pos;

    auto anim = body->addComponent<components::SpriteSheetAnimator>();
    anim->size = size;

    auto actor = entity->addComponent<Actor>();
    actor->size = size;

    auto controller = entity->addComponent<TopDownPlayerController>();
    controller->maxSpeed = maxSpeed;

    //auto healthBar = entity->addComponent<hg::HealthBar>();

    //healthBar->health = health;
    //healthBar->maxHealth = maxHealth;
    //healthBar->offset = hg::Vec2(size.x() * -0.5, size.y());

    return entity;
}