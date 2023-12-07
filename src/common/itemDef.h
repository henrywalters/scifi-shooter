//
// Created by henry on 10/31/23.
//

#ifndef SCIFISHOOTER_ITEMDEF_H
#define SCIFISHOOTER_ITEMDEF_H

#include <hagame/utils/config.h>
#include <hagame/utils/store.h>
#include <hagame/common/weapons.h>
#include <hagame/core/object.h>
#include "worldObjectDef.h"

enum class ItemType {
    Weapon,
    Ammo,
    Health,
    PropRequirement,
};

const std::vector<std::string> ITEM_TYPE_NAMES = {
    "Weapon",
    "Ammo",
    "Health",
    "PropRequirement",
};

class ItemDef : public WorldObjectDef {
public:
    std::string tag;
    ItemType type;
    hg::Vec2 size;
    std::string label;
    std::string description;
    std::string texture;

    virtual void loadItem(hg::utils::Config config) = 0;
};

enum class WeaponType {
    Projectile,
    Rocket,
    Raycast,
    Custom,
};

const std::vector<std::string> WEAPON_TYPE_NAMES = {
        "Projectile",
        "Rocket",
        "Raycast",
        "Custom",
};

class PropRequirementDef : public ItemDef {
public:

    void loadItem(hg::utils::Config config);

};

class WeaponItemDef : public ItemDef {
public:

    WeaponType weaponType;
    hg::common::WeaponDef settings;
    float damage;
    float spread;
    float speed;
    float blastRadius;
    std::string particles;
    std::string explosionParticles;
    std::string animations;
    std::string shootSound;

    void loadItem(hg::utils::Config config);
};

class AmmoItemDef : public ItemDef {
public:

    std::string weapon;
    int count;

    void loadItem(hg::utils::Config config);
};

class HealthItemDef : public ItemDef {
public:

    int restores;

    void loadItem(hg::utils::Config config);

};

#endif //SCIFISHOOTER_ITEMDEF_H
