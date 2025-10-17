#include "FunnySpriteManager.hpp"
#include "shaders.hpp"
#include <hiimjustin000.more_icons/include/MoreIcons.hpp>
#include <Geode/utils/coro.hpp>

// 32 * 4 * 4 = 512
// icon size * high graphics * 4
#define RENDERTEXTURE_INIT_PARAMS 512, 512, GL_RGBA, GL_RGBA, GL_LINEAR, GL_CLAMP_TO_EDGE

RenderTextureGroup::RenderTextureGroup()
    : m_cube(RENDERTEXTURE_INIT_PARAMS)
    , m_ship(RENDERTEXTURE_INIT_PARAMS)
    , m_ball(RENDERTEXTURE_INIT_PARAMS)
    , m_ufo(RENDERTEXTURE_INIT_PARAMS)
    , m_wave(RENDERTEXTURE_INIT_PARAMS)
    , m_robot(RENDERTEXTURE_INIT_PARAMS)
    , m_spider(RENDERTEXTURE_INIT_PARAMS)
    , m_swing(RENDERTEXTURE_INIT_PARAMS)
    , m_jetpack(RENDERTEXTURE_INIT_PARAMS) {}

#undef RENDERTEXTURE_IMAGE_PARAMS

Texture2DGroup::Texture2DGroup()
    : m_cube(nullptr)
    , m_ship(nullptr)
    , m_ball(nullptr)
    , m_ufo(nullptr)
    , m_wave(nullptr)
    , m_robot(nullptr)
    , m_spider(nullptr)
    , m_swing(nullptr)
    , m_jetpack(nullptr) {}

FunnySpriteManager::FunnySpriteManager()
    : m_mainIcons()
    , m_dualIcons()
    , m_mainIconsMainOnly()

    , m_icon({})

    , m_wantsRealCountForType(false)
    , m_totalCountForTypes(0)

    , m_iconsForIconType({})

    , m_shaderFailed(false) {}

FunnySpriteManager& FunnySpriteManager::get() {
    static FunnySpriteManager instance;
    return instance;
}

GLuint FunnySpriteManager::textureForGamemode(FunnySpriteGamemode gamemode, bool dual, bool mainOnly) {
    if (mainOnly) {
        // for ghost trail textures
        switch (gamemode) {
            case FunnySpriteGamemode::VehiclePassenger:
            case FunnySpriteGamemode::Cube: return m_mainIconsMainOnly.m_cube->m_uName;
            case FunnySpriteGamemode::Ship: return m_mainIconsMainOnly.m_ship->m_uName;
            case FunnySpriteGamemode::Ball: return m_mainIconsMainOnly.m_ball->m_uName;
            case FunnySpriteGamemode::Ufo: return m_mainIconsMainOnly.m_ufo->m_uName;
            case FunnySpriteGamemode::Wave: return m_mainIconsMainOnly.m_wave->m_uName;
            case FunnySpriteGamemode::Robot: return m_mainIconsMainOnly.m_robot->m_uName;
            case FunnySpriteGamemode::Spider: return m_mainIconsMainOnly.m_spider->m_uName;
            case FunnySpriteGamemode::Swing: return m_mainIconsMainOnly.m_swing->m_uName;
            case FunnySpriteGamemode::Jetpack: return m_mainIconsMainOnly.m_jetpack->m_uName;
            default: return 0;
        }
    }

    if (!dual) {
        switch (gamemode) {
            case FunnySpriteGamemode::VehiclePassenger:
            case FunnySpriteGamemode::Cube: return m_mainIcons.m_cube->m_uName;
            case FunnySpriteGamemode::Ship: return m_mainIcons.m_ship->m_uName;
            case FunnySpriteGamemode::Ball: return m_mainIcons.m_ball->m_uName;
            case FunnySpriteGamemode::Ufo: return m_mainIcons.m_ufo->m_uName;
            case FunnySpriteGamemode::Wave: return m_mainIcons.m_wave->m_uName;
            case FunnySpriteGamemode::Robot: return m_mainIcons.m_robot->m_uName;
            case FunnySpriteGamemode::Spider: return m_mainIcons.m_spider->m_uName;
            case FunnySpriteGamemode::Swing: return m_mainIcons.m_swing->m_uName;
            case FunnySpriteGamemode::Jetpack: return m_mainIcons.m_jetpack->m_uName;
            default: return 0;
        }
    } else {
        switch (gamemode) {
            case FunnySpriteGamemode::VehiclePassenger:
            case FunnySpriteGamemode::Cube: return m_dualIcons.m_cube->m_uName;
            case FunnySpriteGamemode::Ship: return m_dualIcons.m_ship->m_uName;
            case FunnySpriteGamemode::Ball: return m_dualIcons.m_ball->m_uName;
            case FunnySpriteGamemode::Ufo: return m_dualIcons.m_ufo->m_uName;
            case FunnySpriteGamemode::Wave: return m_dualIcons.m_wave->m_uName;
            case FunnySpriteGamemode::Robot: return m_dualIcons.m_robot->m_uName;
            case FunnySpriteGamemode::Spider: return m_dualIcons.m_spider->m_uName;
            case FunnySpriteGamemode::Swing: return m_dualIcons.m_swing->m_uName;
            case FunnySpriteGamemode::Jetpack: return m_dualIcons.m_jetpack->m_uName;
            default: return 0;
        }
    }
}

