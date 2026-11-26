#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <algorithm>
#include <cmath>
#include <deque>
#include <vector>
#include "TrailDraw.hpp"
#include "Labels.hpp"

using namespace geode::prelude;

class $modify(VantaPlayLayer, PlayLayer) {
    struct Fields {
        CCLabelBMFont* label = nullptr;
        CCDrawNode* trailNode = nullptr;
        CCDrawNode* dot = nullptr;
        CCLayerColor* dim = nullptr;
        CCMenu* spMenu = nullptr;
        CCLabelBMFont* spLabel = nullptr;
        std::vector<StartPosObject*> sp;
        int spIdx = -1;
        int spShown = -2;
        std::deque<vanta::TrailPt> trail1;
        std::deque<vanta::TrailPt> trail2;
        std::vector<vanta::HbFx> fx;
        CCPoint prevC = ccp(0.f, 0.f);
        CCPoint vel = ccp(0.f, 0.f);
        bool havePrev = false;
        float time = 0.f;
        float fps = 60.f;
        bool hidden = false;
        bool hbWasOn = false;
        int ncFrames = 0;
        int ncHitFrames = 0;
        int ncDeaths = 0;
        bool hitNow = false;
        bool hitPrev = false;
        CCRect deathRect;
        bool hasDeath = false;
        int frameCounter = 0;
        int objOpIdx = -1;
    };

    void showNewBest(bool newReward, int orbs, int diamonds, bool demonKey, bool gauntlet, bool special) {
        if (vanta::isOn("no-newbest-orbs")) {
            newReward = false;
            orbs = 0;
            diamonds = 0;
        }
        PlayLayer::showNewBest(newReward, orbs, diamonds, demonKey, gauntlet, special);
    }

    // collect Start Pos objects while the level loads
    void addObject(GameObject* obj) {
        PlayLayer::addObject(obj);
        if (obj && obj->m_objectID == 31) {
            if (auto sp = typeinfo_cast<StartPosObject*>(obj)) {
                m_fields->sp.push_back(sp);
            }
        }
    }

    void vantaSwitch(int dir) {
        auto f = m_fields.self();
        int n = (int)f->sp.size();
        if (n == 0) return;
        f->spIdx += dir;
        if (f->spIdx < -1) f->spIdx = n - 1;
        if (f->spIdx >= n) f->spIdx = -1;
        this->setStartPosObject(f->spIdx >= 0 ? f->sp[f->spIdx] : nullptr);
        this->resetLevel();
    }

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();
        auto f = m_fields.self();
        auto win = CCDirector::get()->getWinSize();

        f->dim = CCLayerColor::create(ccc4(0, 0, 0, 255), win.width, win.height);
        f->dim->setOpacity(0);
        m_uiLayer->addChild(f->dim, -20);

        f->label = CCLabelBMFont::create("", "bigFont.fnt");
        m_uiLayer->addChild(f->label, 100);

        f->dot = CCDrawNode::create();
        m_uiLayer->addChild(f->dot, 100);

        f->trailNode = CCDrawNode::create();
        m_objectLayer->addChild(f->trailNode, 1000);

        // Start Pos switcher UI
        std::sort(f->sp.begin(), f->sp.end(), [](StartPosObject* a, StartPosObject* b) {
            return a->getPositionX() < b->getPositionX();
        });

        auto menu = CCMenu::create();
        menu->setPosition({0.f, 0.f});

        auto left = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        left->setScale(0.55f);
        auto lb = CCMenuItemExt::createSpriteExtra(left, [this](CCMenuItemSpriteExtra*) {
            this->vantaSwitch(-1);
        });
        lb->setPosition({win.width / 2.f - 70.f, 24.f});
        menu->addChild(lb);

        auto right = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        right->setScale(0.55f);
        right->setFlipX(true);
        auto rb = CCMenuItemExt::createSpriteExtra(right, [this](CCMenuItemSpriteExtra*) {
            this->vantaSwitch(1);
        });
        rb->setPosition({win.width / 2.f + 70.f, 24.f});
        menu->addChild(rb);

        m_uiLayer->addChild(menu, 100);
        f->spMenu = menu;

        f->spLabel = CCLabelBMFont::create("", "goldFont.fnt");
        f->spLabel->setScale(0.45f);
        f->spLabel->setPosition({win.width / 2.f, 24.f});
        m_uiLayer->addChild(f->spLabel, 100);

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
        f->hasDeath = false;
        f->frameCounter = 0;
        f->fx.clear();
        f->havePrev = false;
        f->vel = ccp(0.f, 0.f);

