//
// Created by henry on 12/16/23.
//

#ifndef SCIFISHOOTER_ASSETBROWSER_H
#define SCIFISHOOTER_ASSETBROWSER_H

#include <hagame/graphics/texture.h>
#include <hagame/utils/file.h>
#include <hagame/utils/pubsub.h>
#include <vector>
#include <string>

class AssetBrowser {
public:

    void render();

private:

    hg::graphics::Texture* texture(hg::utils::FileParts fileParts);

    std::vector<std::string> m_path;

};

#endif //SCIFISHOOTER_ASSETBROWSER_H
