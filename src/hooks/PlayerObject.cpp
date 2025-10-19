#include "PlayerObject.hpp"
#include "../FunnySpriteManager.hpp"
#include "GJBaseGameLayer.hpp"
#include <Geode/utils/coro.hpp>

void FakeSpriteBatchNode::draw() { CCNode::draw(); }
void FakeSpriteBatchNode::visit() { CCNode::visit(); }

bool HookedPlayerObject::init(int player, int ship, GJBaseGameLayer* gameLayer, cocos2d::CCLayer* layer, bool playLayer) {
    m_gameLayer = gameLayer; // required by shouldDoChanges

    if (!shouldDoChanges()) {
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

    auto fields = m_fields.self();

    auto funnySprite = FunnySprite::create();
    funnySprite->setID("funny-sprite"_spr);
    funnySprite->updateForGamemode(FunnySpriteGamemode::Cube);
    funnySprite->setZOrder(1);
    m_mainLayer->addChild(funnySprite);
    fields->m_funnySprite = funnySprite;

    auto funnyVehicleSprite = FunnySprite::create();
    funnyVehicleSprite->setID("funny-vehicle-sprite"_spr);
    funnyVehicleSprite->updateForGamemode(FunnySpriteGamemode::Ship);
    funnyVehicleSprite->setZOrder(2);
    m_mainLayer->addChild(funnyVehicleSprite);
    fields->m_funnyVehicleSprite = funnyVehicleSprite;

    fields->m_currentGamemode = Gamemode::Cube;
    fields->m_isDualUpdated = false;

    fields->m_showFunnySprite = true;
    fields->m_showFunnyVehicleSprite = false;

    updateShitVisibility();
    updateTrailTexture();

    // :(
    geode::Loader::get()->queueInMainThread([self = geode::WeakRef(this)] {
        if (auto lock = self.lock()) lock->updateShitVisibility();
    });

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

void HookedPlayerObject::playDeathEffect() {
    PlayerObject::playDeathEffect();

    if (!shouldDoChanges()) return;

    updateShitVisibility();
}

void HookedPlayerObject::updateFunnySprite() {
    if (!shouldDoChanges()) return;

    auto fields = m_fields.self();

    auto funnySpriteGamemode = (FunnySpriteGamemode)fields->m_currentGamemode;
    if (m_isPlatformer && funnySpriteGamemode == FunnySpriteGamemode::Ship) {
        funnySpriteGamemode = FunnySpriteGamemode::Jetpack;
    }

    switch (fields->m_currentGamemode) {
        case Gamemode::Cube:
        case Gamemode::Ball:
        case Gamemode::Wave:
        case Gamemode::Swing: {
            // set type to whatever, hide vehicle
            fields->m_funnySprite->updateForGamemode(funnySpriteGamemode);
            fields->m_showFunnySprite = true;
            fields->m_showFunnyVehicleSprite = false;
        } break;

        case Gamemode::Ship:
        case Gamemode::Ufo: {
            // set sprite to cube (passenger), show vehicle
            fields->m_funnySprite->updateForGamemode(FunnySpriteGamemode::VehiclePassenger);
            fields->m_funnyVehicleSprite->updateForGamemode(funnySpriteGamemode);
            fields->m_showFunnySprite = true;
            fields->m_showFunnyVehicleSprite = true;
        } break;

        case Gamemode::Robot:
        case Gamemode::Spider: {
            // let the funnysprites in gjspidersprite and gjrobotsprite handle this
            fields->m_showFunnySprite = false;
            fields->m_showFunnyVehicleSprite = false;
        } break;

        default: break;
    }

    updateTrailTexture();
    updateShitVisibility();
}

void HookedPlayerObject::updateTrailTexture() {
    m_iconSprite->setTexture(FunnySpriteManager::get().trailTextureForGamemode((FunnySpriteGamemode)m_fields->m_currentGamemode));
    m_iconSprite->setTextureRect({ 0.f, 0.f, 32.f, 32.f });
}

void HookedPlayerObject::createRobot(int frame) {
    // set up funnysprite in gjrobotsprite
    PlayerObject::createRobot(frame);
    if (!shouldDoChanges()) return;

    if (m_robotSprite->m_paSprite->getChildByType<FunnySprite>(0)) return;

    auto funnySprite = FunnySprite::create();
    funnySprite->setID("funny-robot-sprite"_spr);
    funnySprite->updateForGamemode(FunnySpriteGamemode::Robot);
    funnySprite->setZOrder(3);
    m_robotSprite->m_paSprite->addChild(funnySprite);
    m_fields->m_funnyRobotSprite = funnySprite;

    // nuclear option
    // somehow rob resets everything on this every frame ??
    m_robotSprite->m_headSprite->removeFromParent();

    patchBatchNode(m_robotBatchNode);
}

void HookedPlayerObject::createSpider(int frame) {
    // set up funnysprite in gjspidersprite
    PlayerObject::createSpider(frame);
    if (!shouldDoChanges()) return;

    if (m_spiderSprite->m_paSprite->getChildByType<FunnySprite>(0)) return;

    auto funnySprite = FunnySprite::create();
    funnySprite->setID("funny-spider-sprite"_spr);
    funnySprite->updateForGamemode(FunnySpriteGamemode::Spider);
    funnySprite->setZOrder(3);
    m_spiderSprite->m_paSprite->addChild(funnySprite);
    m_fields->m_funnySpiderSprite = funnySprite;

    m_spiderSprite->m_headSprite->removeFromParent();

    patchBatchNode(m_spiderBatchNode);
}

void HookedPlayerObject::update(float dt) {
    PlayerObject::update(dt);
    if (!shouldDoChanges()) return;

    auto fields = m_fields.self();

    updateShitVisibility();

    if (m_isSecondPlayer && !fields->m_isDualUpdated) {
        fields->m_funnySprite->m_dual = true;
        fields->m_funnyVehicleSprite->m_dual = true;
        fields->m_funnyRobotSprite->m_dual = true;
        fields->m_funnySpiderSprite->m_dual = true;

        fields->m_funnySprite->updateForGamemode(fields->m_funnySprite->m_currentGamemode);
        fields->m_funnyVehicleSprite->updateForGamemode(fields->m_funnyVehicleSprite->m_currentGamemode);
        fields->m_funnySpiderSprite->updateForGamemode(FunnySpriteGamemode::Spider);
        fields->m_funnyRobotSprite->updateForGamemode(FunnySpriteGamemode::Robot);

        fields->m_isDualUpdated = true;
    }
}

void HookedPlayerObject::updateShitVisibility() {
    auto fields = m_fields.self();

    if (m_iconSprite) m_iconSprite->setVisible(false);
    if (m_vehicleSprite) m_vehicleSprite->setVisible(false);

    if (m_iconSprite && fields->m_funnySprite) {
        // cube platformer animations
        if (fields->m_currentGamemode == Gamemode::Cube) {
            fields->m_funnySprite->setScaleX(m_iconSprite->getScaleX());
            fields->m_funnySprite->setScaleY(m_iconSprite->getScaleY());
        }
        fields->m_funnySprite->setRotation(m_iconSprite->getRotation());
    }

    if (m_vehicleSprite) {
        fields->m_funnyVehicleSprite->setScaleX(m_vehicleSprite->getScaleX());
        fields->m_funnyVehicleSprite->setScaleY(m_vehicleSprite->getScaleY());
        fields->m_funnyVehicleSprite->setRotation(m_vehicleSprite->getRotation());
    }

    if (fields->m_funnyRobotSprite) fields->m_funnyRobotSprite->setColor({ 255, 255, 255 });
    if (fields->m_funnySpiderSprite) fields->m_funnySpiderSprite->setColor({ 255, 255, 255 });

    fields->m_funnySprite->setVisible(!m_isDead && fields->m_showFunnySprite);
    fields->m_funnyVehicleSprite->setVisible(!m_isDead && fields->m_showFunnyVehicleSprite);
}

void HookedPlayerObject::patchBatchNode(cocos2d::CCSpriteBatchNode* batchNode) {
    if (!shouldDoChanges()) return;

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

bool HookedPlayerObject::shouldDoChanges() {
    // changed so globed players dont get modified
    if (!m_gameLayer) return false;

    return static_cast<HookedGJBaseGameLayer*>(m_gameLayer)->m_fields->m_creatingPlayers || m_gameLayer->m_player1 == this || m_gameLayer->m_player2 == this;
}
