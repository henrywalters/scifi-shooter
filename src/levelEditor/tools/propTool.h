//
// Created by henry on 1/7/24.
//

#ifndef SCIFISHOOTER_PROPTOOL_H
#define SCIFISHOOTER_PROPTOOL_H

#include "tool.h"
#include "selectedEntity.h"
#include "../../components/prop.h"
#include "../../systems/props.h"

class PropTool : public Tool {
public:

    PropTool(hg::Scene* scene):
        m_selected("Prop"),
        Tool(scene)
    {}

    std::string getButtonLabel() override { return "Prop Tool"; }
    std::string getName() override { return "Prop Tool"; }

protected:

    void onInit() override;
    void renderUI(double dt) override;

private:

    bool m_dirty = false;
    bool m_loadedProps = false;
    SelectedEntity<Prop> m_selected;
    std::vector<std::shared_ptr<PropDef>> m_props;
    PropDef m_selectedProp;
    std::optional<std::string> m_error;

    PropDef::State getState(hg::utils::uuid_t id);

};
#endif //SCIFISHOOTER_PROPTOOL_H
