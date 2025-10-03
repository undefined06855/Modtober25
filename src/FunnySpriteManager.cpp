#include "FunnySpriteManager.hpp"
#include "shaders.hpp"

FunnySpriteManager::FunnySpriteManager()
    : m_cube(128, 128, GL_RGBA, GL_RGBA, GL_NEAREST, GL_CLAMP_TO_EDGE)
    , m_ship(128, 128, GL_RGBA, GL_RGBA, GL_NEAREST, GL_CLAMP_TO_EDGE)
    , m_ball(128, 128, GL_RGBA, GL_RGBA, GL_NEAREST, GL_CLAMP_TO_EDGE)
    , m_ufo(128, 128, GL_RGBA, GL_RGBA, GL_NEAREST, GL_CLAMP_TO_EDGE)
    , m_wave(128, 128, GL_RGBA, GL_RGBA, GL_NEAREST, GL_CLAMP_TO_EDGE)
    , m_robot(128, 128, GL_RGBA, GL_RGBA, GL_NEAREST, GL_CLAMP_TO_EDGE)
    , m_spider(128, 128, GL_RGBA, GL_RGBA, GL_NEAREST, GL_CLAMP_TO_EDGE)
    , m_swing(128, 128, GL_RGBA, GL_RGBA, GL_NEAREST, GL_CLAMP_TO_EDGE)
    , m_jetpack(128, 128, GL_RGBA, GL_RGBA, GL_NEAREST, GL_CLAMP_TO_EDGE) {}

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

    // and create shader
    m_mappingShader = new cocos2d::CCGLProgram;
    bool ret = m_mappingShader->initWithVertexShaderByteArray(g_mappingShaderVertex, g_mappingShaderFragment);
    if (!ret) {
        geode::log::error("Shader failed to load!");
        geode::log::error("{}", m_mappingShader->fragmentShaderLog()); // probably going to crash anyway

        // TODO: show flalertlayer on menulayer or something
        // TODO: remake when gl context changes
    }

    m_mappingShader->addAttribute(kCCAttributeNamePosition, cocos2d::kCCVertexAttrib_Position);
    m_mappingShader->addAttribute(kCCAttributeNameColor, cocos2d::kCCVertexAttrib_Color);
    m_mappingShader->addAttribute(kCCAttributeNameTexCoord, cocos2d::kCCVertexAttrib_TexCoords);

    m_mappingShader->link();
    m_mappingShader->updateUniforms();

    // set CC_Texture1
    m_mappingShader->setUniformLocationWith1i(m_mappingShader->getUniformLocationForName("CC_Texture1"), 1);

    updateRenderedSprites();
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

    int frame = -1;
    auto gameManager = GameManager::get();

    // TODO: make this what the player selects in the extended icon kit
    frame = gameManager->getPlayerShip();
    auto selectedGamemode = IconType::Ship;

    simplePlayer->updatePlayerFrame(frame, selectedGamemode);
    simplePlayer->setColor(gameManager->colorForIdx(gameManager->getPlayerColor()));
    simplePlayer->setSecondColor(gameManager->colorForIdx(gameManager->getPlayerColor2()));
    simplePlayer->setGlowOutline(gameManager->colorForIdx(gameManager->getPlayerGlowColor()));
    if (!gameManager->getPlayerGlow()) simplePlayer->disableGlowOutline();

    // so apparently mat rendertexture is only good if you're rendering
    // something the same size as the screen ?
    auto winSize = cocos2d::CCDirector::get()->getWinSize();
    simplePlayer->setPosition(winSize / 2.f);
    simplePlayer->setScaleX(winSize.width / 32.f);
    simplePlayer->setScaleY(winSize.height / 32.f);

    renderTexture.capture(simplePlayer);
    simplePlayer->release();
}

$on_mod(Loaded) {
    FunnySpriteManager::get().init();
}
