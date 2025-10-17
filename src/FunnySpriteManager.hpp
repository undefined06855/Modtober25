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
    ~RenderTextureGroup();

    cocos2d::CCTexture2D* m_cube;
    cocos2d::CCTexture2D* m_ship;
    cocos2d::CCTexture2D* m_ball;
    cocos2d::CCTexture2D* m_ufo;
    cocos2d::CCTexture2D* m_wave;
    cocos2d::CCTexture2D* m_robot;
    cocos2d::CCTexture2D* m_spider;
    cocos2d::CCTexture2D* m_swing;
    cocos2d::CCTexture2D* m_jetpack;
};

struct Texture2DGroup {
    Texture2DGroup();

    geode::Ref<cocos2d::CCTexture2D> m_cube;
    geode::Ref<cocos2d::CCTexture2D> m_ship;
    geode::Ref<cocos2d::CCTexture2D> m_ball;
    geode::Ref<cocos2d::CCTexture2D> m_ufo;
    geode::Ref<cocos2d::CCTexture2D> m_wave;
    geode::Ref<cocos2d::CCTexture2D> m_robot;
    geode::Ref<cocos2d::CCTexture2D> m_spider;
    geode::Ref<cocos2d::CCTexture2D> m_swing;
    geode::Ref<cocos2d::CCTexture2D> m_jetpack;
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
    RenderTextureGroup m_mainIconsMainOnly; // for ghost trail
    Texture2DGroup m_ghostTrailIcons;

    std::unordered_map<IconType, IconChoiceInfo> m_icon;

    bool m_wantsRealCountForType;
    int m_totalCountForTypes;

    std::unordered_map<IconType, std::vector<UnloadedSingleIconInfo>> m_iconsForIconType;

    bool m_shaderFailed;

    void init();
    void updateRenderedSprites();
    void updateRenderedSprites(RenderTextureGroup& group, bool dual, bool mainOnly);
    void updateRenderedTrailSprites(Texture2DGroup& group);
    SimplePlayer* createSimplePlayer(IconType gamemode, bool dual);
    void updateRenderedSprite(cocos2d::CCTexture2D* texture, IconType gamemode, bool dual, bool mainOnly);
    void updateRenderedTrailSprite(geode::Ref<cocos2d::CCTexture2D>& texture, IconType gamemode);

    void addMappingTexturesToCache();
    void recreateTextures();

    cocos2d::CCGLProgram* getMappingShader();

    GLuint textureForGamemode(FunnySpriteGamemode gamemode, bool dual, bool mainOnly);
    GLuint mappingTextureForGamemode(FunnySpriteGamemode gamemode);
    GLuint transparencyMaskForGamemode(FunnySpriteGamemode gamemode);
    cocos2d::CCTexture2D* trailTextureForGamemode(FunnySpriteGamemode gamemode);

    int realCountForType(IconType type);

    void saveIconChoice();

    CCMenuItemSpriteExtra* getIcon(UnloadedSingleIconInfo info, GJGarageLayer* target);
};