GLuint FunnySpriteManager::mappingTextureForGamemode(FunnySpriteGamemode gamemode) {
    auto cache = cocos2d::CCTextureCache::get();

    if (gamemode == FunnySpriteGamemode::VehiclePassenger) gamemode = FunnySpriteGamemode::Cube;

    auto texture = cache->textureForKey(fmt::format("{:04}.png"_spr, fmt::underlying(gamemode) + 1).c_str());

    if (!texture) return 0;
    else return texture->getName();
}

GLuint FunnySpriteManager::transparencyMaskForGamemode(FunnySpriteGamemode gamemode) {
    auto cache = cocos2d::CCTextureCache::get();

    if (gamemode == FunnySpriteGamemode::VehiclePassenger) gamemode = FunnySpriteGamemode::Cube;

    auto texture = cache->textureForKey(fmt::format("{:04}.png"_spr, fmt::underlying(gamemode) + 1 + 9).c_str());

    if (!texture) return 0;
    else return texture->getName();
}

// the result of this is set as the playerobject's m_iconSprite's texture for
// the ghost trail to copy
cocos2d::CCTexture2D* FunnySpriteManager::trailTextureForGamemode(FunnySpriteGamemode gamemode) {
    switch (gamemode) {
        case FunnySpriteGamemode::None: return nullptr;
        case FunnySpriteGamemode::Cube: return m_ghostTrailIcons.m_cube;
        case FunnySpriteGamemode::Ship: return m_ghostTrailIcons.m_ship;
        case FunnySpriteGamemode::Ball: return m_ghostTrailIcons.m_ball;
        case FunnySpriteGamemode::Ufo: return m_ghostTrailIcons.m_ufo;
        case FunnySpriteGamemode::Wave: return m_ghostTrailIcons.m_wave;
        case FunnySpriteGamemode::Robot: return m_ghostTrailIcons.m_robot;
        case FunnySpriteGamemode::Spider: return m_ghostTrailIcons.m_spider;
        case FunnySpriteGamemode::Swing: return m_ghostTrailIcons.m_swing;
        case FunnySpriteGamemode::Jetpack: return m_ghostTrailIcons.m_jetpack;
        case FunnySpriteGamemode::VehiclePassenger: return nullptr;
    }
}

