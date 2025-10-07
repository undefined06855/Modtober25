#include "ProfilePage.hpp"

void HookedProfilePage::loadPageFromUserInfo(GJUserScore* info) {
    ProfilePage::loadPageFromUserInfo(info);

    auto menu = m_mainLayer->getChildByID("player-menu");
    if (!menu) return;

    auto fields = m_fields.self();

    fields->m_shipOrJetpack = nullptr;
    fields->m_showingJetpack = false;

    for (int i = 0; i < menu->getChildrenCount(); i++) {
        auto child = menu->getChildByIndex(i);
        for (auto childchild : child->getChildrenExt()) {
            childchild->setVisible(false);
        }

        // add our funnysprite instead of the simpleplayer
        auto gamemode = (FunnySpriteGamemode)i;
        auto funnySprite = FunnySprite::create();
        funnySprite->updateForGamemode(gamemode);
        funnySprite->setPosition(child->getContentSize() / 2.f);
        child->addChild(funnySprite);

        // add passenger for gamemodes that need it
        if (gamemode == FunnySpriteGamemode::Ship || gamemode == FunnySpriteGamemode::Ufo) {
            auto passenger = FunnySprite::create();
            passenger->updateForGamemode(FunnySpriteGamemode::CubePassenger);
            funnySprite->addChild(passenger);
        }

        if (gamemode == FunnySpriteGamemode::Ship) {
            fields->m_shipOrJetpack = funnySprite;
        }
    }
}

void HookedProfilePage::toggleShip(cocos2d::CCObject* sender) {
    ProfilePage::toggleShip(sender);

    auto fields = m_fields.self();

    if (fields->m_showingJetpack) {
        m_fields->m_shipOrJetpack->updateForGamemode(FunnySpriteGamemode::Ship);
    } else {
        m_fields->m_shipOrJetpack->updateForGamemode(FunnySpriteGamemode::Jetpack);
    }

    fields->m_showingJetpack = !fields->m_showingJetpack;
}
