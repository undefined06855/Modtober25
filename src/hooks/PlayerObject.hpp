#pragma once
#include <Geode/modify/PlayerObject.hpp>
#include "../FunnySprite.hpp"

// see HookedPlayerObject::patchBatchNode
class FakeSpriteBatchNode : public cocos2d::CCSpriteBatchNode {
public:
    virtual void draw() override;
    virtual void visit() override;
};

class $modify(HookedPlayerObject, PlayerObject) {
    struct Fields {
        FunnySprite* m_funnySprite;
        FunnySprite* m_funnyVehicleSprite;
        GameObject* m_lastLastActivatedPortal;
    };

    bool init(int player, int ship, GJBaseGameLayer* gameLayer, cocos2d::CCLayer* layer, bool playLayer);

    void updatePlayerArt();

    void createRobot(int frame);
    void createSpider(int frame);

    void update(float dt) override;

    void patchBatchNode(cocos2d::CCSpriteBatchNode* batchNode);
};
