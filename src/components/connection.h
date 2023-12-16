//
// Created by henry on 12/5/23.
//

#ifndef SCIFISHOOTER_CONNECTION_H
#define SCIFISHOOTER_CONNECTION_H

#include <hagame/core/component.h>
#include <hagame/core/entity.h>

// The connection component is a way to link one or more components. For example, say you want to connect a levers signal to a door.
class Connection : public hg::Component {
public:

    hg::Entity* connectedTo;

protected:

    OBJECT_NAME(Connection)

};

HG_COMPONENT(Core, Connection)
HG_FIELD(Connection, hg::Entity*, connectedTo)

#endif //SCIFISHOOTER_CONNECTION_H