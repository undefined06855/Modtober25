#include "PlayerObject.hpp"

void FakeSpriteBatchNode::draw() { CCNode::draw(); }
void FakeSpriteBatchNode::visit() { CCNode::visit(); }

bool HookedPlayerObject::init(int player, int ship, GJBaseGameLayer* gameLayer, cocos2d::CCLayer* layer, bool playLayer) {
    if (!gameLayer) {
        // this is an icon on menulayer
        return PlayerObject::init(player, ship, gameLayer, layer, playLayer);
    }

    auto gm = GameManager::get();
    auto origGlow = gm->getPlayerGlow();
    auto origSpider = gm->getPlayerSpider();
    auto origRobot = gm->getPlayerRobot();
    gm->setPlayerGlow(false);
    gm->setPlayerRobot(1); // use default legs for spider and robot
    gm->setPlayerSpider(1);

    if (!PlayerObject::init(player, ship, gameLayer, layer, playLayer)) return false;

    gm->setPlayerGlow(origGlow);
    gm->setPlayerRobot(origRobot);
    gm->setPlayerSpider(origSpider);

    // TODO: icon gradients + fine outline compat (just disable)
    // TODO: more icons compat?

    // TODO: player ghost trail effect still default

    auto fields = m_fields.self();

    auto funnySprite = FunnySprite::create();
    funnySprite->updateForGamemode(FunnySpriteGamemode::Cube);
    funnySprite->setZOrder(1);
    m_mainLayer->addChild(funnySprite);
    fields->m_funnySprite = funnySprite;

    auto funnyVehicleSprite = FunnySprite::create();
    funnyVehicleSprite->updateForGamemode(FunnySpriteGamemode::Ship);
    funnyVehicleSprite->setZOrder(2);
    funnyVehicleSprite->setVisible(false);
    m_mainLayer->addChild(funnyVehicleSprite);
    fields->m_funnyVehicleSprite = funnyVehicleSprite;

    fields->m_currentGamemode = Gamemode::None;

    return true;
}

void HookedPlayerObject::toggleFlyMode(bool enabled, bool fromStart) {
    PlayerObject::toggleFlyMode(enabled, fromStart);
    if (enabled) enterGamemode(Gamemode::Ship);
    else exitGamemode(Gamemode::Ship);
}

void HookedPlayerObject::toggleRollMode(bool enabled, bool fromStart) {
    PlayerObject::toggleRollMode(enabled, fromStart);
    if (enabled) enterGamemode(Gamemode::Ball);
    else exitGamemode(Gamemode::Ball);
}

void HookedPlayerObject::toggleBirdMode(bool enabled, bool fromStart) {
    PlayerObject::toggleBirdMode(enabled, fromStart);
    if (enabled) enterGamemode(Gamemode::Ufo);
    else exitGamemode(Gamemode::Ufo);
}

void HookedPlayerObject::toggleDartMode(bool enabled, bool fromStart) {
    PlayerObject::toggleDartMode(enabled, fromStart);
    if (enabled) enterGamemode(Gamemode::Wave);
    else exitGamemode(Gamemode::Wave);
}

void HookedPlayerObject::toggleRobotMode(bool enabled, bool fromStart) {
    PlayerObject::toggleRobotMode(enabled, fromStart);
    if (enabled) enterGamemode(Gamemode::Robot);
    else exitGamemode(Gamemode::Robot);
}

void HookedPlayerObject::toggleSpiderMode(bool enabled, bool fromStart) {
    PlayerObject::toggleSpiderMode(enabled, fromStart);
    if (enabled) enterGamemode(Gamemode::Spider);
    else exitGamemode(Gamemode::Spider);
}

void HookedPlayerObject::toggleSwingMode(bool enabled, bool fromStart) {
    PlayerObject::toggleSwingMode(enabled, fromStart);
    if (enabled) enterGamemode(Gamemode::Swing);
    else exitGamemode(Gamemode::Swing);
}

void HookedPlayerObject::enterGamemode(Gamemode gamemode) {
    m_fields->m_currentGamemode = gamemode;
    updateFunnySprite();
}

void HookedPlayerObject::exitGamemode(Gamemode gamemode) {
    auto fields = m_fields.self();

    if (fields->m_currentGamemode == gamemode) {
        // when going into cube rob toggles off every gamemode
        // we just have to assume that's what he's doing
        fields->m_currentGamemode = Gamemode::Cube;
        updateFunnySprite(); // which makes this unnecessary 7 out of 8 times
    }
}

