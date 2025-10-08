#pragma once
#include <RenderTexture.hpp>
#include "FunnySprite.hpp"

struct IconChoiceInfo {
    int m_index;
    IconType m_iconType;
};

struct UnloadedSingleIconInfo {
    IconType m_type;
    int m_index;
};

struct RenderTextureGroup {
    RenderTextureGroup();

    RenderTexture m_cube;
    RenderTexture m_ship;
    RenderTexture m_ball;
    RenderTexture m_ufo;
    RenderTexture m_wave;
    RenderTexture m_robot;
    RenderTexture m_spider;
    RenderTexture m_swing;
    RenderTexture m_jetpack;
};

class FunnySpriteManager {
    FunnySpriteManager();
public:
    static FunnySpriteManager& get();

    // these store the rendered versions of whatever frame you've chosen for
    // each gamemode

    // then it gets passed to the FunnySprite* and it adds the sprite, then when
    // it renders it renders with the shader to make it appear like the sprite

    RenderTextureGroup m_mainIcons;
    RenderTextureGroup m_dualIcons;

    std::unordered_map<IconType, IconChoiceInfo> m_icon;

    bool m_wantsRealCountForType;
    int m_totalCountForTypes;

    std::unordered_map<IconType, std::vector<UnloadedSingleIconInfo>> m_iconsForIconType;

    void init();
    void updateRenderedSprites();
    void updateRenderedSprites(RenderTextureGroup& group, bool dual);
    void updateRenderedSprite(RenderTexture& renderTexture, IconType gamemode, bool dual);

    cocos2d::CCGLProgram* getMappingShader();

    GLuint textureForGamemode(FunnySpriteGamemode gamemode, bool dual);
    GLuint mappingTextureForGamemode(FunnySpriteGamemode gamemode);
    GLuint transparencyMaskForGamemode(FunnySpriteGamemode gamemode);

    int realCountForType(IconType type);

    void saveIconChoice();

    CCMenuItemSpriteExtra* getIcon(UnloadedSingleIconInfo info, GJGarageLayer* target);
};
