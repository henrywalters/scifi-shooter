//
// Created by henry on 12/13/23.
//

#ifndef SCIFISHOOTER_FILEBROWSER_H
#define SCIFISHOOTER_FILEBROWSER_H

#include <filesystem>

#include <hagame/utils/pubsub.h>
#include <hagame/utils/config.h>
#include <hagame/utils/file.h>
#include "imgui.h"
#include "imfilebrowser.h"

class FileBrowser {
public:

    FileBrowser():
        m_config(hg::utils::Config::Parse(hg::utils::f_readLines("file_browser.ini")))
    {}

    void loadFile(std::function<void(std::filesystem::path)> onChoose, std::vector<std::string> filters = {});
    void loadFiles(std::function<void(std::vector<std::filesystem::path>)> onChoose, std::vector<std::string> filters = {});
    void saveFile(std::function<void(std::filesystem::path)> onChoose, std::vector<std::string> filters = {});
    void setPwd(std::string pwd);

    void render();

private:

    hg::utils::Config m_config;

    std::unique_ptr<ImGui::FileBrowser> m_files;
    bool m_browsingFiles = false;
    bool m_choosingMany = false;
    std::function<void(std::filesystem::path)> m_onChooseOne;
    std::function<void(std::vector<std::filesystem::path>)> m_onChooseMany;
};

#endif //SCIFISHOOTER_FILEBROWSER_H
