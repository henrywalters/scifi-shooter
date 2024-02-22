//
// Created by henry on 2/18/24.
//

#ifndef SCIFISHOOTER_ENDPOINT_H
#define SCIFISHOOTER_ENDPOINT_H

#include <hagame/core/component.h>
#include <hagame/core/entity.h>

class EndPoint : public hg::Component {
protected:
    OBJECT_NAME(EndPoint)
};

HG_COMPONENT(Game, EndPoint)

#endif //SCIFISHOOTER_ENDPOINT_H
