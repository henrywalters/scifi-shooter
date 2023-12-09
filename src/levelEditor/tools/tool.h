//
// Created by henry on 12/7/23.
//

#ifndef SCIFISHOOTER_TOOL_H
#define SCIFISHOOTER_TOOL_H

#include <hagame/core/object.h>

class Tool : public hg::Object {
public:

    Tool(hg::Scene* scene):
        m_scene(scene)
    {}

    virtual void onInit() {}
    virtual void onUpdate(double dt) {};
    virtual void renderUI(double dt) {};
    virtual std::string getButtonLabel() = 0;
    virtual std::string getName() = 0;

protected:
    OBJECT_NAME(Tool)

    hg::Scene* m_scene;

};

#endif //SCIFISHOOTER_TOOL_H
