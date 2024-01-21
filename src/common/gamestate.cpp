//
// Created by henry on 9/25/23.
//
#include "gamestate.h"
#include "../components/item.h"
#include <hagame/graphics/components/sprite.h>
#include <hagame/math/collisions.h>
#include <hagame/graphics/components/tilemap.h>

using namespace hg;
using namespace hg::math;
using namespace hg::graphics;

Vec2 GameState::randomTilemapPos() {
    /*
    bool isValid = false;
    hg::Vec2i idx;
    while (!isValid) {
        idx = Vec2i(random.integer(tilemap->min().x(), tilemap->max().x()), random.integer(tilemap->min().y(), tilemap->max().y()));
        if (!tilemap->getLayer(0)->has(idx)) {
            isValid = true;
        }
    }
    return tilemap->getPos(idx);
     */
    return hg::Vec2::Zero();
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
        for (const auto& polygon : levelGeometry) {
            auto hit = math::collisions::checkRayAgainstPolygon(ray, polygon, t);
            if (hit.has_value()) {
                canSee = false;
                break;
            }
        }
    }

    return canSee;
}

std::optional<hg::math::collisions::Hit> GameState::raycastGeometry(hg::math::Ray ray, float& t) {
    bool hasMinT = false;
    std::optional<collisions::Hit> bestHit;
    float tmpT;

    for (const auto& polygon : levelGeometry) {
        auto hit = math::collisions::checkRayAgainstPolygon(ray, polygon, tmpT);
        if (hit.has_value()) {
            if (!hasMinT || tmpT < t) {
                t = tmpT;
                hasMinT = true;
                bestHit = hit;
            }
        }
    }

    return bestHit;
}


std::optional<Entity *> GameState::raycast(math::Ray ray, Vec2& pos, std::vector<hg::Entity*> ignore) {

    float entityT;
    float geometryT = 1.0f;

    ray.direction.normalize();
    ray.direction *= 100;

    float minT;
    bool hasMinT = false;

    for (const auto& polygon : levelGeometry) {
        auto hit = math::collisions::checkRayAgainstPolygon(ray, polygon, geometryT);
        if (hit.has_value()) {
            if (!hasMinT || geometryT < minT) {
                minT = geometryT;
                hasMinT = true;
            }
        }
    }

    // We only need to check this far
    pos = ray.getPointOnLine(geometryT).resize<2>();
    ray.direction *= geometryT;

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

hg::graphics::Color GameState::randomColor() {
    return hg::graphics::Color(random.integer(0, 255), random.integer(0, 255), random.integer(0, 255));
}

void GameState::gotoScene(hg::Game* game, std::string next) {
    if (game->scenes()->activeName().has_value()) {
        parentScene = game->scenes()->activeName().value();
    } else {
        parentScene = "";
    }
    game->scenes()->activate(next);
}

bool GameState::isColliding(hg::Rect rect) {
    for (const auto& polygon : levelGeometry) {
        if (math::collisions::checkRectAgainstPolygon(rect, polygon)) {
            return true;
        }
    }
    return false;
}

std::optional<collisions::Hit> GameState::isColliding(hg::Rect rect, hg::Vec3 vel, double dt) {
    bool hasHit = false;
    float minT;
    std::optional<collisions::Hit> bestHit = std::nullopt;
    float t;
    for (const auto& polygon : levelGeometry) {
        auto hit = collisions::checkRayAgainstPolygon(Ray(rect.getCenter().resize<3>(), vel * dt), polygon, t);
        if (hit.has_value() && t < 1.0f && (!hasHit || t < minT)) {
            hasHit = true;
            minT = t;
            bestHit = hit;
        }
    }
    return bestHit;
}

std::vector<hg::utils::PathFinding::Node> GameState::findTraversableNeighbors(hg::Vec2i index) {
    std::vector<hg::utils::PathFinding::Node> out;
    std::array<hg::Vec2i, 4> neighbors = {
        index + hg::Vec2i(1, 0),
        index + hg::Vec2i(0, 1),
        index + hg::Vec2i(-1, 0),
        index + hg::Vec2i(0, -1),
    };
    std::array<bool, 4> canTraverse = {true, true, true, true};
    scene->entities.forEach<hg::graphics::components::Tilemap>([&](auto tilemap, auto entity) {
        for (int i = 0; i < 4; i++) {
            if (tilemap->collide && tilemap->tiles.has(neighbors[i])) {
                canTraverse[i] = false;
            }
        }
    });

    for (int i = 0; i < 4; i++) {
        if (canTraverse[i]) {
            utils::PathFinding::Node neighbor;
            neighbor.position = neighbors[i];
            neighbor.cost = 0;
            out.push_back(neighbor);
        }
    }

    return out;
}

bool GameState::canTraverse(hg::Vec2i index) {
    bool canTraverse = true;
    scene->entities.forEach<hg::graphics::components::Tilemap>([&](auto tilemap, auto entity) {
        if (!canTraverse) {return;}
        for (int i = 0; i < 4; i++) {
            if (tilemap->collide && tilemap->tiles.has(index)) {
                canTraverse = false;
                return;
            }
        }
    });
    return canTraverse;
}

Vec2i GameState::getTileIndex(hg::Vec2 pos) {
    return pos.div(TILE_SIZE).floor().cast<int>();
}

Vec2 GameState::getTilePos(hg::Vec2i index) {
    return index.cast<float>().prod(TILE_SIZE);
}

