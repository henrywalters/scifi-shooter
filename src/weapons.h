//
// Created by henry on 5/25/23.
//

#ifndef SCIFISHOOTER_WEAPONS_H
#define SCIFISHOOTER_WEAPONS_H

#include <hagame/common/weapons.h>
#include <hagame/core/entity.h>

class Runtime;

class ProjectileWeapon : public hg::common::Weapon {
public:

    std::string particles;
    hg::Entity* source;
    Runtime* runtime;
    float speed = 1000.0f;

    ProjectileWeapon(hg::common::WeaponDef def):
        hg::common::Weapon(def)
    {}

protected:

    void onFire(hg::Vec3 pos, hg::Vec3 dir) override;

};

class RocketWeapon : public hg::common::Weapon {
public:
    std::string particles;
    std::string explosionParticles;
    hg::Entity* source;
    Runtime* runtime;
    float speed = 1000.0f;
    float explosionDamage = 100.0f;
    float blastRadius = 2000.0f;

    RocketWeapon(hg::common::WeaponDef def):
        hg::common::Weapon(def)
    {}

protected:
    void onFire(hg::Vec3 pos, hg::Vec3 dir) override;
};

class RaycastWeapon : public hg::common::Weapon {
public:

    hg::Entity* source;
    Runtime* runtime;

    hg::Vec2 computeSpread() override {
        return hg::Vec2(m_spread, 0);
    }

    /*
     * shotsPerCast - number of raycasts sent per cast
     * spread - the direction will be rotated by +/- spread
     */
    RaycastWeapon(hg::common::WeaponDef def, int shotsPerCast, float spread):
        hg::common::Weapon(def),
        m_shotsPerCast(shotsPerCast),
        m_spread(spread)
    {}

protected:

    void onFire(hg::Vec3 pos, hg::Vec3 dir) override;

private:

    int m_shotsPerCast;
    float m_spread;

};

#endif //SCIFISHOOTER_WEAPONS_H
