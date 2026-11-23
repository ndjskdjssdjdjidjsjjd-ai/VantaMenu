#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/GameManager.hpp>
#include <deque>
#include "VantaPopup.hpp"

using namespace geode::prelude;

static void drawRects(CCDrawNode* node, std::deque<CCRect> const& rects, ccColor4F color) {
    size_t n = rects.size();
    for (size_t i = 0; i < n; i++) {
        auto const& r = rects[i];
        float a = 0.15f + 0.85f * (float(i + 1) / float(n));
        CCPoint pts[4] = {
            ccp(r.getMinX(), r.getMinY()),
            ccp(r.getMaxX(), r.getMinY()),
            ccp(r.getMaxX(), r.getMaxY()),
            ccp(r.getMinX(), r.getMaxY())
        };
        node->drawPolygon(pts, 4, ccColor4F{0.f, 0.f, 0.f, 0.f}, 0.5f, ccColor4F{color.r, color.g, color.b, a});
    }
}

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
    struct Fields {
        CCLabelBMFont* label = nullptr;
        CCDrawNode* trailNode = nullptr;
        CCDrawNode* dot = nullptr;
        CCMenu* hbMenu = nullptr;
        std::deque<CCRect> trail1;
        std::deque<CCRect> trail2;
        float time = 0.f;
        float fps = 60.f;
        bool hidden = false;
        bool hbWasOn = false;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        auto win = CCDirector::get()->getWinSize();

        // labels
        m_fields->label = CCLabelBMFont::create("", "bigFont.fnt");
        m_uiLayer->addChild(m_fields->label, 100);

        // cheat indicator dot
        m_fields->dot = CCDrawNode::create();
        m_uiLayer->addChild(m_fields->dot, 100);

        // hitbox trail (moves with the level)
        m_fields->trailNode = CCDrawNode::create();
        m_objectLayer->addChild(m_fields->trailNode, 1000);

        // on-screen hitbox button
        auto menu = CCMenu::create();
        auto spr = ButtonSprite::create("Hitbox", 55, true, "bigFont.fnt", "GJ_button_04.png", 22.f, .5f);
        auto btn = CCMenuItemExt::createSpriteExtra(spr, [](CCMenuItemSpriteExtra*) {
            bool now = !vanta::isOn("show-hitbox");
            Mod::get()->setSavedValue<bool>("show-hitbox", now);
        });
        menu->addChild(btn);
        menu->setPosition({win.width - 40.f, win.height / 2.f});
        m_uiLayer->addChild(menu, 100);
        m_fields->hbMenu = menu;
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        m_fields->trail1.clear();
        m_fields->trail2.clear();
        m_fields->time = 0.f;
    }

    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        if (vanta::isOn("noclip") && obj != m_anticheatSpike) return;
        PlayLayer::destroyPlayer(player, obj);
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);
        auto f = m_fields.self();
        auto win = CCDirector::get()->getWinSize();

        // hide player
        if (vanta::isOn("hide-player")) {
            m_player1->setVisible(false);
            f->hidden = true;
        } else if (f->hidden) {
            m_player1->setVisible(true);
            f->hidden = false;
        }

        // show hitboxes
        if (vanta::isOn("show-hitbox")) {
            m_isDebugDrawEnabled = true;
            if (m_debugDrawNode) m_debugDrawNode->setVisible(true);
            this->updateDebugDraw();
            f->hbWasOn = true;
        } else if (f->hbWasOn) {
            f->hbWasOn = false;
            m_isDebugDrawEnabled = false;
            if (m_debugDrawNode) {
                m_debugDrawNode->clear();
                m_debugDrawNode->setVisible(false);
            }
        }

        // hitbox button visibility
        if (f->hbMenu) f->hbMenu->setVisible(vanta::isOn("hitbox-button"));

        // hitbox trail
        if (f->trailNode) {
            f->trailNode->clear();
            if (vanta::isOn("hitbox-trail")) {
                size_t maxLen = (size_t)vanta::trailLength();
                f->trail1.push_back(m_player1->getObjectRect());
                if (m_player2 && m_player2->isVisible()) {
                    f->trail2.push_back(m_player2->getObjectRect());
                }
                while (f->trail1.size() > maxLen) f->trail1.pop_front();
                while (f->trail2.size() > maxLen) f->trail2.pop_front();
                drawRects(f->trailNode, f->trail1, ccColor4F{0.3f, 0.9f, 1.f, 1.f});
                drawRects(f->trailNode, f->trail2, ccColor4F{1.f, 0.4f, 0.8f, 1.f});
            } else {
                f->trail1.clear();
                f->trail2.clear();
            }
        }

        // cheat indicator
        if (f->dot) {
            f->dot->clear();
            if (vanta::isOn("cheat-indicator")) {
                bool cheating = vanta::isOn("noclip") || vanta::g_speed != 1.f;
                ccColor4F col = cheating
                    ? ccColor4F{1.f, 0.2f, 0.2f, 1.f}
                    : ccColor4F{0.2f, 1.f, 0.4f, 1.f};
                f->dot->drawDot(ccp(win.width - 12.f, win.height - 12.f), 4.f, col);
            }
        }

        // labels
        f->time += dt;
        float real = CCDirector::get()->getDeltaTime();
        if (real > 0.f) f->fps = f->fps * 0.9f + (1.f / real) * 0.1f;

        if (f->label) {
            if (!vanta::isOn("labels")) {
                f->label->setVisible(false);
            } else {
                std::string s;
                if (vanta::isOn("label-fps")) s += fmt::format("FPS: {}\n", (int)f->fps);
                if (vanta::isOn("label-time")) s += fmt::format("Time: {:.2f}s\n", f->time);
                if (vanta::isOn("label-attempt")) s += fmt::format("Attempt: {}\n", (int)m_attempts);
                if (vanta::isOn("label-percent")) s += fmt::format("Percent: {:.2f}%\n", this->getCurrentPercent());

                f->label->setVisible(true);
                f->label->setString(s.c_str());
                f->label->setScale(0.35f * vanta::labelScale());
                f->label->setOpacity((GLubyte)(255.f * vanta::labelOpacity()));
                f->label->setColor(vanta::labelColor());

                int pos = vanta::getIndex("label-pos");
                float pad = 6.f;
                switch (pos) {
                    case 1: // top right
                        f->label->setAnchorPoint({1.f, 1.f});
                        f->label->setAlignment(kCCTextAlignmentRight);
                        f->label->setPosition({win.width - 24.f, win.height - pad});
                        break;
                    case 2: // bottom left
                        f->label->setAnchorPoint({0.f, 0.f});
                        f->label->setAlignment(kCCTextAlignmentLeft);
                        f->label->setPosition({pad, pad});
                        break;
                    case 3: // bottom right
                        f->label->setAnchorPoint({1.f, 0.f});
                        f->label->setAlignment(kCCTextAlignmentRight);
                        f->label->setPosition({win.width - pad, pad});
                        break;
                    default: // top left
                        f->label->setAnchorPoint({0.f, 1.f});
                        f->label->setAlignment(kCCTextAlignmentLeft);
                        f->label->setPosition({pad, win.height - pad});
                        break;
                }
            }
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
