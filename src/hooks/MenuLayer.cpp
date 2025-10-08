#include "MenuLayer.hpp"
#include "../FunnySpriteManager.hpp"

bool HookedMenuLayer::init() {
    if (!MenuLayer::init()) return false;

    auto& fsm = FunnySpriteManager::get();
    fsm.updateRenderedSprites();

    auto mainMenu = getChildByID("main-menu");
    if (!mainMenu) return true;

    auto iconKit = mainMenu->getChildByID("icon-kit-button");
    if (!iconKit) return true;

    auto pos = mainMenu->convertToWorldSpace(iconKit->getPosition());

    // incompat with the label you see on a new save file but whatever
    bool seen = geode::Mod::get()->getSavedValue<bool>("clicked-icon-kit", false);
    if (!seen) {
        auto iconKitText = cocos2d::CCSprite::create("icon-kit.png"_spr);
        iconKitText->setID("icon-kit-text"_spr);
        iconKitText->setPosition(pos + cocos2d::CCPoint{ -52.f, -42.f });
        addChild(iconKitText);
    }

    auto newIcon = cocos2d::CCSprite::create("new-icon.png"_spr);
    newIcon->setID("new-badge"_spr);
    newIcon->setPosition({ 60.f, 60.f });
    newIcon->setScale(.6f);
    newIcon->setRotation(9.f);
    iconKit->addChild(newIcon);

    auto action = cocos2d::CCRepeatForever::create(
        cocos2d::CCSequence::createWithTwoActions(
            cocos2d::CCEaseInOut::create(cocos2d::CCRotateBy::create(1.2f, 10.f), 2.f),
            cocos2d::CCEaseInOut::create(cocos2d::CCRotateBy::create(1.2f, -10.f), 2.f)
        )
    );
    newIcon->runAction(action);

    if (fsm.m_shaderFailed) {
        auto pop = FLAlertLayer::create(
            "\"\"Icon\"\" Kit",
            "The <cj>mapping shader</c> <cr>failed</c> to compile!\nThe mod "
            "will not <cl>work!</c>",
            "ok"
        );
        pop->m_scene = this;
        pop->show();
    }

    // TODO: better separate dual icons compat?
    if (geode::Loader::get()->isModLoaded("weebify.separate_dual_icons")) {
        auto pop = FLAlertLayer::create(
            "\"\"Icon\"\" Kit",
            "<cj>Separate Dual icons</c> is installed!\nThis mod will "
            "<cy>function</c> with it installed, but you will <cr>not</c> be "
            "able to change your <cb>2-player icon</c>.",
            "ok"
        );
        pop->m_scene = this;
        pop->show();
    }

    return true;
}

void HookedMenuLayer::onGarage(cocos2d::CCObject* sender) {
    MenuLayer::onGarage(sender);
    geode::Mod::get()->setSavedValue("clicked-icon-kit", true);
}
