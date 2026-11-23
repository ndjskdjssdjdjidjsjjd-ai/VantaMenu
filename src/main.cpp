#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/GameManager.hpp>
#include "VantaPopup.hpp"
#include "Background.hpp"

using namespace geode::prelude;

// load enabled texture packs as early as possible
$on_mod(Loaded) {
    vanta::addEnabledPacks();
}

class $modify(VantaMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        static bool applied = false;
        if (!applied) {
            applied = true;
            vanta::applyAll();
        }

        if (auto bg = vanta::Background::create()) {
            this->addChild(bg, -100);
            for (auto child : CCArrayExt<CCNode*>(this->getChildren())) {
                if (typeinfo_cast<MenuGameLayer*>(child)) {
                    bg->setHideNode(child);
                    break;
                }
            }
        }

        addVantaButton(this);
        return true;
    }
};

class $modify(VantaPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();
        addVantaButton(this);
    }
};

class $modify(VantaScheduler, CCScheduler) {
    void update(float dt) {
        CCScheduler::update(dt * vanta::g_speed);
    }
};

class $modify(VantaGameManager, GameManager) {
    bool isIconUnlocked(int id, IconType type) {
        if (vanta::isOn("unlock-all")) return true;
        return GameManager::isIconUnlocked(id, type);
    }

    bool isColorUnlocked(int id, UnlockType type) {
        if (vanta::isOn("unlock-all")) return true;
        return GameManager::isColorUnlocked(id, type);
    }
};
