//
// Created by henry on 12/8/23.
//
#include <hagame/math/aliases.h>
#include <hagame/graphics/color.h>
#include "componentExplorer.h"

std::optional<std::string> componentExplorer() {

    std::optional<std::string> out = std::nullopt;

    if (ImGui::BeginPopup(COMPONENT_EXPLORER.c_str())) {
        for (const auto& category : hg::ComponentFactory::GetCategories()) {
            ImGui::SeparatorText(category.c_str());
            for (const auto& component : hg::ComponentFactory::GetCategory(category)) {
                if (ImGui::Selectable(component.name.c_str())) {
                    out = component.name;
                }
            }
        }
        ImGui::EndPopup();
    }

    if (out.has_value()) {
        ImGui::CloseCurrentPopup();
    }

    return out;
}

#define PRIMITIVE_FIELD(Type, Func) if (field.type == #Type) {                  \
    Type initial = std::any_cast<Type>(value);                                  \
    ImGui::Func(field.field.c_str(), &initial);                                 \
    if (std::any_cast<Type>(field.getter(component)) != initial) {               \
        field.setter(component, initial);                                       \
        return true;                                                            \
    } else {                                                                     \
        return false;                                                           \
    }                                                                           \
}




bool editComponentField(hg::Component* component, hg::ComponentFactory::ComponentField field) {
    auto value = field.getter(component);

    PRIMITIVE_FIELD(bool, Checkbox)
    PRIMITIVE_FIELD(int, InputInt)
    PRIMITIVE_FIELD(float, InputFloat)
    PRIMITIVE_FIELD(double, InputDouble)

    if (field.type == "std::string") {
        std::string initial = std::any_cast<std::string>(value);
        ImGui::InputText(field.field.c_str(), initial.data(), 500);
        if (std::any_cast<std::string>(value) != initial) {
            field.setter(component, initial);
            return true;
        } else {
            return false;
        }
    }

    if (field.type == "hg::graphics::Color") {
        auto initial = std::any_cast<hg::graphics::Color>(value);
        ImGui::ColorPicker4(field.field.c_str(), initial.vector);
        if (std::any_cast<hg::graphics::Color>(value) != initial) {
            field.setter(component, initial);
            return true;
        } else {
            return false;
        }
    }

    ImGui::Text(("Unsupported Type: " + field.type).c_str());
    return false;
}