        vanta::g_hold1 = false;
        vanta::g_hold2 = false;
        vanta::g_frame = 0;
        vanta::g_playIndex = 0;
        if (vanta::macroMode() == 1) {
            vanta::g_macro.clear();
        }
    }

    void levelComplete() {
        if (vanta::macroMode() == 1) {
            vanta::saveMacro(m_level->m_levelID.value());
        }
        PlayLayer::levelComplete();
    }

    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        if (vanta::isOn("noclip") && obj != m_anticheatSpike) {
            m_fields->hitNow = true;
            return;
        }
        if (obj && obj != m_anticheatSpike) {
            m_fields->deathRect = obj->getObjectRect();
            m_fields->hasDeath = true;
        }
        if (vanta::isOn("show-hitbox") && vanta::isOn("hb-anim")) {
            CCPoint pc = vanta::centerOf(m_player1->getObjectRect());
            vanta::spawnShards(m_fields->fx, pc);
            vanta::spawnRing(m_fields->fx, pc, 1, 36.f, 0.45f);
            if (obj && obj != m_anticheatSpike) {
                vanta::spawnRing(m_fields->fx, vanta::centerOf(obj->getObjectRect()), 1, 46.f, 0.5f);
            }
        }
        PlayLayer::destroyPlayer(player, obj);

        // custom respawn time
        if (vanta::isOn("respawn-custom") && m_player1->m_isDead) {
            this->stopAllActions();
            auto delay = CCDelayTime::create(vanta::respawnTime());
            auto call = CCCallFunc::create(this, callfunc_selector(PlayLayer::delayedResetLevel));
            this->runAction(CCSequence::create(delay, call, nullptr));
        }
    }

    // faded blocks keep a thin outline so the level stays readable
    void vantaDrawOutlines(CCDrawNode* node) {
        float px = m_player1->getPositionX();
        float k = 1.f - vanta::objOpacity();
        for (auto obj : CCArrayExt<GameObject*>(m_objects)) {
            float x = obj->getPositionX();
            if (x < px - 260.f || x > px + 700.f) continue;
            auto t = obj->m_objectType;
            if (t == GameObjectType::Decoration || !vanta::objAffects(t)) continue;
            CCRect r = obj->getObjectRect();
            if (r.size.width < 1.f || r.size.height < 1.f) continue;
            vanta::strokeBox(node, r, 0.f, 0.8f, ccColor4F{1.f, 1.f, 1.f, 0.35f * k});
        }
    }

    // styled hitboxes for blocks, hazards and special objects near the player
    void vantaDrawObjects(CCDrawNode* node) {
        float px = m_player1->getPositionX();
        CCPoint pc = vanta::centerOf(m_player1->getObjectRect());
        float lo = px - 260.f;
        float hi = px + 700.f;
        for (auto obj : CCArrayExt<GameObject*>(m_objects)) {
            float x = obj->getPositionX();
            if (x < lo || x > hi) continue;
            auto t = obj->m_objectType;
            if (t == GameObjectType::Decoration) continue;
            int kind = 2;
            if (t == GameObjectType::Hazard) kind = 1;
            else if (t == GameObjectType::Solid || t == GameObjectType::Slope) kind = 0;
            CCRect rr = obj->getObjectRect();
            CCPoint oc = vanta::centerOf(rr);
            float ddx = oc.x - pc.x;
            float ddy = oc.y - pc.y;
            float dist = std::sqrt(ddx * ddx + ddy * ddy);
            float prox = std::max(0.f, 1.f - dist / 110.f);
            vanta::objectBox(node, rr, kind, prox);
        }
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);
        auto f = m_fields.self();
        auto win = CCDirector::get()->getWinSize();
        bool dead = m_player1->m_isDead;

        // brightness overlay
        if (f->dim) {
            GLubyte a = vanta::dimAlpha();
            f->dim->setOpacity(a);
            f->dim->setVisible(a > 0);
        }

        // building opacity
        float opK = vanta::objOpacity();
        int opMode = vanta::getIndex("obj-mode");
        int oi = (int)(opK * 100.f) * 100 + vanta::getIndex("obj-target") * 10 + opMode;
        if (oi != f->objOpIdx) {
            f->objOpIdx = oi;
            for (auto obj : CCArrayExt<GameObject*>(m_objects)) {
                if (vanta::objAffects(obj->m_objectType)) obj->setOpacity(255);
            }
        } else if (opK < 1.f && opMode != 0) {
            float ppx = m_player1->getPositionX();
            for (auto obj : CCArrayExt<GameObject*>(m_objects)) {
                float ox = obj->getPositionX();
                if (ox < ppx - 220.f || ox > ppx + 220.f) continue;
                if (vanta::objAffects(obj->m_objectType)) obj->setOpacity(255);
            }
        }

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

        // the game's own hitboxes
        if (vanta::isOn("show-hitbox") && !vanta::isOn("hb-objects")) {
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

        float rdt = CCDirector::get()->getDeltaTime();
        vanta::g_anim += rdt;

        CCPoint cNow = vanta::centerOf(m_player1->getObjectRect());
        if (f->havePrev) {
            f->vel = ccp(cNow.x - f->prevC.x, cNow.y - f->prevC.y);
        }
        f->prevC = cNow;
        f->havePrev = true;

        // hitboxes: everything here only shows when Show Hitboxes is on
        bool master = vanta::isOn("show-hitbox");
        bool trailOn = vanta::isOn("hitbox-trail");
        bool deathTrail = false;
        bool visible = master || deathTrail;
        bool animOn = vanta::isOn("hb-anim");
        if (vanta::g_click1) {
            vanta::g_click1 = false;
            if (master && animOn) {
                vanta::spawnRing(f->fx, vanta::centerOf(m_player1->getObjectRect()), 0, 26.f, 0.3f);
            }
        }
        if (vanta::g_click2) {
            vanta::g_click2 = false;
            if (master && animOn && m_player2 && m_player2->isVisible()) {
                vanta::spawnRing(f->fx, vanta::centerOf(m_player2->getObjectRect()), 0, 26.f, 0.3f);
            }
        }
        if (f->trailNode) {
            f->trailNode->clear();
            if (vanta::isOn("obj-outline") && vanta::objOpacity() < 1.f) {
                this->vantaDrawOutlines(f->trailNode);
            }
            if (master && vanta::isOn("hb-objects")) {
                this->vantaDrawObjects(f->trailNode);
            }
            if (trailOn) {
                if (!dead) {
                    size_t maxLen = (size_t)vanta::trailLength();
                    f->trail1.push_back({m_player1->getObjectRect(), vanta::g_hold1, f->frameCounter});
                    if (m_player2 && m_player2->isVisible()) {
                        f->trail2.push_back({m_player2->getObjectRect(), vanta::g_hold2, f->frameCounter});
                    }
                    f->frameCounter++;
                    while (f->trail1.size() > maxLen) f->trail1.pop_front();
                    while (f->trail2.size() > maxLen) f->trail2.pop_front();
                }
                if (visible) {
                    vanta::drawTrail(f->trailNode, f->trail1, false, dead);
                    vanta::drawTrail(f->trailNode, f->trail2, true, dead);
                }
            } else {
                f->trail1.clear();
                f->trail2.clear();
            }
            if (master && vanta::isOn("hb-player")) {
                vanta::drawPlayerBox(f->trailNode, m_player1, false, vanta::g_hold1);
                if (m_player2 && m_player2->isVisible()) {
                    vanta::drawPlayerBox(f->trailNode, m_player2, true, vanta::g_hold2);
                }
            }
            if (visible && dead && f->hasDeath && vanta::isOn("trail-deathmark")) {
                vanta::drawDeath(f->trailNode, f->deathRect, m_player1->getObjectRect());
            }
            if (master && vanta::isOn("hb-velocity") && !dead) {
                vanta::drawArrow(f->trailNode, cNow, f->vel);
            }
            if (master && animOn) {
                vanta::drawFx(f->trailNode, f->fx, rdt);
            } else {
                f->fx.clear();
            }
        }

        // Start Pos switcher UI
        if (f->spMenu && f->spLabel) {
            bool show = vanta::isOn("startpos") && !f->sp.empty();
            f->spMenu->setVisible(show);
            f->spLabel->setVisible(show);
            if (show && f->spShown != f->spIdx) {
                f->spShown = f->spIdx;
                if (f->spIdx < 0) {
                    f->spLabel->setString("StartPos: Off");
                } else {
                    f->spLabel->setString(fmt::format("StartPos {}/{}", f->spIdx + 1, f->sp.size()).c_str());
                }
            }
        }

        // cheat indicator: circle (top-left)
        if (f->dot) {
            f->dot->clear();
            if (vanta::isOn("cheat-indicator")) {
                bool cheating = vanta::isOn("noclip")
                    || vanta::g_speed != 1.f
                    || vanta::macroMode() != 0;
                ccColor4F col = cheating
                    ? ccColor4F{1.f, 0.2f, 0.2f, 1.f}
                    : ccColor4F{0.2f, 1.f, 0.4f, 1.f};
                CCPoint pos = ccp(14.f, win.height - 14.f);
                CCPoint ring[32];
                for (int i = 0; i < 32; i++) {
                    float a = i * 6.2831853f / 32.f;
                    ring[i] = ccp(pos.x + std::cos(a) * 6.f, pos.y + std::sin(a) * 6.f);
                }
                f->dot->drawPolygon(ring, 32, col, 0.f, ccColor4F{0.f, 0.f, 0.f, 0.f});
            }
        }

        // labels
        f->time += dt;
        float real = CCDirector::get()->getDeltaTime();
        if (real > 0.f) f->fps = f->fps * 0.9f + (1.f / real) * 0.1f;

        if (f->label) {
            vanta::LabelData d;
            d.fps = f->fps;
            d.time = f->time;
            d.attempts = (int)m_attempts;
            d.percent = this->getCurrentPercent();
            d.x = m_player1->getPositionX();
            d.y = m_player1->getPositionY();
            d.velY = rdt > 0.f ? f->vel.y / rdt : 0.f;
            d.ncFrames = f->ncFrames;
            d.ncHit = f->ncHitFrames;
            d.ncDeaths = f->ncDeaths;
            vanta::updateLabel(f->label, d, win);
        }
    }
};
