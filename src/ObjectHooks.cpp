#include <Geode/Geode.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "Features.hpp"

using namespace geode::prelude;

// Building Opacity: scales the opacity of blocks and decoration in levels.
// Hazards, portals and pads are left alone so the level stays readable.
class $modify(VantaGameObject, GameObject) {
    void setOpacity(unsigned char op) {
        if (PlayLayer::get()) {
            float k = vanta::objOpacity();
            if (k < 1.f &&
                (m_objectType == GameObjectType::Solid || m_objectType == GameObjectType::Decoration)) {
                op = (unsigned char)(op * k);
            }
        }
        GameObject::setOpacity(op);
    }
};