void FunnySpriteManager::init() {
    // add textures to cache
    addMappingTexturesToCache();

    // create shader
    getMappingShader();

    // update rendered sprites (now done in menulayer!!!)
    // updateRenderedSprites();

    // load ufo dome for default ufo so we can use it
    // third param is used for unloading the icon so should be something gd won't use
    GameManager::get()->loadIcon(1, (int)IconType::Ufo, 0xb00b1e5);

    // get total count for types
    for (int i = fmt::underlying(IconType::Cube); i <= fmt::underlying(IconType::Jetpack); i++) {
        m_totalCountForTypes += realCountForType((IconType)i);
    }

    // set m_icon
    if (geode::Mod::get()->hasSavedValue("icons") && geode::Mod::get()->getSavedValue<matjson::Value>("icons").size() != 0) {
        // get from mod config
        geode::log::info("getting icon data from mod config");
        auto array = geode::Mod::get()->getSavedValue<matjson::Value>("icons");
        for (int i = 0; i < array.size(); i++) {
            (void)($try<> {
                int index = co_await array[i]["index"].asInt();
                int ofIconType = co_await array[i]["of"].asUInt();

                geode::log::info("{} icon is {} of {}", i, index, ofIconType);

                m_icon[(IconType)i] = IconChoiceInfo{
                    .m_index = index,
                    .m_iconType = (IconType)ofIconType
                };
            });
        }
    } else {
        // get from save data
        geode::log::info("getting icon data from gd save");
        for (IconType i = IconType::Cube; i <= IconType::Jetpack; i = (IconType)(fmt::underlying(i) + 1)) {
            m_icon[i] = IconChoiceInfo{
                .m_index = GameManager::get()->activeIconForType(i),
                .m_iconType = i
            };
        }

        saveIconChoice();
    }

    // create m_icons
    for (IconType i = IconType::Cube; i <= IconType::Jetpack; i = (IconType)(fmt::underlying(i) + 1)) {
        m_iconsForIconType[i] = {};

        // first, add all icons for this icon type in the vanilla game
        for (int j = 0; j < realCountForType(i); j++) {
            m_iconsForIconType[i].push_back({
                .m_type = i,
                .m_index = j + 1
            });
        }

        // then, add all icons for the rest of the icon types, skipping i
        for (IconType j = IconType::Cube; j <= IconType::Jetpack; j = (IconType)(fmt::underlying(j) + 1)) {
            if (j == i) continue;

            for (int k = 0; k < realCountForType(j); k++) {
                m_iconsForIconType[i].push_back({
                    .m_type = j,
                    .m_index = k + 1
                });
            }
        }
    }
}

void FunnySpriteManager::addMappingTexturesToCache() {
    auto cache = cocos2d::CCTextureCache::get();
    for (int i = 1; i <= 18; i++) {
        cache->addImage(fmt::format("{:04}.png"_spr, i).c_str(), false);
    }
}

// if there's a better way to do this let me know please!!!
void FunnySpriteManager::recreateTextures() {
    m_dualIcons.~Texture2DGroup();
    new (&m_dualIcons) Texture2DGroup();

    m_mainIcons.~Texture2DGroup();
    new (&m_mainIcons) Texture2DGroup();

    m_mainIconsMainOnly.~Texture2DGroup();
    new (&m_mainIconsMainOnly) Texture2DGroup();

    m_ghostTrailIcons.~Texture2DGroup();
    new (&m_ghostTrailIcons) Texture2DGroup();

    addMappingTexturesToCache();

    updateRenderedSprites();
}

cocos2d::CCGLProgram* FunnySpriteManager::getMappingShader() {
    auto mappingShader = cocos2d::CCShaderCache::sharedShaderCache()->programForKey("mapping_shader"_spr);
    if (mappingShader) return mappingShader;

    mappingShader = new cocos2d::CCGLProgram;
    bool ret = mappingShader->initWithVertexShaderByteArray(g_mappingShaderVertex, g_mappingShaderFragment);
    if (!ret) {
        geode::log::error("Shader failed to load!");
        geode::log::error("{}", mappingShader->fragmentShaderLog()); // probably going to crash anyway

        m_shaderFailed = true;
        return nullptr;
    }

    mappingShader->addAttribute(kCCAttributeNamePosition, cocos2d::kCCVertexAttrib_Position);
    mappingShader->addAttribute(kCCAttributeNameColor, cocos2d::kCCVertexAttrib_Color);
    mappingShader->addAttribute(kCCAttributeNameTexCoord, cocos2d::kCCVertexAttrib_TexCoords);

    mappingShader->link();
    mappingShader->updateUniforms();

    mappingShader->setUniformLocationWith1i(mappingShader->getUniformLocationForName("CC_Texture1"), 1);
    mappingShader->setUniformLocationWith1i(mappingShader->getUniformLocationForName("CC_Texture2"), 2);

    cocos2d::CCShaderCache::sharedShaderCache()->addProgram(mappingShader, "mapping_shader"_spr);

    return mappingShader;
}

