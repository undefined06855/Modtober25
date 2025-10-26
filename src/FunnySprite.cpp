#include "FunnySprite.hpp"
#include "FunnySpriteManager.hpp"
#include <Geode/ui/GeodeUI.hpp>

FunnySprite* FunnySprite::create() {
    auto ret = new FunnySprite;
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool FunnySprite::init() {
    if (!CCSprite::initWithFile("blank.png"_spr)) return false;

    m_currentTexture = 0;
    m_currentMappingTexture = 0;
    m_currentTransparencyTexture = 0;
    m_currentGamemode = FunnySpriteGamemode::None;

    m_dual = false;
    m_mainOnly = false;

    m_limbs = {};

    m_animatedMappingTexture = 0;
    m_animatedTransparencyTexture = 0;
    m_isAnimating = false;
    m_animationPercentage = 0.f;
    m_animatedGamemode = FunnySpriteGamemode::None;

    m_ufoDome = cocos2d::CCSprite::createWithSpriteFrameName("bird_01_3_001.png");
    m_ufoDome->setID("dome");
    m_ufoDome->setPosition({ 16.f, 9.f });
    m_ufoDome->setScale(.775f);
    m_ufoDome->setVisible(false);
    m_ufoDome->setZOrder(-1);
    addChild(m_ufoDome);

    setShaderProgram(FunnySpriteManager::get().getMappingShader());

    scheduleUpdate();

    return true;
}

void FunnySprite::updateForGamemode(FunnySpriteGamemode gamemode, bool animate) {
    static const std::unordered_map<FunnySpriteGamemode, GamemodeInfo> gamemodeInfoMap = {
        { FunnySpriteGamemode::Cube, {
            .m_scale = 1.f,
            .m_offset = { 0.f, 0.f }
        } },
        { FunnySpriteGamemode::Ship, {
            .m_scale = 1.3f,
            .m_offset = { 0.f, -3.f }
        } },
        { FunnySpriteGamemode::Ball, {
            .m_scale = 1.f,
            .m_offset = { 0.f, 0.f }
        } },
        { FunnySpriteGamemode::Ufo, {
            .m_scale = 1.07f,
            .m_offset = { 0.f, 0.f }
        } },
        { FunnySpriteGamemode::Wave, {
            .m_scale = .86f,
            .m_offset = { 0.f, 0.f }
        } },
        { FunnySpriteGamemode::Robot, {
            .m_scale = 1.06f,
            .m_offset = { 0.f, 0.f }
        } },
        { FunnySpriteGamemode::Spider, {
            .m_scale = 1.15f,
            .m_offset = { 1.f, 0.f }
        } },
        { FunnySpriteGamemode::Swing, {
            .m_scale = 1.15f,
            .m_offset = { 0.f, 0.f }
        } },
        { FunnySpriteGamemode::Jetpack, {
            .m_scale = 1.f,
            .m_offset = { 0.f, 0.f }
        } },
        { FunnySpriteGamemode::JetpackPassenger, {
            .m_scale = .55f,
            .m_offset = { 0.f, 0.f }
        } },
        { FunnySpriteGamemode::ShipPassenger, {
            .m_scale = .6f,
            .m_offset = { 0.f, 3.f }
        } },
        { FunnySpriteGamemode::UfoPassenger, {
            .m_scale = .45f,
            .m_offset = { 0.f, 1.5f }
        } },
    };

    if (!gamemodeInfoMap.contains(gamemode)) return;

    auto& gamemodeInfo = gamemodeInfoMap.at(gamemode);

    auto& fsm = FunnySpriteManager::get();

    if (animate && m_currentMappingTexture != fsm.mappingTextureForGamemode(gamemode)) {
        animateToGamemode(gamemode);
    } else {
        m_currentMappingTexture = fsm.mappingTextureForGamemode(gamemode);
        m_currentTransparencyTexture = fsm.transparencyMaskForGamemode(gamemode);
    }

    m_currentTexture = fsm.textureForGamemode(gamemode, m_dual, m_mainOnly);

    m_ufoDome->setVisible(gamemode == FunnySpriteGamemode::Ufo && !m_mainOnly);

    // to allow us to use setScale and stuff to copy the original icon's
    // transforms, we'll use an additional transform
    auto transform = cocos2d::CCAffineTransformMakeIdentity();
    transform = cocos2d::CCAffineTransformTranslate(transform, gamemodeInfo.m_offset.x, gamemodeInfo.m_offset.y);
    transform = cocos2d::CCAffineTransformScale(transform, gamemodeInfo.m_scale, gamemodeInfo.m_scale);
    setAdditionalTransform(transform);

    m_currentGamemode = gamemode;

    // need a better blend func, aa looks weird but whatever this is good enough
    auto blendFunc = cocos2d::ccBlendFunc{ GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA };
    setBlendFunc(blendFunc);
}

void FunnySprite::animateToGamemode(FunnySpriteGamemode gamemode) {
    m_animatedGamemode = gamemode;
    m_animatedMappingTexture = FunnySpriteManager::get().mappingTextureForGamemode(gamemode);
    m_animatedTransparencyTexture = FunnySpriteManager::get().transparencyMaskForGamemode(gamemode);

    m_currentTexture = FunnySpriteManager::get().textureForGamemode(gamemode, m_dual, m_mainOnly);

    if (!m_isAnimating) m_animationPercentage = 0.f;
    m_isAnimating = true;
}

// violence subroutine
void FunnySprite::addLimbs(FunnySpriteGamemode gamemode) {
    // remove any extra limbs
    for (auto limb : m_limbs) limb->removeFromParent();
    m_limbs.clear();

    if (gamemode != FunnySpriteGamemode::Robot && gamemode != FunnySpriteGamemode::Spider) {
        return;
    }

    // create the victim
    GJRobotSprite* victim;
    if (gamemode == FunnySpriteGamemode::Spider) {
        victim = GJSpiderSprite::create(1);
    } else {
        victim = GJRobotSprite::create(1);
    }

    // make the victim look nice before we take off its limbs
    auto gameManager = GameManager::get();
    victim->updateColor01(gameManager->colorForIdx(gameManager->getPlayerColor()));
    victim->updateColor02(gameManager->colorForIdx(gameManager->getPlayerColor2()));
    victim->updateGlowColor(gameManager->colorForIdx(gameManager->getPlayerGlowColor()), false);
    victim->hideGlow();

    // take off the limbs (except the head) and add to ourself
    for (auto child : geode::cocos::CCArrayExt<cocos2d::CCNode*>(victim->m_paSprite->m_spriteParts)) {
        if (child == victim->m_headSprite) continue;

        child->setZOrder(child->getZOrder() - 3);
        child->setPosition(child->getPosition() + cocos2d::CCPoint{ 16.f, 16.f });

        // surgically detach the limbs
        child->removeFromParent();
        m_limbs.push_back(child);
        addChild(child);
    }
}

// taken from icon ninja
unsigned int* getNumberOfDraws() {
    // compiler yells at me if i use #elifdef :broken_heart:
    // thanks to jasmine for ALL of these
#if defined(GEODE_IS_MACOS)
    static_assert(GEODE_COMP_GD_VERSION == 22074, "Please update macOS offsets");
    return reinterpret_cast<unsigned int*>(geode::base::get() + GEODE_ARM_MAC(0x8b0f60) GEODE_INTEL_MAC(0x98bf30));
#elif defined(GEODE_IS_IOS)
    static_assert(GEODE_COMP_GD_VERSION == 22074, "Please update iOS offsets");
    return reinterpret_cast<unsigned int*>(geode::base::get() + 0x8791d0);
#else
    return &g_uNumberOfDraws;
#endif
}

void FunnySprite::draw() {
    CC_NODE_DRAW_SETUP();

    cocos2d::ccGLBlendFunc(m_sBlendFunc.src, m_sBlendFunc.dst);

    // note https://discord.com/channels/911701438269386882/911702535373475870/1421852242998329504
    // but for this to work in a batch node (or our fake batch node) we need to
    // use ccglbindtexture2dn instead of manual opengl for texture to get set
    // properly on every sprite after
    cocos2d::ccGLBindTexture2DN(0, m_currentTexture);
    cocos2d::ccGLBindTexture2DN(1, m_currentMappingTexture);
    cocos2d::ccGLBindTexture2DN(2, m_currentTransparencyTexture);
    cocos2d::ccGLBindTexture2DN(3, m_animatedMappingTexture);
    cocos2d::ccGLBindTexture2DN(4, m_animatedTransparencyTexture);

    m_pShaderProgram->setUniformLocationWith1f(m_pShaderProgram->getUniformLocationForName("u_animationPercentage"), m_animationPercentage);

    cocos2d::ccGLEnableVertexAttribs(cocos2d::kCCVertexAttribFlag_PosColorTex);

    // vertex
    int diff = offsetof(cocos2d::ccV3F_C4B_T2F, vertices);
    glVertexAttribPointer(cocos2d::kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, sizeof(m_sQuad.bl), (void*)((uintptr_t)&m_sQuad + diff));

    // texCoods
    diff = offsetof(cocos2d::ccV3F_C4B_T2F, texCoords);
    glVertexAttribPointer(cocos2d::kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(m_sQuad.bl), (void*)((uintptr_t)&m_sQuad + diff));

    // color
    diff = offsetof(cocos2d::ccV3F_C4B_T2F, colors);
    glVertexAttribPointer(cocos2d::kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(m_sQuad.bl), (void*)((uintptr_t)&m_sQuad + diff));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    *getNumberOfDraws() += 1;
}

void FunnySprite::update(float dt) {
    if (!m_isAnimating) return;

    static const float animationRate = 1.f / .2f; // 0.2 seconds

    m_animationPercentage += dt * animationRate;

    if (m_animationPercentage >= 1.f) {
        m_isAnimating = false;
        m_animationPercentage = 1.f;

        m_currentGamemode = m_animatedGamemode;
        m_currentMappingTexture = m_animatedMappingTexture;
        m_currentTransparencyTexture = m_animatedTransparencyTexture;
    }
}
