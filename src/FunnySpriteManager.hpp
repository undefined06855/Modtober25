#pragma once
#include <RenderTexture.hpp>
#include "FunnySprite.hpp"

struct IconChoiceInfo {
    int m_index;
    IconType m_ofIconType;
};

struct UnloadedSingleIconInfo {
    IconType m_type;
    int m_index;
};

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

    std::unordered_map<IconType, IconChoiceInfo> m_icon;

    cocos2d::CCGLProgram* m_mappingShader;

    bool m_wantsRealCountForType;
    int m_totalCountForTypes;

    std::unordered_map<IconType, std::vector<UnloadedSingleIconInfo>> m_iconsForIconType = {};

    void init();
    void updateRenderedSprites();
    void updateRenderedSprite(RenderTexture& renderTexture, IconType gamemode);

    GLuint textureForGamemode(FunnySpriteGamemode gamemode);
    GLuint mappingTextureForGamemode(FunnySpriteGamemode gamemode);

    int realCountForType(IconType type);

    int currentIconIndexInTermsOf(IconType type, IconType typeInTermsOf);

    void saveIconChoice();

    CCMenuItemSpriteExtra* getIcon(UnloadedSingleIconInfo info, GJGarageLayer* target);
};
