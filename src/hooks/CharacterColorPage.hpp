#pragma once
#include <Geode/modify/CharacterColorPage.hpp>
#include "../FunnySprite.hpp"

class $modify(HookedCharacterColorPage, CharacterColorPage) {
    struct Fields {
        FunnySprite* m_shipOrJetpack;
        FunnySprite* m_robot;
        FunnySprite* m_spider;
        bool m_showingJetpack;
    };

    bool init();

    void toggleShip(cocos2d::CCObject* sender);
    void updateIconColors();
};
