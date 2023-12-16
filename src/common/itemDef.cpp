//
// Created by henry on 10/31/23.
//
#include "itemDef.h"

void WeaponItemDef::loadItem(hg::utils::Config config) {
    auto wepType = config.getRaw(tag, "weaponType");
    damage = config.get<float>(tag, "damage");
    spread = config.get<float>(tag, "spread") * (M_PI / 180.0); // Expect degrees
    animations = config.getRaw(tag, "animations");
    shootSound = config.getRaw(tag, "shootSound");

    bool validType = false;
    for (int i = 0; i < WEAPON_TYPE_NAMES.size(); i++) {
        if (wepType == WEAPON_TYPE_NAMES[i]) {
            weaponType = (WeaponType) i;
            validType = true;
        }
    }

    if (!validType) {
        throw std::runtime_error("Invalid weapon type: " + wepType);
    }

    if (weaponType == WeaponType::Projectile) {
        particles = config.getRaw(tag, "particles");
    }

    if (weaponType == WeaponType::Rocket) {
        particles = config.getRaw(tag, "particles");
        explosionParticles = config.getRaw(tag, "explosionParticles");
        blastRadius = config.get<float>(tag, "blastRadius");
    }

    settings.name = tag;
    settings.load(config);
}

void AmmoItemDef::loadItem(hg::utils::Config config) {
    weapon = config.getRaw(tag, "weapon");
    count = config.get<int>(tag, "count");
}

void HealthItemDef::loadItem(hg::utils::Config config) {
    restores = config.get<int>(tag, "restores");
}

void PropRequirementDef::loadItem(hg::utils::Config config) {

}
