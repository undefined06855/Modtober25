#include "GameManager.hpp"
#include "../FunnySpriteManager.hpp"

int HookedGameManager::countForType(IconType type) {
    auto& fsm = FunnySpriteManager::get();

    if (fsm.m_wantsRealCountForType) return GameManager::countForType(type);

    return fsm.m_totalCountForTypes;
}
