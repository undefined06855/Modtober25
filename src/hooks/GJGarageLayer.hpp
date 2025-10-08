#pragma once
#include <Geode/modify/GJGarageLayer.hpp>
#include "../FunnySprite.hpp"

class $modify(HookedGJGarageLayer, GJGarageLayer) {
    struct Fields {
        FunnySprite* m_player;
        geode::Ref<cocos2d::CCArray> m_lastItemsFetched;
        bool m_initializing;
    };

    static void onModify(auto& self);

    bool init();

    void setupPage(int page, IconType type);

    cocos2d::CCArray* getItems(int count, int page, IconType type, int current);
    void onSelect(cocos2d::CCObject* sender);

    int defaultPageForIconType(IconType type);

    void onBack(cocos2d::CCObject* sender);
};
