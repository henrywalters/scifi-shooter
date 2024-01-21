//
// Created by henry on 1/7/24.
//

#ifndef SCIFISHOOTER_SELECTEDENTITY_H
#define SCIFISHOOTER_SELECTEDENTITY_H

#include <hagame/utils/pubsub.h>
#include "tool.h"
#include "../events.h"

enum class SelectedEntityEvents {
    Selected,
    Unselected,
};

template <typename ComponentType>
class SelectedEntity {
public:

    hg::EventEmitter<SelectedEntityEvents> events;

    SelectedEntity(std::string componentName);

    bool hasSelected() const;
    hg::Entity* entity() const;
    ComponentType* component() const;
    void select(hg::Entity* entity);
    void unselect();

private:

    std::string m_componentName;
    bool m_hasSelected = false;
    hg::Entity* m_selected;

};



template<typename ComponentType>
SelectedEntity<ComponentType>::SelectedEntity(std::string componentName):
    m_componentName(componentName)
{
    Events()->subscribe(EventTypes::SelectEntity, [&](const auto& e) {
        EntityEvent payload = std::get<EntityEvent>(e.payload);
        if (payload.entity->hasComponent<ComponentType>()) {
            select(payload.entity);
        } else {
            unselect();
        }
    });

    Events()->subscribe(EventTypes::RemoveEntity, [&](const auto& e) {
        EntityEvent payload = std::get<EntityEvent>(e.payload);
        if (payload.entity == m_selected) {
            unselect();
        }
    });

    Events()->subscribe(EventTypes::AddComponent, [&](const auto& e) {
        ComponentEvent payload = std::get<ComponentEvent>(e.payload);
        if (payload.component == m_componentName) {
            select(payload.entity);
        }
    });

    Events()->subscribe(EventTypes::RemoveComponent, [&](const auto& e) {
        ComponentEvent payload = std::get<ComponentEvent>(e.payload);
        if (payload.component == m_componentName) {
            unselect();
        }
    });

    Events()->subscribe(EventTypes::NewLevel, [&](const auto& e) {
        unselect();
    });

    Events()->subscribe(EventTypes::LoadLevel, [&](const auto& e) {
        hg::Scene* scene = std::get<hg::Scene*>(e.payload);
        if (hasSelected() && !scene->entities.exists(m_selected->id())) {
            unselect();
        }
    });
}

template<typename ComponentType>
void SelectedEntity<ComponentType>::unselect() {
    m_hasSelected = false;
    m_selected = nullptr;
    std::cout << "UNSELECTED\n";
    events.emit(SelectedEntityEvents::Unselected);
}

template<typename ComponentType>
void SelectedEntity<ComponentType>::select(hg::Entity *entity) {
    m_hasSelected = true;
    m_selected = entity;
    std::cout << "SELECTED " << m_selected->name << "\n";
    events.emit(SelectedEntityEvents::Selected);
}

template<typename ComponentType>
ComponentType *SelectedEntity<ComponentType>::component() const {
    return m_hasSelected ? m_selected->getComponent<ComponentType>() : nullptr;
}

template<typename ComponentType>
hg::Entity *SelectedEntity<ComponentType>::entity() const {
    return m_hasSelected ? m_selected : nullptr;
}

template<typename ComponentType>
bool SelectedEntity<ComponentType>::hasSelected() const {
    return m_hasSelected;
}

#endif //SCIFISHOOTER_SELECTEDENTITY_H
