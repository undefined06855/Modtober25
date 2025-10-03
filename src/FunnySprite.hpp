#pragma once

struct GamemodeInfo {
    float m_scale;
    cocos2d::CCPoint m_offset;
};

enum class FunnySpriteGamemode {
    Cube, Ship, Ball, Ufo, Wave, Robot, Spider, Swing, Jetpack,
    CubePassenger
};

class FunnySprite : public cocos2d::CCSprite {
public:
    static FunnySprite* create(PlayerObject* playerObject);
    bool init(PlayerObject* playerObject);

    PlayerObject* m_playerObject;
    GLuint m_currentTexture;
    GLuint m_currentMappingTexture;

    virtual void updateForGamemode(FunnySpriteGamemode gamemode);

    virtual void draw() override;
};
