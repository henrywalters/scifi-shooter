//
// Created by henry on 12/3/23.
//

#ifndef SCIFISHOOTER_WORLDOBJECTS_H
#define SCIFISHOOTER_WORLDOBJECTS_H

#include <hagame/core/system.h>
#include <hagame/core/scene.h>
#include <hagame/core/entityMap.h>
#include <hagame/utils/store.h>
#include <hagame/core/component.h>
#include "../common/worldObjectDef.h"

template <typename KeyType, IsWorldObjectDef ObjectType, hg::IsComponent ComponentType>
class WorldObjects : public hg::System {
public:

    WorldObjects():
        m_map(TILE_SIZE)
    {}

    virtual void load(hg::utils::MultiConfig config) = 0;
    virtual void save(hg::utils::MultiConfig& config) = 0;

    // Get all objects with the radius of some position
    std::vector<hg::Entity*> getWithinRadius(hg::Vec2 pos, float radius) {
        std::vector<hg::Entity*> items;
        for (const auto& entity : m_map.getNeighbors(pos, hg::Vec2(radius * 2, radius * 2))) {
            if ((entity->transform.position.resize<2>() - pos).magnitude() <= radius) {
                items.push_back(entity);
            }
        }
        return items;
    }

    ObjectType* get(KeyType key) {
        return m_store.get(key).get();
    }

    hg::Entity* spawn(ObjectType* def, hg::Vec2 pos) {
        auto entity = scene->entities.add();
        auto component = entity->addComponent<ComponentType>();
        component->def = def;
        entity->transform.position = pos.resize<3>();
        m_map.insert(pos, def->size, entity);
        onSpawn(entity);
        return entity;
    }

    void remove(hg::Entity* entity) {
        auto object = entity->getComponent<ComponentType>();
        if (!object) {
            return;
        }
        scene->entities.remove(entity);
    }

    void onBeforeUpdate() override {
        m_map.clear();
        scene->entities.forEach<ComponentType>([&](ComponentType* comp, hg::Entity* entity) {
            if (!comp->def) { return; }
            m_map.insert(entity->position().resize<2>(), comp->def->size, entity);
        });
    }

    hg::utils::Store<KeyType, std::shared_ptr<ObjectType>>& store() {
        return m_store;
    }

protected:

    hg::EntityMap2D m_map;
    hg::utils::Store<KeyType, std::shared_ptr<ObjectType>> m_store;

    virtual void onSpawn(hg::Entity* entity) {}

};

#endif //SCIFISHOOTER_WORLDOBJECTS_H
