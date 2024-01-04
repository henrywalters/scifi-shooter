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

void ProjectileWeapon::onFire(hg::Vec3 pos, hg::Vec3 dir) {
    hg::utils::Random rand;
    auto projectileEntity = runtime->entities.add();
    projectileEntity->transform.position = pos;
    auto projectile = projectileEntity->addComponent<Projectile>();
    projectile->direction = dir;
    projectile->speed = speed;
    projectile->damage = rand.real<float>(settings.minDamage, settings.maxDamage);
    projectile->source = source;
    projectileEntity->addComponent<hg::graphics::ParticleEmitterComponent>(runtime->state()->particles.get(particles));
    // emitter->update(2.0);
}

void RocketWeapon::onFire(hg::Vec3 pos, hg::Vec3 dir) {
    hg::utils::Random rand;
    auto projectileEntity = runtime->entities.add();
    projectileEntity->transform.position = pos;

    auto projectile = projectileEntity->addComponent<Projectile>();
    projectile->direction = dir;
    projectile->speed = speed;
    projectile->damage = rand.real<float>(settings.minDamage, settings.maxDamage);
    projectile->source = source;
    projectileEntity->addComponent<hg::graphics::ParticleEmitterComponent>(runtime->state()->particles.get(particles));


    auto explosion = projectileEntity->addComponent<Explosive>();
    explosion->blastRadius = blastRadius;
    explosion->particles = explosionParticles;
    explosion->damage = explosionDamage;
}

void RaycastWeapon::onFire(hg::Vec3 pos, hg::Vec3 dir) {
    hg::utils::Random rand;

    auto aPlayer = source->getComponentInChildren<hg::graphics::components::SpriteSheetAnimator>()->player;
    aPlayer->triggerImmediately("player/" + this->settings.name + "/shoot");

    runtime->getSystem<AudioSystem>()->play(source);

    for (int i = 0; i < m_shotsPerCast; i++) {
        float spread = rand.real<float>(-m_spread, m_spread);
        auto newDir = hg::Quat(spread, hg::Vec3(0, 0, 1)).rotatePoint(dir) * 10000;
        hg::math::Ray ray(pos, newDir);

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

        hit = runtime->state()->raycastGeometry(ray);

        if (hit.has_value() && (!hasMinT || t < minT)) {
            auto hitDisplay = runtime->entities.add();
            hitDisplay->transform.position = hit.value().position;
            auto emitter = hitDisplay->addComponent<hg::graphics::ParticleEmitterComponent>(runtime->state()->particles.get("wall_hit"));
            emitter->emitter()->fire();
            continue;
        } else if (hasMinT) {
            auto hitDisplay = runtime->entities.add();
            hitDisplay->transform.position = ray.getPointOnLine(minT);

            auto hitActor = hitEntity->getComponent<Actor>();

            float damage = -rand.real<float>(settings.minDamage, settings.maxDamage);

            hitActor->addHealth(damage);
            hitEntity->getComponent<hg::HealthBar>()->health = hitActor->health;

            // hitNeighborEntity->getComponent<TopDownPlayerController>()->addVelocity(projectile->direction * 2000);

            auto emitter = hitDisplay->addComponent<hg::graphics::ParticleEmitterComponent>(runtime->state()->particles.get("blood_hit"));
            emitter->emitter()->fire();
            continue;
        }


    }
}
