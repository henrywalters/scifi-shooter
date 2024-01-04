//
// Created by henry on 12/29/23.
//

#ifndef SCIFISHOOTER_EVENTS_H
#define SCIFISHOOTER_EVENTS_H

#include <hagame/utils/pubsub.h>
#include <hagame/core/entity.h>
#include <hagame/utils/file.h>
#include <hagame/core/scene.h>


enum class EventTypes {
    AddEntity,
    AddChild,
    AddChildTo,
    RemoveEntity,
    SelectEntity,
    DuplicateEntity,
    AddComponent,
    UpdateComponent,
    RemoveComponent,
    SelectAsset,
    NewLevel,
    LoadLevel,
    SaveLevel,
};

struct EntityEvent {
    hg::Entity* entity;
    hg::Entity* target;
};

struct ComponentEvent {
    hg::Entity* entity;
    std::string component;
    std::string field;
};

using AssetEvent = hg::utils::FileParts;

using LevelEvent = hg::Scene*;

struct Event {
    std::variant<EntityEvent, ComponentEvent, AssetEvent, LevelEvent> payload;
};

hg::Publisher<EventTypes, Event>* Events();


#endif //SCIFISHOOTER_EVENTS_H
