//
// Created by henry on 1/16/24.
//
#include <hagame/core/assets.h>
#include "ui.h"
#include "../../thirdparty/imgui/misc/cpp/imgui_stdlib.h"

using namespace hg;
using namespace std;

Entity * ui::SelectEntity(string label, Scene *scene, Entity* selected, string selectMessage, string deselectMessage) {
    std::string preview = selected ? selected->name : selectMessage;
    Entity* out = selected;
    if (ImGui::BeginCombo(label.c_str(), preview.c_str())) {
        if (selected && ImGui::Selectable(deselectMessage.c_str())) {
            out = nullptr;
        }
        scene->entities.forEach([&](auto other) {
            if (ImGui::Selectable(other->name.c_str(), selected && selected->id() == other->id())) {
                out = other;
            }
        });
        ImGui::EndCombo();
    }
    return out;
}

bool ui::SelectAsset(
        std::string extension,
        std::string directory,
        std::string label,
        string &asset,
        std::function<std::string(hg::utils::FileParts)> formatter,
        std::string selectMessage,
        std::string deselectMessage
) {
    bool dirty = false;
    std::string preview = asset != "" ? asset : selectMessage;
    if (ImGui::BeginCombo(label.c_str(), preview.c_str())) {
        if (asset != "" && ImGui::Selectable(deselectMessage.c_str())) {
            asset = "";
            dirty = true;
        }

        auto files = hg::utils::d_listFiles(directory, true);

        for (const auto& file : files) {
            auto parts = hg::utils::f_getParts(file);
            if (parts.extension == extension) {
                std::string formatted = formatter(parts);
                if (ImGui::Selectable(formatted.c_str())) {
                    asset = formatted;
                    dirty = true;
                }
            }
        }

        ImGui::EndCombo();
    }

    return dirty;
}

bool ui::SelectEnum(std::string label, std::vector<hg::utils::EnumElement> enums, utils::enum_t &selected) {
    bool dirty = false;
    std::string preview = enums[selected].label;
    if (ImGui::BeginCombo(label.c_str(), preview.c_str())) {
        ImGui::PushID(0);
        ImGui::PopID();
        for (int i = 0; i < enums.size(); i++) {
            ImGui::PushID(i + 1);
            if (ImGui::Selectable(enums[i].label.c_str(), selected == enums[i].key)) {
                selected = enums[i].key;
                dirty = true;
            }
            ImGui::PopID();
        };
        ImGui::EndCombo();
    }
    return dirty;
}

bool ui::SelectTexture(std::string label, string &asset) {
    ImGui::Image((ImTextureID) hg::getTexture(asset)->id, ImVec2(32, 32));
    ImGui::SameLine();
    bool dirty = ui::SelectAsset(
            "png",
            hg::ASSET_DIR + "textures/items/",
            label,
            asset,
            [](auto parts) { return hg::utils::s_replace(parts.path, hg::ASSET_DIR + "textures/", "") + parts.name; }
    );
    return dirty;
}

bool ui::SelectParticle(std::string label, string &asset) {
    return ui::SelectAsset(
        "hgp",
        hg::ASSET_DIR + "particles/",
        label,
        asset,
        [](auto parts) {
            return hg::utils::s_replace(parts.path, hg::ASSET_DIR, "") + parts.name;
        }
    );
}

bool ui::SelectWav(std::string label, string &asset) {
    return ui::SelectAsset(
        "wav",
        hg::ASSET_DIR + "audio/",
        label,
        asset,
        [](auto parts) {
            return hg::utils::s_replace(parts.path, hg::ASSET_DIR + "audio/", "") + parts.name;
        }
    );
}

bool ui::SelectSpriteSheet(std::string label, string &asset) {
    return ui::SelectAsset(
            "hgss",
            hg::ASSET_DIR + "textures/",
            label,
            asset,
            [](auto parts) {
                return hg::utils::s_replace(parts.path, hg::ASSET_DIR, "") + parts.name;
            }
    );
}
