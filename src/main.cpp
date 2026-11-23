#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/GameManager.hpp>
#include "VantaPopup.hpp"

using namespace geode::prelude;

class $modify(VantaMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        static bool applied = false;
        if (!applied) {
            applied = true;
            vanta::applyAll();
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

class $modify(VantaPlayLayer, PlayLayer) {
    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        if (vanta::isOn("noclip") && obj != m_anticheatSpike) return;
        PlayLayer::destroyPlayer(player, obj);
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);
        static bool hidden = false;
        if (vanta::isOn("hide-player")) {
            m_player1->setVisible(false);
            hidden = true;
        } else if (hidden) {
            m_player1->setVisible(true);
            hidden = false;
        }
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