void HookedPlayerObject::updateFunnySprite() {
    if (!m_gameLayer) return;

    auto fields = m_fields.self();

    auto funnySpriteGamemode = (FunnySpriteGamemode)fields->m_currentGamemode;
    if (m_isPlatformer && funnySpriteGamemode == FunnySpriteGamemode::Ship) {
        funnySpriteGamemode = FunnySpriteGamemode::Jetpack;
    }

    // TODO: ufo dome

    switch (fields->m_currentGamemode) {
        case Gamemode::Cube:
        case Gamemode::Ball:
        case Gamemode::Wave:
        case Gamemode::Swing: {
            // set type to whatever, hide vehicle
            fields->m_funnySprite->updateForGamemode(funnySpriteGamemode);
            fields->m_funnySprite->setVisible(true);
            fields->m_funnyVehicleSprite->setVisible(false);
        } break;

        case Gamemode::Ship:
        case Gamemode::Ufo: {
            // set sprite to cube (passenger), show vehicle
            fields->m_funnySprite->updateForGamemode(FunnySpriteGamemode::CubePassenger);
            fields->m_funnyVehicleSprite->updateForGamemode(funnySpriteGamemode);
            fields->m_funnySprite->setVisible(true);
            fields->m_funnyVehicleSprite->setVisible(true);
        } break;

        case Gamemode::Robot:
        case Gamemode::Spider: {
            // let the funnysprites in gjspidersprite and gjrobotsprite handle this
            fields->m_funnySprite->setVisible(false);
            fields->m_funnyVehicleSprite->setVisible(false);
        } break;

        default: break;
    }
}

void HookedPlayerObject::createRobot(int frame) {
    // set up funnysprite in gjrobotsprite
    PlayerObject::createRobot(frame);
    if (!m_gameLayer) return;

    if (m_robotSprite->m_paSprite->getChildByType<FunnySprite>(0)) return;

    auto funnySprite = FunnySprite::create();
    funnySprite->updateForGamemode(FunnySpriteGamemode::Robot);
    funnySprite->setZOrder(3);
    m_robotSprite->m_paSprite->addChild(funnySprite);

    // nuclear option
    // somehow rob resets everything on this every frame ??
    m_robotSprite->m_headSprite->removeFromParent();

    patchBatchNode(m_robotBatchNode);
}

void HookedPlayerObject::createSpider(int frame) {
    // set up funnysprite in gjspidersprite
    PlayerObject::createSpider(frame);
    if (!m_gameLayer) return;

    if (m_spiderSprite->m_paSprite->getChildByType<FunnySprite>(0)) return;

    auto funnySprite = FunnySprite::create();
    funnySprite->updateForGamemode(FunnySpriteGamemode::Spider);
    funnySprite->setZOrder(3);
    m_spiderSprite->m_paSprite->addChild(funnySprite);

    m_spiderSprite->m_headSprite->removeFromParent();

    patchBatchNode(m_spiderBatchNode);
}

void HookedPlayerObject::update(float dt) {
    PlayerObject::update(dt);
    if (!m_gameLayer) return;

    auto fields = m_fields.self();

    if (m_iconSprite) m_iconSprite->setVisible(false);
    if (m_vehicleSprite) m_vehicleSprite->setVisible(false);

    // TODO: set visibility of funnysprite depending on iconSprite for respawn
    // animation

    if (m_iconSprite) {
        fields->m_funnySprite->setScaleX(m_iconSprite->getScaleX());
        fields->m_funnySprite->setScaleY(m_iconSprite->getScaleY());
        fields->m_funnySprite->setRotation(m_iconSprite->getRotation());
    }

    if (m_vehicleSprite) {
        fields->m_funnyVehicleSprite->setScaleX(m_vehicleSprite->getScaleX());
        fields->m_funnyVehicleSprite->setScaleY(m_vehicleSprite->getScaleY());
        fields->m_funnyVehicleSprite->setRotation(m_vehicleSprite->getRotation());
    }
}

void HookedPlayerObject::patchBatchNode(cocos2d::CCSpriteBatchNode* batchNode) {
    if (!m_gameLayer) return;

    // icon gradients already includes this (this is taken from it)
    if (geode::Loader::get()->isModLoaded("zilko.icon_gradients")) return;

    // replace vtable of node with the one from fakespritebatchnode to remove
    // all of the batch stuff so we can use our own sprites etc

    // this is pretty scary code ngl but well it works for zilko ig
    static void* vtable = []() -> void* {
        FakeSpriteBatchNode temp;
        return *(void**)&temp;
    }();

    *(void**)batchNode = vtable;
}