void FunnySpriteManager::saveIconChoice() {
    std::vector<matjson::Value> save = {};
    save.resize(fmt::underlying(IconType::Jetpack) + 1, nullptr);

    for (auto& [type, info] : m_icon) {
        save[fmt::underlying(type)] = matjson::makeObject({
            { "index", info.m_index },
            { "of", fmt::underlying(info.m_iconType) }
        });
    }

    geode::Mod::get()->setSavedValue("icons", matjson::Value(save));
}

void FunnySpriteManager::updateRenderedSprites() {
    updateRenderedSprites(m_mainIcons, false, false);
    updateRenderedSprites(m_dualIcons, true, false);
    updateRenderedSprites(m_mainIconsMainOnly, false, true);
    updateRenderedTrailSprites(m_ghostTrailIcons);
}

void FunnySpriteManager::updateRenderedSprites(Texture2DGroup& group, bool dual, bool mainOnly) {
    updateRenderedSprite(group.m_cube, IconType::Cube, dual, mainOnly);
    updateRenderedSprite(group.m_ship, IconType::Ship, dual, mainOnly);
    updateRenderedSprite(group.m_ball, IconType::Ball, dual, mainOnly);
    updateRenderedSprite(group.m_ufo, IconType::Ufo, dual, mainOnly);
    updateRenderedSprite(group.m_wave, IconType::Wave, dual, mainOnly);
    updateRenderedSprite(group.m_robot, IconType::Robot, dual, mainOnly);
    updateRenderedSprite(group.m_spider, IconType::Spider, dual, mainOnly);
    updateRenderedSprite(group.m_swing, IconType::Swing, dual, mainOnly);
    updateRenderedSprite(group.m_jetpack, IconType::Jetpack, dual, mainOnly);
}

void FunnySpriteManager::updateRenderedTrailSprites(Texture2DGroup& group) {
    updateRenderedTrailSprite(group.m_cube, IconType::Cube);
    updateRenderedTrailSprite(group.m_ship, IconType::Ship);
    updateRenderedTrailSprite(group.m_ball, IconType::Ball);
    updateRenderedTrailSprite(group.m_ufo, IconType::Ufo);
    updateRenderedTrailSprite(group.m_wave, IconType::Wave);
    updateRenderedTrailSprite(group.m_robot, IconType::Robot);
    updateRenderedTrailSprite(group.m_spider, IconType::Spider);
    updateRenderedTrailSprite(group.m_swing, IconType::Swing);
    updateRenderedTrailSprite(group.m_jetpack, IconType::Jetpack);
}

