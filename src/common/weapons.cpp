//
// Created by henry on 5/25/23.
//
#include "weapons.h"
#include "../scenes/runtime.h"

#include <hagame/graphics/components/particleEmitterComponent.h>
#include <hagame/common/components/healthBar.h>
#include <hagame/graphics/components/spriteSheetAnimator.h>
#include <hagame/utils/random.h>
#include "../components/projectile.h"
#include "../components/explosive.h"
#include "../systems/player.h"
#include "../systems/audio.h"

void GameWeapon::onFire(hg::Vec3 pos, hg::Vec3 dir) {
    if (item->projectile) {
        shootProjectile(pos, dir);
    } else {
        shootRaycast(pos, dir);
    }
}

void GameWeapon::shootProjectile(hg::Vec3 pos, hg::Vec3 dir) {
    for (int i = 0; i < item->shotsPerFire; i++) {
        hg::utils::Random rand;
        auto projectileEntity = runtime->entities.add();
        projectileEntity->transform.position = pos;
        auto projectile = projectileEntity->addComponent<Projectile>();
        projectile->direction = applySpread(dir);
        projectile->speed = item->projectileSettings.speed;
        projectile->damage = rand.real<float>(settings.minDamage, settings.maxDamage);
        projectile->source = source;
        projectileEntity->addComponent<hg::graphics::ParticleEmitterComponent>(runtime->state()->particles.get(item->projectileSettings.particles));

        if (item->explosive) {
            addExplosive(runtime->entities.add());
        }
    }
}

void GameWeapon::shootRaycast(hg::Vec3 pos, hg::Vec3 dir) {
    auto aPlayer = source->getComponentInChildren<hg::graphics::components::SpriteSheetAnimator>()->player;
    runtime->getSystem<AudioSystem>()->play(source);

    for (int i = 0; i < item->shotsPerFire; i++) {
        hg::math::Ray ray(pos, applySpread(dir));

        float t;
        float minT;
        bool hasMinT = false;
        hg::Entity* hitEntity;

        std::optional<hg::math::collisions::Hit> hit;

        runtime->entities.forEach<Actor>([&](Actor* actor, hg::Entity* entity) {
            if (actor->entity == source) {
                return;
            }

            hg::Rect rect(entity->transform.position.resize<2>() - actor->size * 0.5, actor->size);

            if (hg::math::collisions::checkRayAgainstRect(ray, rect, t) && (!hasMinT || t < minT)) {
                hasMinT = true;
                minT = t;
                hitEntity = entity;
            }
        });

        auto state = runtime->state();

        hit = state->raycastGeometry(ray, t);

        if (hit.has_value() && (!hasMinT || t < minT)) {
            auto hitDisplay = runtime->entities.add();
            hitDisplay->transform.position = hit.value().position;
            auto hitQuad = hitDisplay->addComponent<hg::graphics::Quad>();
            hitDisplay->transform.position[2] = 5;
            hitQuad->size = hg::Vec2(4.0 / 64.0, 4.0 / 64.0);
            hitQuad->color = hg::graphics::Color::blue();
            if (runtime->state()->particles.has(item->hitParticles)) {
                auto emitter = hitDisplay->addComponent<hg::graphics::ParticleEmitterComponent>(
                        runtime->state()->particles.get(item->hitParticles));
                emitter->emitter()->fire();
            } else {
                std::cout << "Missing Hit Particles: " << item->hitParticles << "\n";
            }
            addExplosive(runtime->entities.add())->explode(runtime, runtime->state());
            continue;
        } else if (hasMinT) {
            auto hitDisplay = runtime->entities.add();
            hitDisplay->transform.position = ray.getPointOnLine(minT);

            auto hitActor = hitEntity->getComponent<Actor>();

            float damage = -runtime->state()->random.real<float>(settings.minDamage, settings.maxDamage);

            hitActor->addHealth(damage);
            hitEntity->getComponent<hg::HealthBar>()->health = hitActor->health;

            auto emitter = hitDisplay->addComponent<hg::graphics::ParticleEmitterComponent>(runtime->state()->particles.get(item->hitParticles));
            emitter->emitter()->fire();
            addExplosive(runtime->entities.add())->explode(runtime, runtime->state());
            continue;
        }
    }
}

Explosive* GameWeapon::addExplosive(hg::Entity* entity) {
    auto explosion = entity->addComponent<Explosive>();
    explosion->blastRadius = item->explosiveSettings.blastRadius;
    explosion->particles = item->explosiveSettings.particles;
    explosion->damage = runtime->state()->random.real<float>(item->explosiveSettings.minDamage, item->explosiveSettings.maxDamage);
    return explosion;
}

hg::Vec3 GameWeapon::applySpread(hg::Vec3 dir) const {
    float angle = runtime->random()->real<float>(-item->spread, item->spread);
    return hg::Quat(angle, hg::Vec3(0, 0, 1)).rotatePoint(dir) * 10000;
}
