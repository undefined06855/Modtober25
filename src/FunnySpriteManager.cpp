#include "FunnySpriteManager.hpp"
#include "shaders.hpp"
#include <hiimjustin000.more_icons/include/MoreIcons.hpp>

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

FunnySpriteManager::FunnySpriteManager()
    : m_mainIcons()
    , m_dualIcons()

    , m_icon({})

    , m_wantsRealCountForType(false)
    , m_totalCountForTypes(0)

    , m_iconsForIconType({})

    , m_shaderFailed(false) {}

FunnySpriteManager& FunnySpriteManager::get() {
    static FunnySpriteManager instance;
    return instance;
}

GLuint FunnySpriteManager::textureForGamemode(FunnySpriteGamemode gamemode, bool dual) {
    if (!dual) {
        switch (gamemode) {
            case FunnySpriteGamemode::VehiclePassenger:
            case FunnySpriteGamemode::Cube: return m_mainIcons.m_cube.getTexture();
            case FunnySpriteGamemode::Ship: return m_mainIcons.m_ship.getTexture();
            case FunnySpriteGamemode::Ball: return m_mainIcons.m_ball.getTexture();
            case FunnySpriteGamemode::Ufo: return m_mainIcons.m_ufo.getTexture();
            case FunnySpriteGamemode::Wave: return m_mainIcons.m_wave.getTexture();
            case FunnySpriteGamemode::Robot: return m_mainIcons.m_robot.getTexture();
            case FunnySpriteGamemode::Spider: return m_mainIcons.m_spider.getTexture();
            case FunnySpriteGamemode::Swing: return m_mainIcons.m_swing.getTexture();
            case FunnySpriteGamemode::Jetpack: return m_mainIcons.m_jetpack.getTexture();
            default: return 0;
        }
    } else {
        switch (gamemode) {
            case FunnySpriteGamemode::VehiclePassenger:
            case FunnySpriteGamemode::Cube: return m_dualIcons.m_cube.getTexture();
            case FunnySpriteGamemode::Ship: return m_dualIcons.m_ship.getTexture();
            case FunnySpriteGamemode::Ball: return m_dualIcons.m_ball.getTexture();
            case FunnySpriteGamemode::Ufo: return m_dualIcons.m_ufo.getTexture();
            case FunnySpriteGamemode::Wave: return m_dualIcons.m_wave.getTexture();
            case FunnySpriteGamemode::Robot: return m_dualIcons.m_robot.getTexture();
            case FunnySpriteGamemode::Spider: return m_dualIcons.m_spider.getTexture();
            case FunnySpriteGamemode::Swing: return m_dualIcons.m_swing.getTexture();
            case FunnySpriteGamemode::Jetpack: return m_dualIcons.m_jetpack.getTexture();
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

void FunnySpriteManager::init() {
    // add textures to cache
    auto cache = cocos2d::CCTextureCache::get();

    for (int i = 1; i <= 18; i++) {
        cache->addImage(fmt::format("{:04}.png"_spr, i).c_str(), false);
    }

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
            int index = array[i]["index"].asInt().unwrapOr(0);
            int ofIconType = array[i]["of"].asUInt().unwrapOr(0);

            geode::log::info("{} icon is {} of {}", i, index, ofIconType);

            m_icon[(IconType)i] = IconChoiceInfo{
                .m_index = index,
                .m_iconType = (IconType)ofIconType
            };
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

    // set CC_Texture1 and CC_Texture2
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
    updateRenderedSprites(m_mainIcons, false);
    updateRenderedSprites(m_dualIcons, true);
}

void FunnySpriteManager::updateRenderedSprites(RenderTextureGroup& group, bool dual) {
    updateRenderedSprite(group.m_cube, IconType::Cube, dual);
    updateRenderedSprite(group.m_ship, IconType::Ship, dual);
    updateRenderedSprite(group.m_ball, IconType::Ball, dual);
    updateRenderedSprite(group.m_ufo, IconType::Ufo, dual);
    updateRenderedSprite(group.m_wave, IconType::Wave, dual);
    updateRenderedSprite(group.m_robot, IconType::Robot, dual);
    updateRenderedSprite(group.m_spider, IconType::Spider, dual);
    updateRenderedSprite(group.m_swing, IconType::Swing, dual);
    updateRenderedSprite(group.m_jetpack, IconType::Jetpack, dual);
}

void FunnySpriteManager::updateRenderedSprite(RenderTexture& renderTexture, IconType gamemode, bool dual) {
    auto simplePlayer = SimplePlayer::create(0);
    auto playerSprite = simplePlayer->getChildrenExt()[0];

    auto gameManager = GameManager::get();

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

        auto settingName = iconTypeToName.at(m_icon[gamemode].m_iconType);
        simplePlayer->updatePlayerFrame(mod->getSavedValue<int64_t>(settingName), m_icon[gamemode].m_iconType);
    } else {
        simplePlayer->updatePlayerFrame(m_icon[gamemode].m_index, m_icon[gamemode].m_iconType);
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

    // so apparently mat rendertexture is only good if you're rendering
    // something the same size as the screen so we need to resize
    // TODO: look into mat rendertexture?

    // TODO: note: icon gradients queues in main thread
    // https://github.com/Zilko/icon-gradients/blob/main/src/Hooks/SimplePlayer.cpp#L19

    auto winSize = cocos2d::CCDirector::get()->getWinSize();
    simplePlayer->setPosition(winSize / 2.f);
    if (gameManager->getPlayerGlow()) {
        // add more space for glow
        simplePlayer->setScaleX(winSize.width / (playerSprite->getContentWidth() + 2.5f));
        simplePlayer->setScaleY(winSize.height / (playerSprite->getContentHeight() + 2.5f));
    } else {
        simplePlayer->setScaleX(winSize.width / (playerSprite->getContentWidth() + .5f));
        simplePlayer->setScaleY(winSize.height / (playerSprite->getContentHeight() + .5f));
    }

    renderTexture.capture(simplePlayer);
    simplePlayer->release();
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