SimplePlayer* FunnySpriteManager::createSimplePlayer(IconType gamemode, bool dual) {
    auto simplePlayer = SimplePlayer::create(0);
    auto playerSprite = simplePlayer->getChildByIndex(0);

    auto gameManager = GameManager::get();

    auto simplePlayerType = m_icon[gamemode].m_iconType;

    // this was so nice and beautiful before MOD COMPATIBILITY :(

    if (dual && geode::Loader::get()->isModLoaded("weebify.separate_dual_icons")) {
        auto mod = geode::Loader::get()->getLoadedMod("weebify.separate_dual_icons");

        static const std::unordered_map<IconType, const char*> iconTypeToName = {
            { IconType::Cube, "cube" },
            { IconType::Ship, "ship" },
            { IconType::Ball, "roll" },
            { IconType::Ufo, "ufo" },
            { IconType::Wave, "dart" },
            { IconType::Robot, "robot" },
            { IconType::Spider, "spider" },
            { IconType::Swing, "swing" },
            { IconType::Jetpack, "jetpack" }
        };

        auto settingName = iconTypeToName.at(simplePlayerType);
        simplePlayer->updatePlayerFrame(mod->getSavedValue<int64_t>(settingName, m_icon[gamemode].m_index), simplePlayerType);
    } else {
        simplePlayer->updatePlayerFrame(m_icon[gamemode].m_index, simplePlayerType);
    }

    if (MoreIcons::loaded()) {
        MoreIcons::updateSimplePlayer(simplePlayer, gamemode, dual);
    }

    if (!dual) {
        simplePlayer->setColor(gameManager->colorForIdx(gameManager->getPlayerColor()));
        simplePlayer->setSecondColor(gameManager->colorForIdx(gameManager->getPlayerColor2()));
    } else {
        if (geode::Loader::get()->isModLoaded("weebify.separate_dual_icons")) {
            auto mod = geode::Loader::get()->getLoadedMod("weebify.separate_dual_icons");
            auto col1 = mod->getSavedValue<int64_t>("color1");
            auto col2 = mod->getSavedValue<int64_t>("color2");

            simplePlayer->setColor(gameManager->colorForIdx(col1));
            simplePlayer->setSecondColor(gameManager->colorForIdx(col2));
        } else {
            simplePlayer->setColor(gameManager->colorForIdx(gameManager->getPlayerColor2()));
            simplePlayer->setSecondColor(gameManager->colorForIdx(gameManager->getPlayerColor()));
        }
    }

    simplePlayer->setGlowOutline(gameManager->colorForIdx(gameManager->getPlayerGlowColor()));
    if (!gameManager->getPlayerGlow()) simplePlayer->disableGlowOutline();

    if (simplePlayerType == IconType::Robot || simplePlayerType == IconType::Spider) {
        // remove all unwanted children
        for (int i = 0; i < simplePlayer->getChildrenCount(); i++) {
            auto child = simplePlayer->getChildByIndex(i);
            if (!geode::cast::typeinfo_cast<GJRobotSprite*>(child)) {
                child->removeFromParent();
                i--;
            }
        }
    }

    return simplePlayer;
}

void FunnySpriteManager::updateRenderedSprite(geode::Ref<cocos2d::CCTexture2D>& texture, IconType gamemode, bool dual, bool mainOnly) {
    auto renderTexture = RenderTexture(512, 512, GL_RGBA, GL_RGBA, GL_LINEAR, GL_CLAMP_TO_EDGE);

    auto simplePlayer = createSimplePlayer(gamemode, dual);
    auto playerSprite = simplePlayer->getChildByIndex(0);

    auto gameManager = GameManager::get();

    // so apparently mat rendertexture is only good if you're rendering
    // something the same size as the screen so we need to resize
    // TODO: look into mat rendertexture?

    // TODO: note: icon gradients and fine outline queues in main thread
    // https://github.com/Zilko/icon-gradients/blob/main/src/Hooks/SimplePlayer.cpp#L19

    auto winSize = cocos2d::CCDirector::get()->getWinSize();
    playerSprite->setPosition(winSize / 2.f);
    if (gameManager->getPlayerGlow()) {
        // add more space for glow
        playerSprite->setScaleX(winSize.width / (playerSprite->getContentWidth() + 2.5f));
        playerSprite->setScaleY(winSize.height / (playerSprite->getContentHeight() + 2.5f));
    } else {
        playerSprite->setScaleX(winSize.width / (playerSprite->getContentWidth() + .5f));
        playerSprite->setScaleY(winSize.height / (playerSprite->getContentHeight() + .5f));
    }

    // if this is a ufo, add more space for dome
    if (m_icon[gamemode].m_iconType == IconType::Ufo) {
        playerSprite->setScaleY(winSize.height / (playerSprite->getContentHeight() + 20.f));
        playerSprite->setPositionY(playerSprite->getPositionY() - 90.f);
    }

    // if this is ship or wave, reduce scale Y a bit
    if (m_icon[gamemode].m_iconType == IconType::Ship || m_icon[gamemode].m_iconType == IconType::Wave) {
        playerSprite->setScaleY(winSize.height / (playerSprite->getContentHeight() + 10.f));
    }

    // if this is a robot or spider, add more space just because
    if (m_icon[gamemode].m_iconType == IconType::Robot || m_icon[gamemode].m_iconType == IconType::Spider) {
        playerSprite->setScaleY(winSize.height / (playerSprite->getContentHeight() + 10.f));
        playerSprite->setPositionY(playerSprite->getPositionY() - 40.f);

        playerSprite->setScaleX(winSize.width / (playerSprite->getContentWidth() + 10.f));
    }

    if (mainOnly) {
        playerSprite->removeAllChildren();
        if (simplePlayer->m_robotSprite) {
            simplePlayer->m_robotSprite->m_paSprite = nullptr;
        }
        if (simplePlayer->m_spiderSprite) {
            simplePlayer->m_spiderSprite->m_paSprite = nullptr;
        }
    }

    renderTexture.capture(playerSprite);
    texture = renderTexture.intoTexture();
    // simplePlayer->release();
}

