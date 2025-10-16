#pragma once
#include <Geode/modify/GameManager.hpp>

class $modify(HookedGameManager, GameManager) {
    int countForType(IconType type);

    void reloadAllStep5();
};
