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

#include <hagame/utils/enum.h>

ENUM(ItemType)
ENUM_VALUE(ItemType, Weapon)
ENUM_VALUE(ItemType, Ammo)
ENUM_VALUE(ItemType, Health)
ENUM_VALUE(ItemType, Key)

class ItemDef : public WorldObjectDef {
public:
    hg::utils::enum_t type;
    std::string tag;
    std::string description;
    std::string texture;

    void load(hg::utils::Config config);
    void save(hg::utils::Config& config);
    void ui();

    HG_GET(bool, dirty);

protected:

    bool m_dirty = false;

    OBJECT_NAME(ItemDef)

    virtual void loadItem(hg::utils::Config config) {}
    virtual void saveItem(hg::utils::Config& config) {}
    virtual void onUi() {}

};

struct ProjectileSettings {
    std::string particles;
    float speed;
};

struct ExplosiveSettings {
    float blastRadius = 0;
    std::string sound;
    std::string particles;
    float minDamage = 0;
    float maxDamage = 0;
};

class WeaponItemDef : public ItemDef {
public:

    WeaponItemDef() {
        this->type = ItemType::Weapon;
    }

    hg::common::WeaponDef settings;
    bool startingWeapon = false;
    bool projectile = false;
    bool explosive = false;
    float spread = 0;
    int shotsPerFire = 1;

    std::string hitParticles;
    std::string animations;
    std::string shootSound;
    std::string hitSound;

    ProjectileSettings projectileSettings;
    ExplosiveSettings explosiveSettings;

protected:

    OBJECT_NAME(WeaponItemDef)

    void loadItem(hg::utils::Config config) override;
    void saveItem(hg::utils::Config& config) override;
    void onUi() override;
};

class AmmoItemDef : public ItemDef {
public:

    AmmoItemDef() {
        this->type = ItemType::Ammo;
    }

    std::string weapon;
    int count;

protected:

    OBJECT_NAME(AmmoItemDef)

    void loadItem(hg::utils::Config config) override;
    void saveItem(hg::utils::Config& config) override;
};

class HealthItemDef : public ItemDef {
public:

    HealthItemDef() {
        this->type = ItemType::Health;
    }

    int restores;

protected:

    OBJECT_NAME(HealthItemDef)

    void loadItem(hg::utils::Config config) override;
    void saveItem(hg::utils::Config& config) override;

    void onUi() override;
};

class KeyItemDef : public ItemDef {
public:

    KeyItemDef() {
        this->type = ItemType::Key;
    }

protected:

    OBJECT_NAME(KeyItemDef)

};

ENUM_FACTORY(ItemType)
FACTORY_VALUE(ItemType, Weapon, ItemDef, WeaponItemDef)
FACTORY_VALUE(ItemType, Ammo, ItemDef, AmmoItemDef)
FACTORY_VALUE(ItemType, Health, ItemDef, HealthItemDef)
FACTORY_VALUE(ItemType, Key, ItemDef, KeyItemDef)

#endif //SCIFISHOOTER_ITEMDEF_H
