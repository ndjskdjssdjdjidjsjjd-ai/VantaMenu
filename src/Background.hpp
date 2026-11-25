#pragma once
#include "BgCore.hpp"
#include "BgThemes1.hpp"
#include "BgThemes2.hpp"
#include "BgThemes3.hpp"
#include "BgThemes4.hpp"
#include "BgProps.hpp"
#include "BgReal.hpp"

namespace vanta {

class Background;
inline Background* g_bg = nullptr;

class Background : public CCNode {
protected:
    CCLayerGradient* m_grad = nullptr;
    Ref<CCNode> m_hide;
    BgState m_s;
    float m_speed = 1.f;

    bool init() override {
        if (!CCNode::init()) return false;
        auto win = CCDirector::get()->getWinSize();
        m_s.W = win.width;
        m_s.H = win.height;

        m_grad = CCLayerGradient::create(ccc4(0, 0, 0, 255), ccc4(0, 0, 0, 255));
        m_grad->setContentSize(win);
        this->addChild(m_grad);

        m_s.draw = CCDrawNode::create();
        this->addChild(m_s.draw, 1);

        // second layer with additive blending: glows, embers, stars, aurora
        m_s.add = CCDrawNode::create();
        m_s.add->setBlendFunc(ccBlendFunc{GL_SRC_ALPHA, GL_ONE});
        this->addChild(m_s.add, 2);

        g_bg = this;
        g_bgRefresh = []{
            if (g_bg) g_bg->refresh();
        };

        this->scheduleUpdate();
        this->refresh();
        return true;
    }

    void setTheme(int t) {
        static ccColor3B const tops[] = {
            {0, 0, 0}, {12, 0, 0}, {8, 16, 40}, {120, 205, 255}, {30, 150, 255},
            {255, 190, 215}, {4, 8, 25}, {0, 0, 12}, {20, 25, 40}
        };
        static ccColor3B const bots[] = {
            {0, 0, 0}, {120, 30, 0}, {70, 110, 160}, {190, 245, 170}, {190, 235, 255},
            {255, 238, 243}, {8, 35, 45}, {25, 12, 55}, {65, 75, 95}
        };
        static int const base[] = {0, 70, 110, 40, 30, 50, 60, 120, 150};
        static float const dens[] = {0.5f, 1.f, 1.8f};

        if (t < 0 || t > 8) t = 0;
        m_s.theme = t;

        m_grad->setVisible(t != 0);
        m_s.draw->setVisible(t != 0);
        m_s.add->setVisible(t != 0);
        if (m_hide) m_hide->setVisible(t == 0);

        m_grad->setStartColor(tops[t]);
        m_grad->setEndColor(bots[t]);
        m_grad->setStartOpacity(255);
        m_grad->setEndOpacity(255);

        int di = getIndex("bg-density");
        float mult = dens[(di >= 0 && di < 3) ? di : 1];
        int count = (int)(base[t] * mult);

        m_s.p.assign(count, Particle{});
        for (auto& p : m_s.p) spawn(m_s, p, true);
        m_s.fx.clear();
        m_s.bolt.clear();
        m_s.bolt2.clear();
        m_s.flash = 0.f;
        m_s.draw->clear();
        m_s.add->clear();
    }

