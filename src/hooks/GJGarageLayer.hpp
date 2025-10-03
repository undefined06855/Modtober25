#pragma once
#include <Geode/modify/GJGarageLayer.hpp>

class $modify(HookedGJGarageLayer, GJGarageLayer) {
    cocos2d::CCArray* getItems(int count, int page, IconType type, int current);
    void onSelect(cocos2d::CCObject* sender);
};
