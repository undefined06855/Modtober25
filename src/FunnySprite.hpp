#pragma once

struct GamemodeInfo {
    float m_scale;
    cocos2d::CCPoint m_offset;
};

// make sure the first 9 are the same as IconType
enum class FunnySpriteGamemode {
    Cube, Ship, Ball, Ufo, Wave, Robot, Spider, Swing, Jetpack,
    CubePassenger
};

class FunnySprite : public cocos2d::CCSprite {
public:
    static FunnySprite* create();
    bool init();

    GLuint m_currentTexture;
    GLuint m_currentMappingTexture;

    void updateForGamemode(FunnySpriteGamemode gamemode);
    void updateForGamemode(IconType gamemode);

    void draw();
};
