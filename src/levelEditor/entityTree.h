//
// Created by henry on 12/5/23.
//

#ifndef SCIFISHOOTER_ENTITYTREE_H
#define SCIFISHOOTER_ENTITYTREE_H

#include <hagame/core/entity.h>
#include <hagame/utils/pubsub.h>
#include "componentExplorer.h"
#include <hagame/core/scene.h>

class EntityTree {
public:

    enum class EventTypes {
        AddEntity,
        AddChild,
        AddChildTo,
        RemoveEntity,
        SelectEntity,
    };

    struct Event {
        hg::Entity* entity;
        hg::Entity* target;
    };

    hg::Publisher<EventTypes, Event> events;

    void render(hg::Scene* scene, hg::Entity* root);

    void renderTree(hg::Scene* scene, hg::Entity* entity, bool root = false);

private:

    hg::Entity* m_selected = nullptr;

};

#endif //SCIFISHOOTER_ENTITYTREE_H
