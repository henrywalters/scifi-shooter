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

#include "../common/utils.h"
#include "../common/weapons.h"
#include "../components/actor.h"
#include "../components/item.h"
#include "../components/light.h"
#include "props.h"
#include "../scenes/editorRuntime.h"
#include "../levelEditor/events.h"

using namespace hg;
using namespace hg::graphics;
using namespace hg::input::devices;

Player::Player(hg::graphics::Window *window, GameState* state):
        m_window(window),
        m_state(state)
{}

void Player::spawn(hg::Vec2 pos) {
    player = AddActor(scene, Vec3::Zero(), "player", Vec2(1, 1), 3);
    player->transform.position = pos.resize<3>();
    auto light = player->addComponent<LightComponent>();
    light->color = Color(0.5f, 0.5f, 0.5f);
    light->attenuation = 0.5;
    //auto rect = player->addComponent<hg::math::components::RectCollider>();
    //rect->pos = Vec2(-0.5, -0.5);
    //rect->size = Vec2(1, 1);
    auto collider = player->addComponent<hg::math::components::CircleCollider>();
    collider->radius = 0.3;
    scene->addToGroup(PLAYER_GROUP, player);
    player->name = "Player";
    // player->getComponent<Actor>()->weapons.selectWeapon(2);
    scene->getSystem<Renderer>()->setCameraPosition(player->transform.position);
    // auto weapon = scene->getSystem<Items>()->get("handgun");
    // pickUpWeapon((WeaponItemDef*) weapon, 36);

    auto audio = scene->getSystem<AudioSystem>();

    player->getComponent<Actor>()->onDeath = [&]() {
        spawn(Vec2::Zero());
    };

    auto items = scene->getSystem<Items>();
    items->store().forEach([&](auto index, auto item) {
        if (item->type == ItemType::Weapon && ((WeaponItemDef*) item.get())->startingWeapon) {
            pickUpWeapon((WeaponItemDef*) item.get(), 12);
            auto audioSource = player->addComponent<hg::audio::AudioSource>();
            audioSource->channel = (int )AudioChannel::Sfx;
            audioSource->streamName = ((WeaponItemDef*)item.get())->shootSound;
        }
    });
}

void Player::pickUpWeapon(WeaponItemDef* weapon, int ammo) {
    auto actor = player->getComponent<Actor>();
    auto added = actor->weapons.add<GameWeapon>(weapon->settings, weapon);
    added->runtime = (EditorRuntime*) scene;
    added->source = player;
    added->addAmmo(ammo);
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
    Events()->subscribe(EventTypes::LoadLevel, [&](auto e) {
        player = nullptr;
    });
}

void Player::onUpdate(double dt) {

    if (!player) {
        return;
    }

    utils::Profiler::Start("Player::onUpdate");

    if (m_state->paused) {
        return;
    }

    auto weapon = (GameWeapon*) player->getComponent<Actor>()->weapons.getWeapon();
    std::string weaponName = weapon->item->tag;
    std::string animationName = weapon->item->animations;

    auto aPlayer = player->getComponentInChildren<components::SpriteSheetAnimator>()->player;

    Renderer* renderer = scene->getSystem<Renderer>();

    m_mousePos = renderer->getMousePos();

    Vec2 laserHit;
    Vec3 gunPos = player->position() + player->children()[0]->rotation().rotatePoint(Vec3(30, -15, 0));
    math::Ray laserRay(gunPos, (m_mousePos.resize<3>() - gunPos));
    m_state->raycast(laserRay, laserHit, {player});

    renderer->setLaserPointer(laserRay.origin, laserHit.resize<3>());
    renderer->setCrossHair(m_mousePos, 10, 15);

    auto actor = player->getComponent<Actor>();

    for (int i = 0; i < actor->weapons.getCount(); i++) {
        if (m_window->input.keyboardMouse.keyboard.numbersPressed[i + 1]) {
            actor->weapons.selectWeapon(i);
        }
    }

    if (weapon) {
        if (m_window->input.keyboardMouse.keyboard.letters[KeyboardMouse::LetterIndex('R')]) {
            if (actor->weapons.getWeapon()->reload()) {
                aPlayer->triggerImmediately(animationName + "/reload");
            }
        } else if (player->getComponent<TopDownPlayerController>()->velocity().magnitudeSq() > 0) {
            aPlayer->trigger(animationName + "/move");
        }
    }

    Props* props = scene->getSystem<Props>();
    auto propsInRange = props->getWithinRadius(player->transform.position.resize<2>(), INTERACT_DISTANCE);

    if (m_window->input.keyboardMouse.mouse.leftPressed) {
        for (const auto& prop : propsInRange) {
            std::cout << prop->getComponent<Prop>()->def->tag << "\n";
            prop->getComponent<Prop>()->toggle((hg::Entity*) player->getChildByName("Inventory"));
        }
    }


    renderer->setCameraPosition(player->transform.position);

    utils::Profiler::End("Player::onUpdate");
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

    if (!player) {
        return;
    }

    utils::Profiler::Start("Player::onFixedUpdate");

    if (m_state->paused) {
        return;
    }

    Items* items = scene->getSystem<Items>();

    auto pickUp = items->getWithinRadius(player->transform.position.resize<2>(), PICKUP_DISTANCE);

    for (const auto& item : pickUp) {
        if (std::find(m_pickingUp.begin(), m_pickingUp.end(), item) == m_pickingUp.end()) {
            //m_pickingUp.push_back(item);
            //items->remove(item);
            player->getChildByName("Inventory")->addChild(item);
            item->transform.position = Vec3::Zero();
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

    utils::Profiler::End("Player::onFixedUpdate");
}

void Player::despawn() {
    if (player) {
        scene->entities.remove(player);
        player = nullptr;
    }
}

