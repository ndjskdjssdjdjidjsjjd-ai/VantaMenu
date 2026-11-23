#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
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

// ---------- Macro bot + CPS (input level) ----------
class $modify(VantaBaseGameLayer, GJBaseGameLayer) {
    void handleButton(bool down, int button, bool isPlayer1) {
        auto pl = PlayLayer::get();
        if (pl && static_cast<GJBaseGameLayer*>(pl) == this) {
            int mode = vanta::macroMode();

            // during playback, ignore real taps so they can't mess up the run
            if (mode == 2 && !vanta::g_injecting) return;

            if (down) vanta::registerClick();

            if (mode == 1 && !vanta::g_injecting) {
                vanta::g_macro.push_back({vanta::g_frame, down, button, isPlayer1});
            }
        }
        GJBaseGameLayer::handleButton(down, button, isPlayer1);
    }

    void processCommands(float dt, bool isHalfTick, bool isLastTick) {
        auto pl = PlayLayer::get();
        if (pl && static_cast<GJBaseGameLayer*>(pl) == this) {
            if (vanta::macroMode() == 2) {
                while (vanta::g_playIndex < vanta::g_macro.size() &&
                       vanta::g_macro[vanta::g_playIndex].frame <= vanta::g_frame) {
                    auto a = vanta::g_macro[vanta::g_playIndex++];
                    vanta::g_injecting = true;
                    this->handleButton(a.down, a.button, a.p1);
                    vanta::g_injecting = false;
                }
            }
            GJBaseGameLayer::processCommands(dt, isHalfTick, isLastTick);
            vanta::g_frame++;
            return;
        }
        GJBaseGameLayer::processCommands(dt, isHalfTick, isLastTick);
    }
};

class $modify(VantaPlayLayer, PlayLayer) {
    struct Fields {
        CCLabelBMFont* label = nullptr;
        CCDrawNode* trailNode = nullptr;
        CCDrawNode* dot = nullptr;
        std::deque<CCRect> trail1;
        std::deque<CCRect> trail2;
        float time = 0.f;
        float fps = 60.f;
        bool hidden = false;
        bool hbWasOn = false;

        // noclip stats
        int ncFrames = 0;
        int ncHitFrames = 0;
        int ncDeaths = 0;
        bool hitNow = false;
        bool hitPrev = false;
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        m_fields->label = CCLabelBMFont::create("", "bigFont.fnt");
        m_uiLayer->addChild(m_fields->label, 100);

        m_fields->dot = CCDrawNode::create();
        m_uiLayer->addChild(m_fields->dot, 100);

        m_fields->trailNode = CCDrawNode::create();
        m_objectLayer->addChild(m_fields->trailNode, 1000);

        if (vanta::macroMode() == 2) {
            vanta::loadMacro(m_level->m_levelID.value());
        }
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        auto f = m_fields.self();
        f->trail1.clear();
        f->trail2.clear();
        f->time = 0.f;
        f->ncFrames = 0;
        f->ncHitFrames = 0;
        f->ncDeaths = 0;
        f->hitNow = false;
        f->hitPrev = false;

        vanta::g_frame = 0;
        vanta::g_playIndex = 0;
        if (vanta::macroMode() == 1) {
            vanta::g_macro.clear();
        }
    }

    void levelComplete() {
        // recorded run gets saved when you beat the level
        if (vanta::macroMode() == 1) {
            vanta::saveMacro(m_level->m_levelID.value());
        }
        PlayLayer::levelComplete();
    }

    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        if (vanta::isOn("noclip") && obj != m_anticheatSpike) {
            m_fields->hitNow = true; // would have died here
            return;
        }
        PlayLayer::destroyPlayer(player, obj);
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);
        auto f = m_fields.self();
        auto win = CCDirector::get()->getWinSize();
        bool dead = m_player1->m_isDead;

        // noclip stats
        if (vanta::isOn("noclip")) {
            f->ncFrames++;
            if (f->hitNow) {
                f->ncHitFrames++;
                if (!f->hitPrev) f->ncDeaths++;
            }
        }
        f->hitPrev = f->hitNow;
        f->hitNow = false;

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

        // hitbox trail: records while alive, shows with hitboxes AND on death
        if (f->trailNode) {
            f->trailNode->clear();
            if (vanta::isOn("hitbox-trail")) {
                if (!dead) {
                    size_t maxLen = (size_t)vanta::trailLength();
                    f->trail1.push_back(m_player1->getObjectRect());
                    if (m_player2 && m_player2->isVisible()) {
                        f->trail2.push_back(m_player2->getObjectRect());
                    }
                    while (f->trail1.size() > maxLen) f->trail1.pop_front();
                    while (f->trail2.size() > maxLen) f->trail2.pop_front();
                }
                if (vanta::isOn("show-hitbox") || dead) {
                    drawRects(f->trailNode, f->trail1, vanta::trailColor(false));
                    drawRects(f->trailNode, f->trail2, vanta::trailColor(true));
                }
            } else {
                f->trail1.clear();
                f->trail2.clear();
            }
        }

        // cheat indicator (top-left)
        if (f->dot) {
            f->dot->clear();
            if (vanta::isOn("cheat-indicator")) {
                bool cheating = vanta::isOn("noclip")
                    || vanta::g_speed != 1.f
                    || vanta::macroMode() != 0;
                ccColor4F col = cheating
                    ? ccColor4F{1.f, 0.2f, 0.2f, 1.f}
                    : ccColor4F{0.2f, 1.f, 0.4f, 1.f};
                f->dot->drawDot(ccp(12.f, win.height - 12.f), 4.f, col);
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
                if (vanta::isOn("label-cps")) s += fmt::format("CPS: {}\n", vanta::cps());
                if (vanta::isOn("label-time")) s += fmt::format("Time: {:.2f}s\n", f->time);
                if (vanta::isOn("label-attempt")) s += fmt::format("Attempt: {}\n", (int)m_attempts);
                if (vanta::isOn("label-percent")) s += fmt::format("Percent: {:.2f}%\n", this->getCurrentPercent());

                if (vanta::isOn("noclip")) {
                    if (vanta::isOn("label-nc-acc")) {
                        float acc = f->ncFrames > 0
                            ? 100.f * float(f->ncFrames - f->ncHitFrames) / float(f->ncFrames)
                            : 100.f;
                        s += fmt::format("NC Acc: {:.2f}%\n", acc);
                    }
                    if (vanta::isOn("label-nc-deaths")) {
                        s += fmt::format("NC Deaths: {}\n", f->ncDeaths);
                    }
                }

                if (vanta::isOn("label-macro")) {
                    int mode = vanta::macroMode();
                    if (mode == 1) {
                        s += fmt::format("Macro: Recording ({})\n", vanta::g_macro.size());
                    } else if (mode == 2) {
                        s += fmt::format("Macro: Playing {}/{}\n", vanta::g_playIndex, vanta::g_macro.size());
                    }
                }

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
                        f->label->setPosition({win.width - pad, win.height - pad});
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
                    default: // top left (shifted right to clear the indicator dot)
                        f->label->setAnchorPoint({0.f, 1.f});
                        f->label->setAlignment(kCCTextAlignmentLeft);
                        f->label->setPosition({22.f, win.height - pad});
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
