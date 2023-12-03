//
// Created by henry on 8/23/23.
//
#include <hagame/common/weapons.h>
#include <hagame/graphics/debug.h>

#include <imgui.h>
#include <hagame/common/components/topDownPlayerController.h>
#include <hagame/math/components/rectCollider.h>
#include <hagame/graphics/components/spriteSheetAnimator.h>
#include <hagame/graphics/spriteSheet.h>
#include <hagame/math/lineIntersection.h>

#include "player.h"
#include "renderer.h"
#include "items.h"
#include "audio.h"

#include "../utils.h"
#include "../weapons.h"
#include "../components/actor.h"
#include "../components/item.h"
#include "../components/light.h"


using namespace hg;
using namespace hg::graphics;
using namespace hg::input::devices;

Player::Player(hg::graphics::Window *window, GameState* state):
        m_window(window),
        m_state(state)
{}

void Player::spawn(hg::Vec2 pos) {

    player = AddActor(scene, Vec3::Zero(), "player", Vec2(1, 1), 1);
    auto rect = player->addComponent<hg::math::components::RectCollider>();
    rect->rect = Rect(Vec2(-32, -32), Vec2(1, 1));
    player->addComponent<LightComponent>();

    scene->addToGroup(PLAYER_GROUP, player);
    player->name = "Player";
    // player->getComponent<Actor>()->weapons.selectWeapon(2);
    scene->getSystem<Renderer>()->setCameraPosition(player->transform.position);
    auto weapon = scene->getSystem<Items>()->get("handgun");
    pickUpWeapon((WeaponItemDef*) weapon, 36);

    auto audio = scene->getSystem<AudioSystem>();
    audio->addSource(player, AudioChannel::Sfx, ((WeaponItemDef*)weapon)->shootSound);

    player->getComponent<Actor>()->onDeath = [&]() {
        spawn(Vec2::Zero());
    };
}

void Player::pickUpWeapon(WeaponItemDef* weapon, int ammo) {
    auto actor = player->getComponent<Actor>();
    if (weapon->weaponType == WeaponType::Raycast) {
        auto added = actor->weapons.add<RaycastWeapon>(weapon->settings, weapon->settings.shotsPerSecond, weapon->spread * math::DEG2RAD);
        added->runtime = (Runtime*) scene;
        added->source = player;
        added->addAmmo(ammo);
    } else if (weapon->weaponType == WeaponType::Projectile) {
        auto added = actor->weapons.add<ProjectileWeapon>(weapon->settings);
        added->runtime = (Runtime*) scene;
        added->source = player;
        added->addAmmo(ammo);
    } else {
        throw std::runtime_error("UNSUPPORTED WEAPON TYPE: " + weapon->settings.name);
    }

    auto aPlayer = player->getComponentInChildren<components::SpriteSheetAnimator>()->player;

    std::vector<std::string> animationNames = {
        "idle",
        "move",
        "reload",
        "shoot",
    };

    std::vector<Animation*> animations;

    for (const auto &anim : animationNames) {
        std::string name = weapon->animations + "/" + anim;
        animations.push_back(aPlayer->addAnimation(name, getSpriteSheet(name)));
        std::cout << "Loaded animation: " << name << "\n";
    }

    // All weapon animations point back to idle mode
    for (int i = 1; i < animations.size(); i++) {
        aPlayer->connect(animations[i], animations[0]);
    }

    aPlayer->trigger(animations[0]);
}

void Player::onInit() {

    spawn(Vec2::Zero());

}

