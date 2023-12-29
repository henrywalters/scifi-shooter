//
// Created by henry on 12/29/23.
//
#include "events.h"

hg::Publisher<EventTypes, Event> *Events() {
    static std::unique_ptr<hg::Publisher<EventTypes, Event>> events;

    if (!events) {
        events = std::make_unique<hg::Publisher<EventTypes, Event>>();
    }

    return events.get();
}
