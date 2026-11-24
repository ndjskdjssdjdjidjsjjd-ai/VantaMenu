#include <Geode/Geode.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <algorithm>
#include <cmath>
#include "Features.hpp"

using namespace geode::prelude;

// Building Opacity: custom 0-100% opacity for blocks / decoration / everything but hazards.
// Modes: Uniform, Clear Near Player (see through blocks around you), Clear Far From Player.
class $modify(VantaGameObject, GameObject) {
    void setOpacity(unsigned char op) {
        auto pl = PlayLayer::get();
        if (pl && vanta::objAffects(m_objectType)) {
            float base = vanta::objOpacity();
            if (base < 1.f) {
                float k = base;
                int mode = vanta::getIndex("obj-mode");
                if (mode != 0 && pl->m_player1) {
                    float dx = this->getPositionX() - pl->m_player1->getPositionX();
                    float dy = this->getPositionY() - pl->m_player1->getPositionY();
                    float d = std::sqrt(dx * dx + dy * dy);
                    float t = std::min(1.f, d / 160.f);
                    if (mode == 1) k = base + (1.f - base) * t;
                    else k = 1.f - (1.f - base) * t;
                }
                op = (unsigned char)(op * k);
            }
        }
        GameObject::setOpacity(op);
    }
};
