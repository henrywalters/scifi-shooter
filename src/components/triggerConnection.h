//
// Created by henry on 1/20/24.
//

#ifndef SCIFISHOOTER_TRIGGERCONNECTION_H
#define SCIFISHOOTER_TRIGGERCONNECTION_H

#include "connection.h"

class TriggerConnection : public Connection {
protected:

    OBJECT_NAME(TriggerConnection)

};

HG_COMPONENT(Game, TriggerConnection)

#endif //SCIFISHOOTER_TRIGGERCONNECTION_H
