//
// Created by henry on 12/27/23.
//

#ifndef SCIFISHOOTER_SHADERTOOL_H
#define SCIFISHOOTER_SHADERTOOL_H

#include <hagame/core/scene.h>
#include <hagame/graphics/primitives/quad.h>
#include <hagame/graphics/mesh.h>
#include "tool.h"

class ShaderTool : public Tool {
public:

    ShaderTool(hg::Scene* scene):
        Tool(scene)
    {}

    std::string getButtonLabel() override { return "Shader Tool"; }
    std::string getName() override { return "Shader Tool"; }

protected:

    void onInit() override;
    void renderUI(double dt) override;
    void onUpdate(double dt) override;
    void renderOverlay() override;

private:

    struct Shader {
        hg::utils::FileParts file;
        std::string contents;

        bool operator==(const Shader& shader) const {
            return file == shader.file;
        }
    };

    void acceptDragDrop();

    std::vector<std::unique_ptr<Shader>> m_shaders;

};

#endif //SCIFISHOOTER_SHADERTOOL_H