    void update(float dtRaw) override {
        if (m_s.theme == 0) return;
        float dt = dtRaw * m_speed;
        m_s.t += dt;
        m_s.draw->clear();
        m_s.add->clear();

        static ccColor4F const skyB[9] = {
            C(0.f, 0.f, 0.f, 1.f), C(0.55f, 0.12f, 0.02f, 1.f), C(0.55f, 0.68f, 0.85f, 1.f),
            C(0.88f, 0.97f, 0.85f, 1.f), C(0.95f, 0.97f, 1.f, 1.f), C(1.f, 0.95f, 0.93f, 1.f),
            C(0.04f, 0.16f, 0.18f, 1.f), C(0.2f, 0.1f, 0.32f, 1.f), C(0.38f, 0.42f, 0.5f, 1.f)
        };
        static ccColor4F const skyM[9] = {
            C(0.f, 0.f, 0.f, 1.f), C(0.18f, 0.03f, 0.02f, 1.f), C(0.2f, 0.3f, 0.5f, 1.f),
            C(0.62f, 0.85f, 0.97f, 1.f), C(0.45f, 0.78f, 0.98f, 1.f), C(1.f, 0.82f, 0.88f, 1.f),
            C(0.02f, 0.07f, 0.15f, 1.f), C(0.07f, 0.04f, 0.2f, 1.f), C(0.22f, 0.25f, 0.32f, 1.f)
        };
        static ccColor4F const skyT[9] = {
            C(0.f, 0.f, 0.f, 1.f), C(0.03f, 0.f, 0.f, 1.f), C(0.03f, 0.06f, 0.18f, 1.f),
            C(0.3f, 0.62f, 0.95f, 1.f), C(0.1f, 0.45f, 0.9f, 1.f), C(0.85f, 0.65f, 0.85f, 1.f),
            C(0.f, 0.02f, 0.07f, 1.f), C(0.f, 0.f, 0.06f, 1.f), C(0.08f, 0.09f, 0.13f, 1.f)
        };
        int th = std::max(0, std::min(8, m_s.theme));
        skyBands(m_s, skyB[th], skyM[th], skyT[th]);

        float t = m_s.t;
        float wind = S(t * 0.35f) * 12.f + std::max(0.f, S(t * 0.9f + 1.f)) * 18.f;

        drawBackdrop(m_s, wind);

        switch (m_s.theme) {
            case 1: themeFire(m_s, dt, wind); break;
            case 2: themeWinter(m_s, dt, wind); break;
            case 3: themeSpring(m_s, dt, wind); break;
            case 4: themeSummer(m_s, dt, wind); break;
            case 5: themeBlossom(m_s, dt, wind); break;
            case 6: themeAurora(m_s); break;
            case 7: themeStars(m_s, dt); break;
            case 8: themeRain(m_s, dt, wind); break;
            default: break;
        }

        drawProps(m_s, wind);
        drawExtras(m_s, wind);

        // cinematic finish: horizon haze, vignette and a slow camera drift
        float W = m_s.W;
        float H = m_s.H;
        switch (m_s.theme) {
            case 1: haze(m_s, H * 0.12f, H * 0.5f, C(1.f, 0.45f, 0.1f, 0.22f)); break;
            case 2: haze(m_s, H * 0.25f, H * 0.35f, C(0.6f, 0.75f, 1.f, 0.2f)); break;
            case 3: haze(m_s, H * 0.3f, H * 0.4f, C(1.f, 1.f, 0.9f, 0.2f)); break;
            case 4: haze(m_s, H * 0.34f, H * 0.25f, C(1.f, 1.f, 1.f, 0.25f)); break;
            case 5: haze(m_s, H * 0.25f, H * 0.4f, C(1.f, 0.85f, 0.9f, 0.22f)); break;
            case 6: haze(m_s, H * 0.15f, H * 0.3f, C(0.3f, 1.f, 0.7f, 0.15f)); break;
            case 8: haze(m_s, H * 0.1f, H * 0.5f, C(0.7f, 0.75f, 0.85f, 0.25f)); break;
            default: break;
        }
        vignette(m_s);
        CCPoint drift = ccp(-W * 0.025f + S(t * 0.17f) * 5.f, -H * 0.025f + S(t * 0.13f + 1.f) * 3.f);
        m_s.draw->setScale(1.05f);
        m_s.draw->setPosition(drift);
        m_s.add->setScale(1.05f);
        m_s.add->setPosition(drift);
    }

public:
    ~Background() override {
        if (g_bg == this) g_bg = nullptr;
    }

    static Background* create() {
        auto ret = new Background();
        if (ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    void setHideNode(CCNode* n) {
        m_hide = n;
        this->refresh();
    }

    void refresh() {
        static float const speeds[] = {0.5f, 1.f, 2.f};
        int si = getIndex("bg-speed");
        m_speed = speeds[(si >= 0 && si < 3) ? si : 1];
        setTheme(getIndex("bg-theme"));
    }
};

}
