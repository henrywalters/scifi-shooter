//
// Created by henry on 10/31/23.
//
#include <hagame/core/assets.h>
#include "itemDef.h"
#include "../common/ui.h"
#include "../../thirdparty/imgui/misc/cpp/imgui_stdlib.h"

#define DIRTY(Code) if (Code) { m_dirty = true; }

void ItemDef::load(hg::utils::Config config) {
    auto idStr = std::to_string(id());
    type = config.get<int>(idStr, "type");
    config.getArray<float, 2>(idStr, "size", size.vector);
    tag = config.getRaw(idStr, "tag");
    description = config.getRaw(idStr, "description");
    texture = config.getRaw(idStr, "texture");

    loadItem(config);
}

void ItemDef::save(hg::utils::Config &config) {
    auto idStr = std::to_string(id());
    config.set(idStr, "type", (int) type);
    config.setRaw(idStr, "tag", tag);
    config.setRaw(idStr, "description", description);
    config.setRaw(idStr, "texture", texture);
    config.setArray<float, 2>(idStr, "size", size.vector);

    saveItem(config);

    m_dirty = false;
}

void ItemDef::ui() {

    DIRTY(ImGui::InputText("Tag", &tag));
    DIRTY(ImGui::InputFloat2("Size", size.vector));
    DIRTY(ImGui::InputText("description", &description));
    DIRTY(ui::SelectTexture("Texture", texture));
    onUi();
}

void WeaponItemDef::saveItem(hg::utils::Config &config) {
    auto idStr = std::to_string(id());
    config.set(idStr, "projectile", projectile);
    config.set(idStr, "explosive", explosive);
    config.set(idStr, "spread", spread);
    config.set(idStr, "projectile.speed", projectileSettings.speed);
    config.set(idStr, "startingWeapon", startingWeapon);
    config.setRaw(idStr, "animations", animations);
    config.setRaw(idStr, "shootSound", shootSound);
    config.setRaw(idStr, "hitSound", hitSound);
    config.setRaw(idStr, "hitParticles", hitParticles);
    config.set(idStr, "shotsPerFire", shotsPerFire);

    if (projectile) {
        config.setRaw(idStr, "projectileParticles", projectileSettings.particles);
        config.set(idStr, "projectile.speed", projectileSettings.speed);
    }

    if (explosive) {
        config.setRaw(idStr, "explosive.sound", explosiveSettings.sound);
        config.setRaw(idStr, "explosive.particles", explosiveSettings.particles);
        config.set(idStr, "explosive.blastRadius", explosiveSettings.blastRadius);
        config.set(idStr, "explosive.minDamage", explosiveSettings.minDamage);
        config.set(idStr, "explosive.maxDamage", explosiveSettings.maxDamage);
    }

    settings.id = id();
    settings.save(config);
}

void WeaponItemDef::loadItem(hg::utils::Config config) {

    auto idStr = std::to_string(id());

    projectile = config.get<bool>(idStr, "projectile");
    explosive = config.get<bool>(idStr, "explosive");
    if (explosive) {
        explosiveSettings.blastRadius = config.get<float>(idStr, "explosive.blastRadius");
        explosiveSettings.particles = config.getRaw(idStr, "explosive.particles");
        explosiveSettings.sound = config.getRaw(idStr, "explosive.sound");
        explosiveSettings.minDamage = config.get<float>(idStr, "explosive.minDamage");
        explosiveSettings.maxDamage = config.get<float>(idStr, "explosive.maxDamage");
    }
    if (projectile) {
        projectileSettings.speed = config.get<float>(idStr, "projectile.speed");
        projectileSettings.particles = config.getRaw(idStr, "projectile.particles");
    }
    shotsPerFire = config.get<int>(idStr, "shotsPerFire");
    spread = config.get<float>(idStr, "spread"); // Expect degrees
    animations = config.getRaw(idStr, "animations");
    shootSound = config.getRaw(idStr, "shootSound");
    hitSound = config.getRaw(idStr, "hitSound");
    shotsPerFire = config.get<int>(idStr, "shotsPerFire");
    startingWeapon = config.get<bool>(idStr, "startingWeapon");

    hitParticles = config.getRaw(idStr, "hitParticles");

    settings.id = id();
    settings.load(config);
}

void WeaponItemDef::onUi() {

    ImGui::SeparatorText("Weapon Settings");

    DIRTY(ImGui::Checkbox("Starting Weapon?", &startingWeapon));
    DIRTY(ImGui::Checkbox("Projectile?", &projectile));
    DIRTY(ImGui::Checkbox("Explosive?", &explosive));
    DIRTY(ImGui::Checkbox("Automatic?", &settings.automatic));
    DIRTY(ImGui::Checkbox("Chambered?", &settings.chambered));
    DIRTY(ImGui::Checkbox("Infinite?", &settings.infinite));
    if (!settings.infinite) {
        DIRTY(ImGui::InputInt("Clip Size", &settings.clipSize));
        DIRTY(ImGui::Checkbox("Limit Ammo?", &settings.limitAmmo));
        if (settings.limitAmmo) {
            DIRTY(ImGui::InputInt("Max Ammo", &settings.maxAmmo));
        }
    }
    if (settings.automatic) {
        DIRTY(ImGui::InputInt("Shots / Second", &settings.shotsPerSecond));
    }
    DIRTY(ImGui::InputInt("Shots / Fire", &shotsPerFire));
    DIRTY(ImGui::InputFloat("Min Damage", &settings.minDamage));
    DIRTY(ImGui::InputFloat("Max Damage", &settings.maxDamage));
    DIRTY(ImGui::InputFloat("Spread", &spread));

    DIRTY(ui::SelectParticle("Hit Particles", hitParticles));
    DIRTY(ui::SelectWav("Shoot Sound", shootSound));
    DIRTY(ui::SelectWav("Hit Sound", hitSound));
    DIRTY(ImGui::InputText("Animations", &animations));

    if (projectile) {
        DIRTY(ImGui::InputFloat("Velocity m/s", &projectileSettings.speed));
        DIRTY(ui::SelectParticle("Projectile Particles", projectileSettings.particles));
    }

    if (explosive) {
        ImGui::SeparatorText("Explosive Settings");
        DIRTY(ImGui::InputFloat("Blast Radius", &explosiveSettings.blastRadius));
        DIRTY(ui::SelectParticle("Explosive Particles", explosiveSettings.particles));
        DIRTY(ui::SelectWav("Explosive Sound", explosiveSettings.sound));
        DIRTY(ImGui::InputFloat("Min Damage", &explosiveSettings.minDamage));
        DIRTY(ImGui::InputFloat("Max Damage", &explosiveSettings.maxDamage));
    }

}

void AmmoItemDef::loadItem(hg::utils::Config config) {
    auto idStr = std::to_string(id());
    weapon = config.getRaw(idStr, "weapon");
    count = config.get<int>(idStr, "count");
}

void AmmoItemDef::saveItem(hg::utils::Config &config) {
    auto idStr = std::to_string(id());
    config.setRaw(idStr, "weapon", weapon);
    config.set(idStr, "count", count);
}

void HealthItemDef::loadItem(hg::utils::Config config) {
    auto idStr = std::to_string(id());
    restores = config.get<int>(idStr, "restores");
}

void HealthItemDef::saveItem(hg::utils::Config &config) {
    auto idStr = std::to_string(id());
    config.set(idStr, "restores", restores);
}

void HealthItemDef::onUi() {
    ImGui::SeparatorText("Health Settings");
    DIRTY(ImGui::InputInt("Restores", &restores, 1));
}
