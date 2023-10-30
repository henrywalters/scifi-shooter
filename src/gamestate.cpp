//
// Created by henry on 9/25/23.
//
#include "gamestate.h"

using namespace hg;

Vec2 GameState::randomTilemapPos() {
    bool isValid = false;
    hg::Vec2i idx;
    while (!isValid) {
        idx = Vec2i(random.integer(tilemap->min().x(), tilemap->max().x()), random.integer(tilemap->min().y(), tilemap->max().y()));
        if (!tilemap->getLayer(0)->has(idx)) {
            isValid = true;
        }
    }
    return tilemap->getPos(idx);
}

bool GameState::canSee(const Vec2 &pos, hg::Entity *entity) {
    auto collider = math::components::Collider::FromEntity(entity);
    if (!collider) {
        return false;
    }

    Cube box = collider->getBoundingCube();

    std::vector<Vec2> points = {
        box.pos.resize<2>(),
        (box.pos + box.size * 0.5).resize<2>(),
        (box.pos + box.size).resize<2>()
    };

    bool canSee = true;
    float t;

    for (const auto& point : points) {
        math::Ray ray(pos.resize<3>(), (point - pos).resize<3>());
        auto hit = tilemap->raycast(0, ray, t);
        if (hit.has_value()) {
            canSee = false;
            break;
        }
    }

    return canSee;
}

std::optional<Entity *> GameState::raycast(math::Ray ray, Vec2& pos, std::vector<hg::Entity*> ignore) {
    float tileT;
    float entityT;

    ray.direction.normalize();
    ray.direction *= 10000;

    auto hit = tilemap->raycast(0, ray, tileT);

    // This assumes there will always be a border on the tilemap. Maybe not ideal?
    if (!hit.has_value()) {
        return std::nullopt;
    }

    // We only need to check this far
    pos = ray.getPointOnLine(tileT).resize<2>();
    ray.direction *= tileT;

    auto hitEntity = entityMap.raycast(ray, entityT, ignore);

    if (hitEntity.has_value()) {
        pos = ray.getPointOnLine(entityT).resize<2>();
    }

    return hitEntity;
}


std::vector<hg::Entity *>
GameState::inVisibleRadius(hg::Vec2 pos, float radius, std::vector<hg::Entity *> ignore) {
    std::vector<Entity*> entities;
    for (const auto& entity : entityMap.getNeighbors(pos, Vec2(radius * 2, radius * 2))) {
        if ((entity->transform.position - pos.resize<3>()).magnitudeSq() <= radius * radius) {
            if (canSee(pos, entity)) {
                entities.push_back(entity);
            }
        }
    }
    return entities;
}