void FunnySpriteManager::updateRenderedTrailSprite(geode::Ref<cocos2d::CCTexture2D>& texture, IconType gamemode) {
    // this gets weirdly cut off and im  not sure why
    auto size = 32.f * cocos2d::CCDirector::get()->getContentScaleFactor();
    auto renderTexture = RenderTexture(size, size, GL_RGBA, GL_RGBA, GL_LINEAR, GL_CLAMP_TO_EDGE);

    auto funnySprite = FunnySprite::create();
    funnySprite->m_mainOnly = true; // only place where this member is used
    funnySprite->updateForGamemode((FunnySpriteGamemode)gamemode);
    funnySprite->setFlipY(true);
    // don't add limbs!

    auto winSize = cocos2d::CCDirector::get()->getWinSize();
    funnySprite->setPosition(winSize / 2.f);
    funnySprite->setScaleX(winSize.width / (funnySprite->getContentWidth() + .5f));
    funnySprite->setScaleY(winSize.height / (funnySprite->getContentHeight() + .5f));

    if (gamemode == IconType::Robot) {
        // not sure why this is needed but whatever i guess
        // still not completely aligned even if i do this
        funnySprite->setPosition(funnySprite->getPosition() + cocos2d::CCPoint{ 0.f, 100.f });
    }

    renderTexture.capture(funnySprite);

    texture = renderTexture.intoTexture();
}

int FunnySpriteManager::realCountForType(IconType type) {
    m_wantsRealCountForType = true;
    auto ret = GameManager::get()->countForType(type);
    m_wantsRealCountForType = false;

    return ret;
}

CCMenuItemSpriteExtra* FunnySpriteManager::getIcon(UnloadedSingleIconInfo info, GJGarageLayer* target) {
    // copied from GJGarageLayer::getItems
    // there's something to do with playerSquare_001.png that i have no idea about

    auto unlockType = GameManager::get()->iconTypeToUnlockType(info.m_type);

    auto item = GJItemIcon::createBrowserItem(unlockType, info.m_index);
    auto scale = item->scaleForType(unlockType);
    item->setScale(scale);

    auto btn = CCMenuItemSpriteExtra::create(item, target, menu_selector(GJGarageLayer::onSelect));
    btn->setTag(info.m_index);
    btn->m_iconType = info.m_type; // this is really good for us actually

    // not in decomp but will make it match visually (not sure where these are from)
    btn->setContentSize({ 30.f, 30.f });
    item->setPosition({ 15.f, 15.f });

    if (!GameManager::get()->isIconUnlocked(info.m_index, info.m_type)) {
        item->changeToLockedState(1.f / scale);
    }

    return btn;
}

$on_mod(Loaded) {
    FunnySpriteManager::get().init();
}
