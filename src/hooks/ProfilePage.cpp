#include "ProfilePage.hpp"

void HookedProfilePage::loadPageFromUserInfo(GJUserScore* info) {
    ProfilePage::loadPageFromUserInfo(info);

    if (!info->isCurrentUser()) return;

    auto menu = m_mainLayer->getChildByID("player-menu");
    if (!menu) return;

    for (int i = 0; i < menu->getChildrenCount(); i++) {
        auto child = menu->getChildByIndex(i);
        for (auto childchild : child->getChildrenExt()) {
            childchild->setVisible(false);
        }

        auto wrap = cocos2d::CCNode::create();
        wrap->setID("icon-wrap"_spr);
        wrap->setPosition(child->getContentSize() / 2.f);
        child->addChild(wrap);

        // add our funnysprite instead of the simpleplayer
        auto gamemode = (FunnySpriteGamemode)i;
        auto funnySprite = FunnySprite::create();
        funnySprite->setID("funny-sprite");
        funnySprite->updateForGamemode(gamemode);
        funnySprite->setZOrder(2);
        funnySprite->addLimbs(gamemode);
        wrap->addChild(funnySprite);
    }
}
