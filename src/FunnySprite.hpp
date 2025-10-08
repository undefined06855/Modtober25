#pragma once

struct GamemodeInfo {
    float m_scale;
    cocos2d::CCPoint m_offset;
};

// make sure the first 9 are the same as IconType
enum class FunnySpriteGamemode {
    None = -1,
    Cube, Ship, Ball, Ufo, Wave, Robot, Spider, Swing, Jetpack,
    VehiclePassenger
};

class FunnySprite : public cocos2d::CCSprite {
public:
    static FunnySprite* create();
    bool init();

    bool m_dual;

    GLuint m_currentTexture;
    GLuint m_currentMappingTexture;
    GLuint m_currentTransparencyTexture;
    FunnySpriteGamemode m_currentGamemode;

    std::vector<cocos2d::CCNode*> m_limbs;
    cocos2d::CCSprite* m_ufoDome;

    void updateTextures();
    void updateForGamemode(FunnySpriteGamemode gamemode);

    void addLimbs(FunnySpriteGamemode gamemode);

    void draw();
};
