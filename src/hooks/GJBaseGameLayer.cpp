#include "GJBaseGameLayer.hpp"

HookedGJBaseGameLayer::Fields::Fields()
    : m_creatingPlayers(false) {}

void HookedGJBaseGameLayer::createPlayer() {
    auto fields = m_fields.self();
    fields->m_creatingPlayers = true;
    GJBaseGameLayer::createPlayer();
    fields->m_creatingPlayers = false;
}
