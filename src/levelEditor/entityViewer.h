//
// Created by henry on 12/5/23.
//

#ifndef SCIFISHOOTER_ENTITYVIEWER_H
#define SCIFISHOOTER_ENTITYVIEWER_H

#include <hagame/core/entity.h>
#include "imgui.h"
#include "componentExplorer.h"

class EntityViewer {
public:

    void render(hg::Entity* root);

    void renderTree(hg::Entity* entity);
    void renderEntity(hg::Entity* entity);

private:

    hg::Entity* m_selected = nullptr;

};

#endif //SCIFISHOOTER_ENTITYVIEWER_H
