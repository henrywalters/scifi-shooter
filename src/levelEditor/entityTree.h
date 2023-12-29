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

    void render(hg::Scene* scene, hg::Entity* root);

    void renderTree(hg::Scene* scene, hg::Entity* entity, bool root = false);

private:

    bool m_dragging = false;
    hg::Entity* m_selected = nullptr;

};

#endif //SCIFISHOOTER_ENTITYTREE_H
