#pragma once
#include <Geode/modify/MenuLayer.hpp>

class $modify(HookedMenuLayer, MenuLayer) {
    bool init();
    void onGarage(cocos2d::CCObject* sender);
};
