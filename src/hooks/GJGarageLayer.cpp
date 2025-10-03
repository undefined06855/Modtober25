#include "GJGarageLayer.hpp"
#include "../FunnySpriteManager.hpp"

// TODO: the current page isn't correct

cocos2d::CCArray* HookedGJGarageLayer::getItems(int count, int page, IconType type, int current) {
    auto& fsm = FunnySpriteManager::get();

    if (fmt::underlying(type) > fmt::underlying(IconType::Jetpack)) {
        return GJGarageLayer::getItems(fsm.realCountForType(type), page, type, current);
    }

    int offset = page * 36;
    cocos2d::CCArray* ret = cocos2d::CCArray::createWithCapacity(36);
    auto& icons = fsm.m_iconsForIconType[type];
    for (int i = offset; i < offset + 36; i++) {
        if (i >= icons.size()) break;
        auto icon = fsm.getIcon(icons[i], this);
        ret->addObject(icon);
    }

    return ret;
}

void HookedGJGarageLayer::onSelect(cocos2d::CCObject* sender) {
    auto cast = static_cast<CCMenuItemSpriteExtra*>(sender);
    auto& fsm = FunnySpriteManager::get();

    fsm.m_icon[m_iconType] = IconChoiceInfo{
        .m_index = cast->getTag(),
        .m_ofIconType = cast->m_iconType
    };

    fsm.saveIconChoice();
    fsm.updateRenderedSprites();

    // TODO: not calling orig makes it so the simpleplayer doesnt update
    // TODO: simpleplayer doesnt look visually correct (obviously...)
    // should we just completely replace the simpleplayer?
}
