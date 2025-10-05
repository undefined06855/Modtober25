#include "GJGarageLayer.hpp"
#include "../FunnySpriteManager.hpp"

// TODO: the current page isn't correct

void HookedGJGarageLayer::onModify(auto& self) {
    // TODO: find correct hook prios for all hooks
    (void)self.setHookPriorityAfterPost("GJGargeLayer::onSelect", "hiimjustin000.more_icons");
    (void)self.setHookPriorityAfterPost("GJGargeLayer::setupPage", "hiimjustin000.more_icons");
}

bool HookedGJGarageLayer::init() {
    auto fields = m_fields.self();
    fields->m_initializing = true;
    if (!GJGarageLayer::init()) return false;
    fields->m_initializing = false;

    // set m_iconPages to set the start pages for each tab
    auto& fsm = FunnySpriteManager::get();
    for (IconType type = IconType::Cube; type <= IconType::Jetpack; type = (IconType)(fmt::underlying(type) + 1)) {
        m_iconPages[type] = defaultPageForIconType(type);
        geode::log::debug("icon page for type {}: {}", fmt::underlying(type), m_iconPages[type]);
    }

    m_playerObject->setVisible(false);

    auto winWidth = cocos2d::CCDirector::get()->getWinSize().width;

    auto wrapper = CCNode::create();
    wrapper->setPosition({ winWidth / 2.f, 235.f });
    wrapper->setScale(15.f);
    addChild(wrapper);

    auto player = FunnySprite::create();
    player->updateForGamemode(GameManager::get()->m_playerIconType);
    wrapper->addChild(player);
    m_fields->m_player = player;

    return true;
}

int HookedGJGarageLayer::defaultPageForIconType(IconType type) {
    auto& fsm = FunnySpriteManager::get();

    // loop through all pages in the tab and see if the current icon is on
    // that tab
    bool found = false;

    auto& currentIcon = fsm.m_icon[type];
    for (int i = 0; i < GameManager::get()->countForType(type); i++) {
        auto& icon = fsm.m_iconsForIconType[type][i];
        if (icon.m_index == currentIcon.m_index && icon.m_type == currentIcon.m_iconType) {
            // yeah we got it
            geode::log::debug("default page for icon type {}: {} ({})", fmt::underlying(type), (i+1) / 36, m_fields->m_initializing ? "init" : "");
            return (i+1) / 36;
        }
    }

    geode::log::warn("did not find page for type {}!", fmt::underlying(type));
    return 0;
}

void HookedGJGarageLayer::setupPage(int page, IconType type) {
    auto fields = m_fields.self();
    if (fields->m_initializing) {
        page = defaultPageForIconType(type);
    }

    geode::log::debug("setting up page {} for icon type {}", page, fmt::underlying(type));

    // FIXME: this randomly crashes if you click death effects -> trails -> any tab
    GJGarageLayer::setupPage(page, type);

    if (type > IconType::Jetpack) return;

    // set up cursor for currently selected icon
    // normally done in gjgaragelayer getItems i believe but we dont call orig
    // there so it all works out

    auto& fsm = FunnySpriteManager::get();

    if (!fsm.m_icon.contains(type)) return;

    auto& selectedIcon = fsm.m_icon[type];

    m_cursor1->setVisible(false);

    for (auto btn : geode::cocos::CCArrayExt<CCMenuItemSpriteExtra*>(fields->m_lastItemsFetched)) {
        if (!btn->getParent()) return;

        auto index = btn->getTag();
        auto type = btn->m_iconType;
        if (index == selectedIcon.m_index && type == selectedIcon.m_iconType) {
            // this is the selected icon!
            auto pos = btn->getParent()->convertToWorldSpace(btn->getPosition());
            m_cursor1->setVisible(true);
            m_cursor1->setPosition(pos);
            return;
        }
    }
}

cocos2d::CCArray* HookedGJGarageLayer::getItems(int count, int page, IconType type, int current) {
    auto& fsm = FunnySpriteManager::get();

    if (type > IconType::Jetpack) {
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

    m_fields->m_lastItemsFetched = ret;

    return ret;
}

void HookedGJGarageLayer::onSelect(cocos2d::CCObject* sender) {
    auto cast = static_cast<CCMenuItemSpriteExtra*>(sender);

    if (m_iconType > IconType::Jetpack) {
        GJGarageLayer::onSelect(sender);
        return;
    }

    auto& fsm = FunnySpriteManager::get();

    fsm.m_icon[m_iconType] = IconChoiceInfo{
        .m_index = cast->getTag(),
        .m_iconType = cast->m_iconType
    };

    fsm.saveIconChoice();
    fsm.updateRenderedSprites();

    m_fields->m_player->updateForGamemode(m_iconType);

    GameManager::get()->m_playerIconType = m_iconType;

    auto pos = cast->getParent()->convertToWorldSpace(cast->getPosition());
    m_cursor1->setVisible(true);
    m_cursor1->setPosition(pos);
}
