//
// Created by henry on 8/23/23.
//
#include <hagame/common/weapons.h>
#include <hagame/graphics/debug.h>

#include <imgui.h>
#include <hagame/common/components/topDownPlayerController.h>
#include <hagame/math/components/rectCollider.h>

#include "player.h"
#include "renderer.h"

#include "../utils.h"
#include "../weapons.h"
#include "../components/actor.h"


using namespace hg;
using namespace hg::common;
using namespace hg::graphics;
using namespace hg::input::devices;

Player::Player(hg::graphics::Window *window, GameState* state):
        m_window(window),
        m_state(state)
{}

void Player::onInit() {

    player = AddActor(scene, Vec3::Zero(), "player", Vec2(64, 64), 500);
    auto rect = player->addComponent<hg::math::components::RectCollider>();
    rect->rect = Rect(Vec2(-32, -32), Vec2(64, 64));
    scene->addToGroup(PLAYER_GROUP, player);
    player->name = "Player";

    WeaponDef pistolDef;
    pistolDef.name = "SG-23";
    pistolDef.infinite = false;
    pistolDef.automatic = false;
    pistolDef.shotsPerSecond = 4;
    pistolDef.maxDamage = 50;
    pistolDef.minDamage = 30;
    auto projWeapon = player->getComponent<Actor>()->weapons.add<RaycastWeapon>(pistolDef, 1, M_PI / 64);
    projWeapon->runtime = (Runtime*) scene;
    projWeapon->source = player;
    projWeapon->addAmmo(64);
    projWeapon->reload();

    WeaponDef plasmaDef;
    plasmaDef.name = "Plasma Rifle";
    plasmaDef.infinite = true;
    plasmaDef.automatic = true;
    plasmaDef.shotsPerSecond = 14;
    plasmaDef.maxDamage = 50;
    plasmaDef.minDamage = 25;
    auto plasma = player->getComponent<Actor>()->weapons.add<ProjectileWeapon>(plasmaDef);
    plasma->particles = "plasma_ball";
    plasma->runtime = (Runtime*) scene;
    plasma->source = player;

    WeaponDef rocketDef;
    rocketDef.name = "Rocket Launcher";
    rocketDef.infinite = false;
    rocketDef.automatic = false;
    rocketDef.shotsPerSecond = 1;
    rocketDef.maxDamage = 0;
    rocketDef.clipSize = 1;
    rocketDef.maxAmmo = 10;
    rocketDef.maxDamage = 0;
    auto rocket = player->getComponent<Actor>()->weapons.add<RocketWeapon>(rocketDef);
    rocket->runtime = (Runtime*) scene;
    rocket->source = player;
    rocket->particles = "plasma_ball";
    rocket->explosionParticles = "explosion";
    rocket->blastRadius = 100.0f;
    rocket->explosionDamage = 500.0f;
    rocket->addAmmo(8);
    rocket->reload();

    WeaponDef shotgunDef;
    shotgunDef.name = "12 Gauge";
    shotgunDef.infinite = true;
    shotgunDef.automatic = false;
    shotgunDef.shotsPerSecond = 1;
    shotgunDef.maxDamage = 35;
    shotgunDef.minDamage = 5;
    auto shotgun = player->getComponent<Actor>()->weapons.add<RaycastWeapon>(shotgunDef, 6, M_PI / 24);
    shotgun->runtime = (Runtime*) scene;
    shotgun->source = player;

    WeaponDef arDef;
    arDef.name = "AK-57";
    arDef.infinite = true;
    arDef.automatic = true;
    arDef.shotsPerSecond = 100;
    arDef.maxDamage = 25;
    arDef.minDamage = 15;
    auto ar = player->getComponent<Actor>()->weapons.add<RaycastWeapon>(arDef, 1, M_PI / 36);
    ar->runtime = (Runtime*) scene;
    ar->source = player;

    player->getComponent<Actor>()->weapons.selectWeapon(2);

    scene->getSystem<Renderer>()->setCameraPosition(player->transform.position);
}

void Player::onUpdate(double dt) {

    if (m_state->paused) {
        return;
    }

    Renderer* renderer = scene->getSystem<Renderer>();

    renderer->setCrossHair(m_mousePos, 10, 15);

    auto actor = player->getComponent<Actor>();

    for (int i = 0; i < actor->weapons.getCount(); i++) {
        if (m_window->input.keyboardMouse.keyboard.numbersPressed[i + 1]) {
            actor->weapons.selectWeapon(i);
        }
    }

    if (m_window->input.keyboardMouse.keyboard.lettersPressed[KeyboardMouse::LetterIndex('R')]) {
        actor->weapons.getWeapon()->reload();
    }
}

void Player::ui() {
    ImGui::Text("Player");
    ImGui::Text(("Position: " + player->transform.position.toString()).c_str());
    auto controller = player->getComponent<hg::TopDownPlayerController>();
    auto actor = player->getComponent<Actor>();
    auto health = player->getComponent<HealthBar>();
    ImGui::Text(("Velocity: " + controller->velocity().toString()).c_str());
    ImGui::SliderFloat("Acceleration", &controller->acceleration, 0, 10000);
    ImGui::SliderFloat("Deacceleration", &controller->deacceleration, 0, 10000);
    ImGui::SliderFloat("Max Speed", &controller->maxSpeed, 0, 10000);
    ImGui::SliderFloat("Health", &actor->health, 0, 100);
    health->health = actor->health;
}

void Player::onFixedUpdate(double dt) {

    if (m_state->paused) {
        return;
    }

    Renderer* renderer = scene->getSystem<Renderer>();

    Vec2 laserHit;
    math::Ray laserRay(player->position(), (m_mousePos.resize<3>() - player->position()));
    m_state->raycast(laserRay, laserHit, {player});

    renderer->setLaserPointer(player->position(), laserHit.resize<3>());

    auto rawMousePos = m_window->input.keyboardMouse.mouse.position;
    rawMousePos[1] = m_window->size()[1] - rawMousePos[1];
    m_mousePos = renderer->getMousePos(rawMousePos);

    auto playerActor = player->getComponent<Actor>();
    playerActor->direction = m_window->input.keyboardMouse.lAxis;
    playerActor->aimDirection = (m_mousePos - player->transform.position.resize<2>()).normalized();
    playerActor->attack = m_window->input.keyboardMouse.rTrigger;

    player->getComponent<Actor>()->direction = m_window->input.keyboardMouse.lAxis;

    float playerRotation = std::atan2(playerActor->aimDirection.y(), playerActor->aimDirection.x());

    player->children()[0]->transform.rotation = Quat(playerRotation - M_PI / 2, Vec3::Face());

    renderer->setCameraPosition(player->transform.position);
}


