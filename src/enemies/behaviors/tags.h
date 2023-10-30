//
// Created by henry on 10/22/23.
//

#ifndef SCIFISHOOTER_TAGS_H
#define SCIFISHOOTER_TAGS_H

enum class BTags {
    Target,
    EntitiesInSight,
    PathEnd,
    Path,
};

const std::vector<std::string> BTagNames = {
        "Target",
        "Entities in Sight",
        "Path End",
        "Path"
};

#endif //SCIFISHOOTER_TAGS_H
