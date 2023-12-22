//
// Created by henry on 12/19/23.
//

#ifndef SCIFISHOOTER_STARTPOINT_H
#define SCIFISHOOTER_STARTPOINT_H

#include <hagame/core/component.h>
#include <hagame/core/entity.h>

class StartPoint : public hg::Component {
protected:
    OBJECT_NAME(StartPoint)
};

HG_COMPONENT(Game, StartPoint)

#endif //SCIFISHOOTER_STARTPOINT_H
