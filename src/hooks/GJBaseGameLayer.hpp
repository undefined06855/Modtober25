#pragma once
#include <Geode/modify/GJBaseGameLayer.hpp>

class $modify(HookedGJBaseGameLayer, GJBaseGameLayer) {
    struct Fields {
        Fields();
        
        bool m_creatingPlayers;
    };

    void createPlayer();
};
