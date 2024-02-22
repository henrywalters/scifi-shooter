//
// Created by henry on 5/25/23.
//

#ifndef SCIFISHOOTER_WEAPONS_H
#define SCIFISHOOTER_WEAPONS_H

#include <hagame/common/weapons.h>
#include <hagame/core/entity.h>
#include "../common/itemDef.h"
#include "../scenes/editorRuntime.h"
#include "../components/explosive.h"

class GameWeapon : public hg::common::Weapon {
public:

    hg::Entity* source;
    EditorRuntime* runtime;
    WeaponItemDef* item;

    GameWeapon(hg::common::WeaponDef def, WeaponItemDef* itemDef):
        hg::common::Weapon(def),
        item(itemDef)
    {}

protected:

    void onFire(hg::Vec3 pos, hg::Vec3 dir) override;

private:

    void shootProjectile(hg::Vec3 pos, hg::Vec3 dir);
    void shootRaycast(hg::Vec3 pos, hg::Vec3 dir);

    Explosive* addExplosive(hg::Entity* entity);

    hg::Vec3 applySpread(hg::Vec3 dir) const;
};

#endif //SCIFISHOOTER_WEAPONS_H