void Player::onUpdate(double dt) {

    if (m_state->paused) {
        return;
    }

    auto weapon = player->getComponent<Actor>()->weapons.getWeapon();
    auto weaponName = weapon->settings.name;

    auto aPlayer = player->getComponentInChildren<components::SpriteSheetAnimator>()->player;

    Renderer* renderer = scene->getSystem<Renderer>();

    auto rawMousePos = m_window->input.keyboardMouse.mouse.position;
    rawMousePos[1] = m_window->size()[1] - rawMousePos[1];
    m_mousePos = renderer->getMousePos(rawMousePos);

    Vec2 laserHit;
    Vec3 gunPos = player->position() + player->children()[0]->rotation().rotatePoint(Vec3(30, -15, 0));
    math::Ray laserRay(gunPos, (m_mousePos.resize<3>() - gunPos));
    m_state->raycast(laserRay, laserHit, {player});

    renderer->setLaserPointer(laserRay.origin, laserHit.resize<3>());

    renderer->setCrossHair(m_mousePos, 0.1, 0.15);

    auto actor = player->getComponent<Actor>();

    for (int i = 0; i < actor->weapons.getCount(); i++) {
        if (m_window->input.keyboardMouse.keyboard.numbersPressed[i + 1]) {
            actor->weapons.selectWeapon(i);
        }
    }

    if (m_window->input.keyboardMouse.keyboard.letters[KeyboardMouse::LetterIndex('R')]) {
        if (actor->weapons.getWeapon()->reload()) {
            aPlayer->triggerImmediately("player/" + weaponName + "/reload");
        }
    } else if (player->getComponent<TopDownPlayerController>()->velocity().magnitudeSq() > 0) {
        aPlayer->trigger("player/" + weaponName + "/move");
    }



    renderer->setCameraPosition(player->transform.position);
}

void Player::ui() {
    ImGui::Text("Player");
    ImGui::Text(("Position: " + player->transform.position.toString()).c_str());
    auto controller = player->getComponent<hg::TopDownPlayerController>();
    auto actor = player->getComponent<Actor>();
    auto health = player->getComponent<HealthBar>();
    ImGui::Text(("Mouse: " + m_mousePos.toString()).c_str());
    ImGui::Text(("Velocity: " + controller->velocity().toString()).c_str());
    ImGui::SliderFloat("Acceleration", &controller->acceleration, 0, 100);
    ImGui::SliderFloat("Deacceleration", &controller->deacceleration, 0, 100);
    ImGui::SliderFloat("Max Speed", &controller->maxSpeed, 0, 100);
    ImGui::SliderFloat("Health", &actor->health, 0, 100);
    health->health = actor->health;
}

void Player::onFixedUpdate(double dt) {

    if (m_state->paused) {
        return;
    }

//    if (m_window->input.keyboardMouse.mouse.left) {
//        auto entity = scene->entities.add();
//        auto light = entity->addComponent<LightComponent>();
//        light->color = m_state->randomColor();
//        light->attenuation = 4.0f;
//        light->dynamic = false;
//        entity->transform.position = m_mousePos.resize<3>();
//    }

    Items* items = scene->getSystem<Items>();

    auto pickUp = items->getItems(player->transform.position.resize<2>(), PICKUP_DISTANCE);

    for (const auto& item : pickUp) {
        if (std::find(m_pickingUp.begin(), m_pickingUp.end(), item) == m_pickingUp.end()) {
            //m_pickingUp.push_back(item);
            items->remove(item);
        }
    }

    for (const auto &item : m_pickingUp) {
        if (!item) { return; }
        item->transform.position += (player->position() - item->position()).normalized() * PICKUP_SPEED * dt;
        if ((item->position() - player->position()).magnitude() < 50) {
            m_pickingUp.erase(std::find(m_pickingUp.begin(), m_pickingUp.end(), item));
            items->remove(item);
        }
    }

    Renderer* renderer = scene->getSystem<Renderer>();

    auto playerActor = player->getComponent<Actor>();
    playerActor->direction = m_window->input.keyboardMouse.lAxis;
    playerActor->aimDirection = (m_mousePos - player->transform.position.resize<2>()).normalized();
    playerActor->attack = m_window->input.keyboardMouse.rTrigger;

    player->getComponent<Actor>()->direction = m_window->input.keyboardMouse.lAxis;

    float playerRotation = std::atan2(playerActor->aimDirection.y(), playerActor->aimDirection.x()) + M_PI / 2.0;

    player->children()[0]->transform.rotation = Quat(playerRotation - M_PI / 2, Vec3::Face());

    for (const auto& entity : m_state->entityMap.getNeighbors(player->position().resize<2>(), playerActor->size)) {
        Item* item = entity->getComponent<Item>();
        if (!item) {
            continue;
        }
    }


}

