#include "FunnySpriteManager.hpp"
#include "shaders.hpp"

// 32 * 4 * 4 = 512
// icon size * high graphics * 4
#define RENDERTEXTURE_INIT_PARAMS 512, 512, GL_RGBA, GL_RGBA, GL_NEAREST, GL_CLAMP_TO_EDGE

FunnySpriteManager::FunnySpriteManager()
    : m_cube(RENDERTEXTURE_INIT_PARAMS)
    , m_ship(RENDERTEXTURE_INIT_PARAMS)
    , m_ball(RENDERTEXTURE_INIT_PARAMS)
    , m_ufo(RENDERTEXTURE_INIT_PARAMS)
    , m_wave(RENDERTEXTURE_INIT_PARAMS)
    , m_robot(RENDERTEXTURE_INIT_PARAMS)
    , m_spider(RENDERTEXTURE_INIT_PARAMS)
    , m_swing(RENDERTEXTURE_INIT_PARAMS)
    , m_jetpack(RENDERTEXTURE_INIT_PARAMS)

    , m_wantsRealCountForType(false)
    , m_totalCountForTypes(0)

    , m_iconsForIconType({}) {}

#undef RENDERTEXTURE_IMAGE_PARAMS

FunnySpriteManager& FunnySpriteManager::get() {
    static FunnySpriteManager instance;
    return instance;
}

GLuint FunnySpriteManager::textureForGamemode(FunnySpriteGamemode gamemode) {
    switch (gamemode) {
        case FunnySpriteGamemode::CubePassenger:
        case FunnySpriteGamemode::Cube: return m_cube.getTexture();
        case FunnySpriteGamemode::Ship: return m_ship.getTexture();
        case FunnySpriteGamemode::Ball: return m_ball.getTexture();
        case FunnySpriteGamemode::Ufo: return m_ufo.getTexture();
        case FunnySpriteGamemode::Wave: return m_wave.getTexture();
        case FunnySpriteGamemode::Robot: return m_robot.getTexture();
        case FunnySpriteGamemode::Spider: return m_spider.getTexture();
        case FunnySpriteGamemode::Swing: return m_swing.getTexture();
        case FunnySpriteGamemode::Jetpack: return m_jetpack.getTexture();
        default: return 0;
    }
}

GLuint FunnySpriteManager::mappingTextureForGamemode(FunnySpriteGamemode gamemode) {
    auto cache = cocos2d::CCTextureCache::get();

    switch (gamemode) {
        case FunnySpriteGamemode::CubePassenger:
        case FunnySpriteGamemode::Cube: return cache->textureForKey("mapping-cube.png"_spr)->getName();
        case FunnySpriteGamemode::Ship: return cache->textureForKey("mapping-ship.png"_spr)->getName();
        case FunnySpriteGamemode::Ball: return cache->textureForKey("mapping-ball.png"_spr)->getName();
        case FunnySpriteGamemode::Ufo: return cache->textureForKey("mapping-ufo.png"_spr)->getName();
        case FunnySpriteGamemode::Wave: return cache->textureForKey("mapping-wave.png"_spr)->getName();
        case FunnySpriteGamemode::Robot: return cache->textureForKey("mapping-robot.png"_spr)->getName();
        case FunnySpriteGamemode::Spider: return cache->textureForKey("mapping-spider.png"_spr)->getName();
        case FunnySpriteGamemode::Swing: return cache->textureForKey("mapping-swing.png"_spr)->getName();
        case FunnySpriteGamemode::Jetpack: return cache->textureForKey("mapping-jetpack.png"_spr)->getName();
        default: return 0;
    }
}

void FunnySpriteManager::init() {
    // add textures to cache
    auto cache = cocos2d::CCTextureCache::get();

    cache->addImage("mapping-cube.png"_spr, false);
    cache->addImage("mapping-ship.png"_spr, false);
    cache->addImage("mapping-ball.png"_spr, false);
    cache->addImage("mapping-ufo.png"_spr, false);
    cache->addImage("mapping-wave.png"_spr, false);
    cache->addImage("mapping-robot.png"_spr, false);
    cache->addImage("mapping-spider.png"_spr, false);
    cache->addImage("mapping-swing.png"_spr, false);
    cache->addImage("mapping-jetpack.png"_spr, false);

    // create shader
    getMappingShader();

    // get initial rendered sprites
    updateRenderedSprites();

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

        // TODO: show flalertlayer on menulayer or something
    }

    mappingShader->addAttribute(kCCAttributeNamePosition, cocos2d::kCCVertexAttrib_Position);
    mappingShader->addAttribute(kCCAttributeNameColor, cocos2d::kCCVertexAttrib_Color);
    mappingShader->addAttribute(kCCAttributeNameTexCoord, cocos2d::kCCVertexAttrib_TexCoords);

    mappingShader->link();
    mappingShader->updateUniforms();

    // set CC_Texture1
    mappingShader->setUniformLocationWith1i(mappingShader->getUniformLocationForName("CC_Texture1"), 1);

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
    updateRenderedSprite(m_cube, IconType::Cube);
    updateRenderedSprite(m_ship, IconType::Ship);
    updateRenderedSprite(m_ball, IconType::Ball);
    updateRenderedSprite(m_ufo, IconType::Ufo);
    updateRenderedSprite(m_wave, IconType::Wave);
    updateRenderedSprite(m_robot, IconType::Robot);
    updateRenderedSprite(m_spider, IconType::Spider);
    updateRenderedSprite(m_swing, IconType::Swing);
    updateRenderedSprite(m_jetpack, IconType::Jetpack);
}

void FunnySpriteManager::updateRenderedSprite(RenderTexture& renderTexture, IconType gamemode) {
    auto simplePlayer = SimplePlayer::create(0);
    auto playerSprite = simplePlayer->getChildrenExt()[0];

    auto gameManager = GameManager::get();

    simplePlayer->updatePlayerFrame(m_icon[gamemode].m_index, m_icon[gamemode].m_iconType);
    simplePlayer->setColor(gameManager->colorForIdx(gameManager->getPlayerColor()));
    simplePlayer->setSecondColor(gameManager->colorForIdx(gameManager->getPlayerColor2()));
    simplePlayer->setGlowOutline(gameManager->colorForIdx(gameManager->getPlayerGlowColor()));
    if (!gameManager->getPlayerGlow()) simplePlayer->disableGlowOutline();

    // so apparently mat rendertexture is only good if you're rendering
    // something the same size as the screen ?
    auto winSize = cocos2d::CCDirector::get()->getWinSize();
    simplePlayer->setPosition(winSize / 2.f);
    simplePlayer->setScaleX(winSize.width / playerSprite->getContentWidth());
    simplePlayer->setScaleY(winSize.height / playerSprite->getContentHeight());

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
