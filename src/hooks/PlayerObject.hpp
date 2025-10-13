#pragma once
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/CCSprite.hpp>
#include "../FunnySprite.hpp"

// see HookedPlayerObject::patchBatchNode
class FakeSpriteBatchNode : public cocos2d::CCSpriteBatchNode {
public:
    void draw();
    void visit();
};

enum class Gamemode {
    None = -1,
    Cube, Ship, Ball, Ufo, Wave, Robot, Spider, Swing
};

class $modify(HookedPlayerObject, PlayerObject) {
    struct Fields {
        FunnySprite* m_funnySprite;
        FunnySprite* m_funnyVehicleSprite;
        FunnySprite* m_funnyRobotSprite;
        FunnySprite* m_funnySpiderSprite;

        bool m_isDualUpdated;
        Gamemode m_currentGamemode; // hope this doesnt desync!

        bool m_showFunnySprite;
        bool m_showFunnyVehicleSprite;
    };

    bool init(int player, int ship, GJBaseGameLayer* gameLayer, cocos2d::CCLayer* layer, bool playLayer);

    void toggleFlyMode(bool enabled, bool fromStart);
    void toggleRollMode(bool enabled, bool fromStart);
    void toggleBirdMode(bool enabled, bool fromStart);
    void toggleDartMode(bool enabled, bool fromStart);
    void toggleRobotMode(bool enabled, bool fromStart);
    void toggleSpiderMode(bool enabled, bool fromStart);
    void toggleSwingMode(bool enabled, bool fromStart);

    void enterGamemode(Gamemode gamemode);
    void exitGamemode(Gamemode gamemode);

    void updateFunnySprite();

    void createRobot(int frame);
    void createSpider(int frame);

    void playDeathEffect();

    void update(float dt);

    void updateShitVisibility();
    void patchBatchNode(cocos2d::CCSpriteBatchNode* batchNode);
    bool shouldDoChanges();
};
