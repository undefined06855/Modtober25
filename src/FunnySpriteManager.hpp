#pragma once
#include <RenderTexture.hpp>
#include "FunnySprite.hpp"

class FunnySpriteManager {
    FunnySpriteManager();
public:
    static FunnySpriteManager& get();

    // these store the rendered versions of whatever frame you've chosen for
    // each gamemode

    // then it gets passed to the FunnySprite* and it adds the sprite, then when
    // it renders it renders with the shader to make it appear like the sprite

    RenderTexture m_cube;
    RenderTexture m_ship;
    RenderTexture m_ball;
    RenderTexture m_ufo;
    RenderTexture m_wave;
    RenderTexture m_robot;
    RenderTexture m_spider;
    RenderTexture m_swing;
    RenderTexture m_jetpack;

    cocos2d::CCGLProgram* m_mappingShader;

    void init();
    void updateRenderedSprites();
    void updateRenderedSprite(RenderTexture& renderTexture, IconType gamemode);

    GLuint textureForGamemode(FunnySpriteGamemode gamemode);
    GLuint mappingTextureForGamemode(FunnySpriteGamemode gamemode);
};
