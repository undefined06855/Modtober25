#include "GameManager.hpp"
#include "../FunnySpriteManager.hpp"

int HookedGameManager::countForType(IconType type) {
    auto& fsm = FunnySpriteManager::get();

    if (fsm.m_wantsRealCountForType) return GameManager::countForType(type);
    if (type > IconType::Jetpack) return GameManager::countForType(type);
    // TODO: maybe fix more icons popup?

    return fsm.m_totalCountForTypes;
}
