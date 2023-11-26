//
// Created by henry on 10/30/23.
//
#include <hagame/common/components/healthBar.h>
#include <hagame/graphics/components/particleEmitterComponent.h>
#include "explode.h"
#include "../../components/actor.h"

Explode::Explode(float damage, float radius, std::string particles):
    m_damage(damage),
    m_radius(radius),
    m_particles(particles)
{

}

hg::utils::bt::Status Explode::process(double dt, EnemyState *state, hg::utils::bt::data_context_t *ctx) {

    auto hitDisplay = state->game->scene->entities.add();
    hitDisplay->transform.position = state->entity->transform.position;
    auto particles = state->game->particles.get(m_particles);

    particles.speed.upper = m_radius;

    auto emitter = hitDisplay->addComponent<hg::graphics::ParticleEmitterComponent>(particles);
    emitter->emitter()->fire();

    for (const auto& entity : state->game->inVisibleRadius(state->entity->transform.position.resize<2>(), m_radius)) {
        if (entity->name == "Player" && state->game->params.invincible) {
            continue;
        }
        auto actor = entity->getComponent<Actor>();
        if (actor) {
            actor->addHealth(-m_damage);
            auto healthBar = entity->getComponent<hg::HealthBar>();
            if (healthBar) {
                healthBar->health = actor->health;
            }
        }
    }
    return hg::utils::bt::Status::Success;
}
