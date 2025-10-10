#include "CharacterColorPage.hpp"
#include "../FunnySpriteManager.hpp"
#include "GJGarageLayer.hpp"

bool HookedCharacterColorPage::init() {
    if (!CharacterColorPage::init()) return false;

    static const std::vector<std::string_view> idsToHide = {
        "cube-icon",
        "", // ship handled separately
        "ball-icon",
        "ufo-icon",
        "wave-icon",
        "robot-icon",
        "spider-icon",
        "swing-icon"
    };

    auto fields = m_fields.self();

    fields->m_shipOrJetpack = nullptr;
    fields->m_robot = nullptr;
    fields->m_spider = nullptr;

    for (int i = 0; i < idsToHide.size(); i++) {
        auto id = idsToHide[i];
        if (id.empty()) continue;
        if (!m_mainLayer->getChildByID(id)) continue;

        auto gamemode = (FunnySpriteGamemode)i;
        auto child = m_mainLayer->getChildByID(id);

        child->setVisible(false);

        auto wrap = cocos2d::CCNode::create();
        wrap->setID("icon-wrap"_spr);
        wrap->setPosition(child->getPosition());
        m_mainLayer->addChild(wrap);

        auto funnySprite = FunnySprite::create();
        funnySprite->setID("funny-sprite");
        funnySprite->updateForGamemode(gamemode);
        funnySprite->setZOrder(2);
        funnySprite->addLimbs(gamemode);
        wrap->addChild(funnySprite);

        if (gamemode == FunnySpriteGamemode::Ufo) {
            auto passenger = FunnySprite::create();
            passenger->setID("funny-passenger-sprite");
            passenger->updateForGamemode(FunnySpriteGamemode::VehiclePassenger);
            wrap->addChild(passenger);
        }

        if (gamemode == FunnySpriteGamemode::Spider) fields->m_spider = funnySprite;
        if (gamemode == FunnySpriteGamemode::Robot) fields->m_robot = funnySprite;
    }

    if (auto ship = m_buttonMenu->getChildByID("ship-button")) {
        ship->getChildrenExt()[0]->setVisible(false);

        auto wrap = cocos2d::CCNode::create();
        wrap->setID("icon-wrap"_spr);
        wrap->setPosition(ship->getContentSize() / 2.f);
        ship->addChild(wrap);

        auto funnySprite = FunnySprite::create();
        funnySprite->setID("funny-sprite");
        funnySprite->updateForGamemode(FunnySpriteGamemode::Ship);
        funnySprite->setZOrder(2);
        wrap->addChild(funnySprite);

        auto passenger = FunnySprite::create();
        passenger->setID("funny-passenger-sprite");
        passenger->updateForGamemode(FunnySpriteGamemode::VehiclePassenger);
        wrap->addChild(passenger);

        fields->m_shipOrJetpack = funnySprite;
    }

    fields->m_showingJetpack = false;

    return true;
}

void HookedCharacterColorPage::updateIconColors() {
    CharacterColorPage::updateIconColors();

    FunnySpriteManager::get().updateRenderedSprites();

    // update limbs
    auto fields = m_fields.self();
    if (fields->m_robot) fields->m_robot->addLimbs(FunnySpriteGamemode::Robot);
    if (fields->m_spider) fields->m_spider->addLimbs(FunnySpriteGamemode::Spider);

    // update gjgaragelayer limbs
    auto garage = static_cast<HookedGJGarageLayer*>(cocos2d::CCScene::get()->getChildByType<GJGarageLayer>(0));
    if (garage) garage->updateLimbs();
}

void HookedCharacterColorPage::toggleShip(cocos2d::CCObject* sender) {
    CharacterColorPage::toggleShip(sender);

    auto fields = m_fields.self();

    if (fields->m_showingJetpack) {
        m_fields->m_shipOrJetpack->updateForGamemode(FunnySpriteGamemode::Ship);
    } else {
        m_fields->m_shipOrJetpack->updateForGamemode(FunnySpriteGamemode::Jetpack);
    }

    fields->m_showingJetpack = !fields->m_showingJetpack;
}
