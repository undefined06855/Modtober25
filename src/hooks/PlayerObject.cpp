#include "PlayerObject.hpp"

void FakeSpriteBatchNode::draw() { CCNode::draw(); }
void FakeSpriteBatchNode::visit() { CCNode::visit(); }

bool HookedPlayerObject::init(int player, int ship, GJBaseGameLayer* gameLayer, cocos2d::CCLayer* layer, bool playLayer) {
    if (!PlayerObject::init(player, ship, gameLayer, layer, playLayer)) return false;
    if (!gameLayer) return true;

    // TODO: starting a level as a non-cube gamemode uses cube mapping
    // TODO: restarting a level as spider/robot doesn't make sprite visible again

    auto fields = m_fields.self();

    auto funnySprite = FunnySprite::create(this);
    funnySprite->updateForGamemode(FunnySpriteGamemode::Cube);
    funnySprite->setZOrder(1);
    m_mainLayer->addChild(funnySprite);
    fields->m_funnySprite = funnySprite;

    auto funnyVehicleSprite = FunnySprite::create(this);
    funnyVehicleSprite->updateForGamemode(FunnySpriteGamemode::Ship);
    funnyVehicleSprite->setZOrder(2);
    funnyVehicleSprite->setVisible(false);
    m_mainLayer->addChild(funnyVehicleSprite);
    fields->m_funnyVehicleSprite = funnyVehicleSprite;

    fields->m_lastLastActivatedPortal = nullptr;

    return true;
}

void HookedPlayerObject::updatePlayerArt() {
    PlayerObject::updatePlayerArt();
    if (!m_gameLayer) return;

    auto fields = m_fields.self();

    if (fields->m_lastLastActivatedPortal == m_lastActivatedPortal) return;
    fields->m_lastLastActivatedPortal = m_lastActivatedPortal;

    if (!m_lastActivatedPortal) return;

    static const std::unordered_map<GameObjectType, FunnySpriteGamemode> gameObjectToIconMap = {
        { GameObjectType::CubePortal, FunnySpriteGamemode::Cube },
        { GameObjectType::ShipPortal, FunnySpriteGamemode::Ship },
        { GameObjectType::BallPortal, FunnySpriteGamemode::Ball },
        { GameObjectType::UfoPortal, FunnySpriteGamemode::Ufo },
        { GameObjectType::WavePortal, FunnySpriteGamemode::Wave },
        { GameObjectType::RobotPortal, FunnySpriteGamemode::Robot },
        { GameObjectType::SpiderPortal, FunnySpriteGamemode::Spider },
        { GameObjectType::SwingPortal, FunnySpriteGamemode::Swing }
    };

    auto type = m_lastActivatedPortal->m_objectType;

    if (!gameObjectToIconMap.contains(type)) return;

    auto iconType = gameObjectToIconMap.at(type);

    if (type == GameObjectType::ShipPortal && m_isPlatformer) iconType = FunnySpriteGamemode::Jetpack;

    switch (type) {
        case GameObjectType::CubePortal:
        case GameObjectType::BallPortal:
        case GameObjectType::WavePortal:
        case GameObjectType::SwingPortal: {
            // set type to whatever, hide vehicle
            fields->m_funnySprite->updateForGamemode(iconType);
            fields->m_funnySprite->setVisible(true);
            fields->m_funnyVehicleSprite->setVisible(false);
        } break;

        case GameObjectType::ShipPortal:
        case GameObjectType::UfoPortal: {
            // set sprite to cube (passenger), show vehicle
            fields->m_funnySprite->updateForGamemode(FunnySpriteGamemode::CubePassenger);
            fields->m_funnyVehicleSprite->updateForGamemode(iconType);
            fields->m_funnySprite->setVisible(true);
            fields->m_funnyVehicleSprite->setVisible(true);
        } break;

        case GameObjectType::RobotPortal:
        case GameObjectType::SpiderPortal: {
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

    auto funnySprite = FunnySprite::create(this);
    funnySprite->updateForGamemode(FunnySpriteGamemode::Robot);
    funnySprite->setZOrder(3);
    m_robotSprite->m_paSprite->addChild(funnySprite);

    patchBatchNode(m_robotBatchNode);
}

void HookedPlayerObject::createSpider(int frame) {
    // set up funnysprite in gjspidersprite
    PlayerObject::createSpider(frame);
    if (!m_gameLayer) return;

    if (m_spiderSprite->m_paSprite->getChildByType<FunnySprite>(0)) return;

    auto funnySprite = FunnySprite::create(this);
    funnySprite->updateForGamemode(FunnySpriteGamemode::Spider);
    funnySprite->setZOrder(3);
    m_spiderSprite->m_paSprite->addChild(funnySprite);

    patchBatchNode(m_spiderBatchNode);
}

void HookedPlayerObject::update(float dt) {
    PlayerObject::update(dt);
    if (!m_gameLayer) return;

    auto fields = m_fields.self();

    if (m_spiderSprite && m_spiderSprite->m_glowSprite) m_spiderSprite->m_glowSprite->setVisible(false);
    if (m_spiderSprite && m_spiderSprite->m_headSprite) m_spiderSprite->m_headSprite->setVisible(false);

    if (m_robotSprite && m_robotSprite->m_glowSprite) m_robotSprite->m_glowSprite->setVisible(false);
    if (m_robotSprite && m_robotSprite->m_headSprite) m_robotSprite->m_headSprite->setVisible(false);

    if (m_iconSprite) m_iconSprite->setVisible(false);
    if (m_vehicleSprite) m_vehicleSprite->setVisible(false);
    if (m_glowSprite) m_glowSprite->setVisible(false);

    if (m_iconSprite) {
        fields->m_funnySprite->setScaleX(m_iconSprite->getScaleX());
        fields->m_funnySprite->setScaleY(m_iconSprite->getScaleY());
    }

    if (m_vehicleSprite) {
        fields->m_funnyVehicleSprite->setScaleX(m_vehicleSprite->getScaleX());
        fields->m_funnyVehicleSprite->setScaleY(m_vehicleSprite->getScaleY());
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
