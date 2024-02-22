//
// Created by henry on 1/21/24.
//

#ifndef SCIFISHOOTER_ITEMTOOL_H
#define SCIFISHOOTER_ITEMTOOL_H

#include "tool.h"
#include "selectedEntity.h"
#include "../../components/item.h"

class ItemTool : public Tool {
public:

    ItemTool(hg::Scene* scene):
        m_selected((Item{}).className()),
        Tool(scene)
    {}

    std::string getButtonLabel() override { return "Item Tool"; }
    std::string getName() override { return "Item Tool"; }

protected:

    void onInit() override;
    void renderUI(double dt) override;

private:

    bool m_initialized = false;
    SelectedEntity<Item> m_selected;
    std::vector<std::shared_ptr<ItemDef>> m_items;
    std::shared_ptr<ItemDef> m_selectedItem;
    std::optional<std::string> m_error;


};

#endif //SCIFISHOOTER_ITEMTOOL_H
