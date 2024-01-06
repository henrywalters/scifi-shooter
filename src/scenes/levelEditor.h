//
// Created by henry on 12/5/23.
//

#ifndef SCIFISHOOTER_LEVELEDITOR_H
#define SCIFISHOOTER_LEVELEDITOR_H

#include <hagame/core/scene.h>
#include <hagame/graphics/tilemap.h>
#include <hagame/graphics/camera.h>
#include <hagame/graphics/resolution.h>
#include <hagame/graphics/renderPasses.h>
#include <hagame/graphics/window.h>
#include <hagame/core/assets.h>
#include <hagame/graphics/debug.h>
#include <hagame/graphics/particleEmitter.h>
#include <hagame/utils/pathfinding.h>
#include <hagame/core/entityMap.h>
#include <hagame/graphics/textBuffer.h>
#include <hagame/audio/player.h>
#include <hagame/core/cppScript.h>

#include "../levelEditor/entityTree.h"
#include "../levelEditor/entityViewer.h"
#include "../levelEditor/fileBrowser.h"
#include "../levelEditor/assetBrowser.h"

#include "../components/actor.h"

#include "../common/constants.h"
#include "../common/gamestate.h"
#include "../common/console.h"

#include "../levelEditor/tools/tool.h"
#include "editorRuntime.h"

const std::string LEVEL_EXT = ".hgs";

class LevelEditor : public hg::Scene {
public:

    const float BUTTON_HEIGHT = 24.0f;

    LevelEditor(hg::graphics::Window* window):
        m_window(window),
        hg::Scene()
    {}

protected:

    void onInit() override;
    void onUpdate(double dt) override;
    void onAfterUpdate() override;

private:

    EditorRuntime* m_runtime;
    hg::utils::MultiConfig m_runtimeData;
    bool m_playing = false;

    std::vector<std::unique_ptr<Tool>> m_tools;

    hg::Rect m_renderRect;

    EntityTree m_entityTree;
    hg::Entity* m_selectedEntity = nullptr;

    hg::graphics::Window* m_window;

    hg::Vec2 m_rawMousePos;
    hg::Vec2 m_mousePos;
    hg::Vec2 m_snapSize = hg::Vec2(20);

    float m_panSpeed = 5.0;
    float m_zoomSpeed = 0.05;

    hg::CppLibraryWrapper* m_scripts;

    FileBrowser m_browser;
    AssetBrowser m_assets;

    std::string m_saveFile = "";

    void renderUI(double dt);

    void renderSettingsWindow(double dt);
    void renderEntityWindow(double dt);
    void renderSelectedEntityWindow(double dt);
    void renderScriptWindow(double dt);
    void renderAssetWindow(double dt);
    void renderRenderWindow(double dt);

    void newScene();
    void saveAs();
    void saveToDisc();
    void loadFromDisc();

    void play();
    void pause();
    void reset(bool force = false);

    double m_elapsedTime = 0;
};

#endif //SCIFISHOOTER_LEVELEDITOR_H
