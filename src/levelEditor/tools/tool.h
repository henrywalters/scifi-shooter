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
    {
        onInit();
    }

    void update(hg::Vec2 mousePos, double dt) {
        m_mousePos = mousePos;
        if (m_open) {

            onUpdate(dt);
        }
    }

    void render(double dt) {
        if (m_open) {
            renderUI(dt);
            renderOverlay();
        }
    }

    void open() {
        m_open = true;
    }

    void close() {
        m_open = false;
    }

    virtual std::string getButtonLabel() = 0;
    virtual std::string getName() = 0;

protected:
    OBJECT_NAME(Tool)

    hg::Scene* m_scene;

    bool m_open = false;

    hg::Vec2 m_mousePos;

    virtual void onInit() {}
    virtual void onUpdate(double dt) {};
    virtual void renderUI(double dt) {};
    virtual void renderOverlay() {};
};

#endif //SCIFISHOOTER_TOOL_H
