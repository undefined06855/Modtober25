#pragma once
#include <Geode/modify/ProfilePage.hpp>
#include "../FunnySprite.hpp"

class $modify(HookedProfilePage, ProfilePage) {
    void loadPageFromUserInfo(GJUserScore* info);
};
