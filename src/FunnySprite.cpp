#include "FunnySprite.hpp"
#include "FunnySpriteManager.hpp"

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

    setShaderProgram(FunnySpriteManager::get().getMappingShader());

    return true;
}

void FunnySprite::updateForGamemode(IconType gamemode) {
    if (gamemode > IconType::Jetpack) return;
    updateForGamemode((FunnySpriteGamemode)gamemode);
}

void FunnySprite::updateForGamemode(FunnySpriteGamemode gamemode) {
    static const std::unordered_map<FunnySpriteGamemode, GamemodeInfo> gamemodeInfoMap = {
        { FunnySpriteGamemode::Cube, {
            .m_scale = 1.f,
            .m_offset = { 0.f, 0.f }
        } },
        { FunnySpriteGamemode::Ship, {
            .m_scale = 1.f,
            .m_offset = { 0.f, 0.f }
        } },
        { FunnySpriteGamemode::Ball, {
            .m_scale = 1.f,
            .m_offset = { 0.f, 0.f }
        } },
        { FunnySpriteGamemode::Ufo, {
            .m_scale = 1.f,
            .m_offset = { 0.f, 0.f }
        } },
        { FunnySpriteGamemode::Wave, {
            .m_scale = 1.f,
            .m_offset = { 0.f, 0.f }
        } },
        { FunnySpriteGamemode::Robot, {
            .m_scale = 1.f,
            .m_offset = { 0.f, 0.f }
        } },
        { FunnySpriteGamemode::Spider, {
            .m_scale = 1.f,
            .m_offset = { 0.f, 0.f }
        } },
        { FunnySpriteGamemode::Swing, {
            .m_scale = 1.f,
            .m_offset = { 0.f, 0.f }
        } },
        { FunnySpriteGamemode::Jetpack, {
            .m_scale = 1.f,
            .m_offset = { 0.f, 0.f }
        } },
        { FunnySpriteGamemode::CubePassenger, {
            .m_scale = .575f,
            .m_offset = { 0.f, 0.f }
        } }
    };

    if (!gamemodeInfoMap.contains(gamemode)) return;

    auto& gamemodeInfo = gamemodeInfoMap.at(gamemode);

    m_currentTexture = FunnySpriteManager::get().textureForGamemode(gamemode);
    m_currentMappingTexture = FunnySpriteManager::get().mappingTextureForGamemode(gamemode);

    setPosition(gamemodeInfo.m_offset);
    setScale(gamemodeInfo.m_scale);
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
