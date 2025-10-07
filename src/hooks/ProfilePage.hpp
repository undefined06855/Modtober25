#pragma once
#include <Geode/modify/ProfilePage.hpp>
#include "../FunnySprite.hpp"

class $modify(HookedProfilePage, ProfilePage) {
    struct Fields {
        FunnySprite* m_shipOrJetpack;
        bool m_showingJetpack;
    };

    void loadPageFromUserInfo(GJUserScore* info);
    void toggleShip(cocos2d::CCObject* sender);
};
