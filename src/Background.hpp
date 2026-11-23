#pragma once
#include "BgCore.hpp"
#include "BgThemes1.hpp"
#include "BgThemes2.hpp"
#include "BgThemes3.hpp"
#include "BgThemes4.hpp"

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
    }

    void update(float dtRaw) override {
        if (m_s.theme == 0) return;
        float dt = dtRaw * m_speed;
        m_s.t += dt;
        m_s.draw->clear();

        float t = m_s.t;
        float wind = S(t * 0.35f) * 12.f + std::max(0.f, S(t * 0.9f + 1.f)) * 18.f;

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
