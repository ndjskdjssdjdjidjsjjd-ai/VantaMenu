#pragma once
#include "Features.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <vector>

namespace vanta {

struct Particle {
    float x = 0.f, y = 0.f, vx = 0.f, vy = 0.f;
    float size = 2.f, phase = 0.f, age = 0.f, maxLife = 1.f;
    float z = 1.f, rot = 0.f, rotV = 0.f;
    int kind = 0;
};

class Background;
inline Background* g_bg = nullptr;

inline float R(float a, float b) {
    return a + (b - a) * (std::rand() / (float)RAND_MAX);
}

inline float S(float v) {
    return std::sin(v);
}

class Background : public CCNode {
protected:
    CCLayerGradient* m_grad = nullptr;
    CCDrawNode* m_draw = nullptr;
    Ref<CCNode> m_hide;
    std::vector<Particle> m_p;
    std::vector<Particle> m_fx;
    std::vector<CCPoint> m_bolt;
    CCSize m_win;
    int m_theme = 0;
    float m_t = 0.f;
    float m_speed = 1.f;
    float m_flash = 0.f;
    float m_nextFlash = 5.f;
    bool m_ssOn = false;
    float m_ssX = 0.f;
    float m_ssY = 0.f;
    float m_ssTimer = 3.f;

    bool init() override {
        if (!CCNode::init()) return false;
        m_win = CCDirector::get()->getWinSize();

        m_grad = CCLayerGradient::create(ccc4(0, 0, 0, 255), ccc4(0, 0, 0, 255));
        m_grad->setContentSize(m_win);
        this->addChild(m_grad);

        m_draw = CCDrawNode::create();
        this->addChild(m_draw, 1);

        g_bg = this;
        g_bgRefresh = []{
            if (g_bg) g_bg->refresh();
        };

        this->scheduleUpdate();
        this->refresh();
        return true;
    }

    // ---------- drawing helpers ----------
    void glow(CCPoint c, float r, ccColor4F col, int layers) {
        for (int i = 0; i < layers; i++) {
            float k = float(i) / float(layers);
            m_draw->drawDot(c, r * (1.f - k),
                ccColor4F{col.r, col.g, col.b, col.a * (0.25f + 0.75f * k) * 2.f / layers});
        }
    }

    void tri(CCPoint a, CCPoint b, CCPoint c, ccColor4F col) {
        CCPoint p[3] = {a, b, c};
        m_draw->drawPolygon(p, 3, col, 0.f, ccColor4F{0.f, 0.f, 0.f, 0.f});
    }

    void petal(float x, float y, float s, float rot, float flip, ccColor4F col) {
        float w = s * (0.35f + 0.65f * std::fabs(std::cos(flip)));
        float c = std::cos(rot);
        float sn = std::sin(rot);
        CCPoint v[4] = {
            ccp(x + (-s) * c, y + (-s) * sn),
            ccp(x + (-w) * (-sn), y + (-w) * c),
            ccp(x + s * c, y + s * sn),
            ccp(x + w * (-sn), y + w * c)
        };
        m_draw->drawPolygon(v, 4, col, 0.f, ccColor4F{0.f, 0.f, 0.f, 0.f});
    }

    void cloud(float x, float y, float s, float a) {
        ccColor4F c{1.f, 1.f, 1.f, a};
        m_draw->drawDot(ccp(x, y), s * 0.5f, c);
        m_draw->drawDot(ccp(x + s * 0.45f, y + s * 0.1f), s * 0.4f, c);
        m_draw->drawDot(ccp(x - s * 0.45f, y + s * 0.05f), s * 0.38f, c);
        m_draw->drawDot(ccp(x + s * 0.1f, y + s * 0.3f), s * 0.38f, c);
    }

    void drawHills(float base, float amp, float freq, float drift, ccColor4F col) {
        float W = m_win.width;
        for (float x = 0.f; x <= W + 8.f; x += 8.f) {
            float y = base + amp * S(x * freq + drift);
            m_draw->drawSegment(ccp(x, 0.f), ccp(x, y), 4.5f, col);
        }
    }

    void drawStars(float alphaMul) {
        for (auto& p : m_p) {
            float a = (0.35f + 0.65f * (0.5f + 0.5f * S(m_t * 2.5f + p.phase))) * alphaMul;
            m_draw->drawDot(ccp(p.x, p.y), p.size, ccColor4F{1.f, 1.f, 1.f, a});
        }
    }

    void makeBolt() {
        m_bolt.clear();
        float x = R(m_win.width * 0.2f, m_win.width * 0.8f);
        float y = m_win.height;
        m_bolt.push_back(ccp(x, y));
        while (y > 20.f) {
            y -= R(14.f, 30.f);
            x += R(-18.f, 18.f);
            m_bolt.push_back(ccp(x, y));
        }
    }

    // ---------- particles ----------
    void spawn(Particle& p, bool initial) {
        float W = m_win.width;
        float H = m_win.height;
        p = Particle{};
        p.phase = R(0.f, 6.28f);
        p.z = R(0.45f, 1.f);

        if (m_theme == 1) { // embers + smoke
            p.kind = (std::rand() % 100 < 18) ? 1 : 0;
            p.x = R(0.f, W);
            if (p.kind == 1) {
                p.y = initial ? R(0.f, H * 0.8f) : R(40.f, 90.f);
                p.vy = R(25.f, 60.f);
                p.vx = R(-6.f, 6.f);
                p.size = R(16.f, 32.f);
                p.maxLife = R(3.f, 5.f);
            } else {
                p.y = initial ? R(0.f, H * 0.7f) : R(0.f, 20.f);
                p.vy = R(50.f, 150.f);
                p.vx = R(-10.f, 10.f);
                p.size = R(1.2f, 3.8f);
                p.maxLife = R(1.4f, 3.4f);
            }
            p.age = initial ? R(0.f, p.maxLife) : 0.f;
        } else if (m_theme == 2) { // snow
            p.x = R(0.f, W);
            p.y = initial ? R(0.f, H) : H + 5.f;
            p.vx = R(-6.f, 6.f) * p.z;
            p.vy = -(15.f + 55.f * p.z);
            p.size = 1.f + 2.6f * p.z;
        } else if (m_theme == 3) { // pollen + petals
            p.x = R(0.f, W);
            p.y = initial ? R(0.f, H) : H + 5.f;
            p.vx = R(4.f, 14.f) * p.z;
            p.vy = -R(6.f, 22.f) * p.z;
            p.size = R(1.2f, 3.f);
            p.kind = std::rand() % 3;
        } else if (m_theme == 4) { // sun dust
            p.x = R(0.f, W);
            p.y = R(H * 0.3f, H);
            p.vx = R(-5.f, 5.f);
            p.vy = R(-3.f, 8.f);
            p.size = R(0.8f, 2.4f);
        } else if (m_theme == 5) { // cherry blossom petals
            p.x = R(0.f, W * 1.3f);
            p.y = initial ? R(0.f, H) : H + 10.f;
            p.vx = -R(8.f, 26.f) * p.z;
            p.vy = -R(22.f, 50.f) * p.z;
            p.size = 2.5f + 3.f * p.z;
            p.rot = R(0.f, 6.28f);
            p.rotV = R(-2.f, 2.f);
            p.kind = std::rand() % 2;
        } else if (m_theme == 6 || m_theme == 7) { // stars
            p.x = R(0.f, W);
            p.y = R(H * 0.25f, H);
            p.size = R(0.6f, 1.8f);
        } else if (m_theme == 8) { // rain
            p.x = R(0.f, W * 1.2f);
            p.y = initial ? R(0.f, H) : H + 10.f;
            p.vx = -(60.f + 40.f * p.z);
            p.vy = -(380.f + 260.f * p.z);
            p.size = 8.f + 8.f * p.z;
        }
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
        m_theme = t;

        m_grad->setVisible(t != 0);
        m_draw->setVisible(t != 0);
        if (m_hide) m_hide->setVisible(t == 0);

        m_grad->setStartColor(tops[t]);
        m_grad->setEndColor(bots[t]);
        m_grad->setStartOpacity(255);
        m_grad->setEndOpacity(255);

        int di = getIndex("bg-density");
        float mult = dens[(di >= 0 && di < 3) ? di : 1];
        int count = (int)(base[t] * mult);

        m_p.assign(count, Particle{});
        for (auto& p : m_p) spawn(p, true);
        m_fx.clear();
        m_bolt.clear();
        m_flash = 0.f;
        m_draw->clear();
    }

    // ================= THEMES =================

    void themeFire(float dt, float W, float H, float wind) {
        for (int i = 0; i < 9; i++) {
            float gx = W * i / 8.f;
            float r = 110.f + S(m_t * 2.2f + i * 1.7f) * 22.f;
            glow(ccp(gx, -30.f), r, ccColor4F{1.f, 0.4f, 0.05f, 0.22f}, 5);
        }
        for (float x = -6.f; x <= W + 6.f; x += 9.f) {
            float n = S(x * 0.045f + m_t * 4.2f) * 0.5f
                    + S(x * 0.13f - m_t * 6.1f) * 0.3f
                    + S(x * 0.31f + m_t * 9.f) * 0.2f;
            float h = 70.f + 55.f * n + 25.f * S(x * 0.01f + m_t * 0.7f);
            float sway = S(x * 0.05f + m_t * 3.f) * 8.f;
            tri(ccp(x - 10.f, 0.f), ccp(x + 10.f, 0.f), ccp(x + sway, h),
                ccColor4F{0.85f, 0.12f, 0.02f, 0.55f});
            tri(ccp(x - 8.f, 0.f), ccp(x + 8.f, 0.f), ccp(x + sway * 0.8f, h * 0.72f),
                ccColor4F{1.f, 0.45f, 0.05f, 0.65f});
            tri(ccp(x - 5.f, 0.f), ccp(x + 5.f, 0.f), ccp(x + sway * 0.5f, h * 0.42f),
                ccColor4F{1.f, 0.85f, 0.3f, 0.8f});
        }
        for (auto& p : m_p) {
            p.age += dt;
            if (p.age >= p.maxLife) {
                spawn(p, false);
                continue;
            }
            float t = p.age / p.maxLife;
            if (p.kind == 1) {
                p.x += (p.vx + wind * 0.5f + S(m_t + p.phase) * 10.f) * dt;
                p.y += p.vy * dt;
                m_draw->drawDot(ccp(p.x, p.y), p.size * (0.6f + t),
                    ccColor4F{0.12f, 0.1f, 0.1f, 0.16f * (1.f - t)});
            } else {
                p.x += (p.vx + S(m_t * 3.f + p.phase) * 22.f + wind) * dt;
                p.y += p.vy * dt;
                float fl = 0.7f + 0.3f * S(m_t * 20.f + p.phase * 5.f);
                glow(ccp(p.x, p.y), p.size * 2.6f * (1.f - 0.6f * t),
                    ccColor4F{1.f, 0.5f, 0.1f, 0.35f * (1.f - t)}, 2);
                m_draw->drawDot(ccp(p.x, p.y), p.size * (1.f - 0.6f * t),
                    ccColor4F{1.f, 0.95f - 0.6f * t, 0.35f * (1.f - t), fl * (1.f - t)});
            }
        }
    }

    void themeWinter(float dt, float W, float H, float wind) {
        for (int i = 0; i < 45; i++) {
            float sx = std::fmod(i * 97.3f, W);
            float sy = H * 0.45f + std::fmod(i * 53.7f, H * 0.55f);
            float a = 0.3f + 0.5f * (0.5f + 0.5f * S(m_t * 1.8f + i));
            m_draw->drawDot(ccp(sx, sy), 0.9f, ccColor4F{1.f, 1.f, 1.f, a});
        }
        glow(ccp(W * 0.8f, H * 0.78f), 75.f, ccColor4F{0.8f, 0.9f, 1.f, 0.2f}, 6);
        m_draw->drawDot(ccp(W * 0.8f, H * 0.78f), 24.f, ccColor4F{0.95f, 0.97f, 1.f, 1.f});

        for (int i = 0; i < 9; i++) {
            float tx = W * (i + 0.5f) / 9.f + S(i * 3.1f) * 14.f;
            float th = 55.f + 28.f * (0.5f + 0.5f * S(i * 2.3f));
            float sw = S(m_t * 0.8f + i) * 1.5f;
            m_draw->drawSegment(ccp(tx, 4.f), ccp(tx, 18.f), 2.5f, ccColor4F{0.25f, 0.15f, 0.1f, 1.f});
            for (int k = 0; k < 3; k++) {
                float by = 14.f + th * 0.28f * k;
                float hw = th * (0.36f - 0.07f * k);
                float ht = th * 0.45f;
                tri(ccp(tx - hw, by), ccp(tx + hw, by), ccp(tx + sw, by + ht),
                    ccColor4F{0.07f, 0.16f, 0.2f, 1.f});
                tri(ccp(tx - hw * 0.55f, by + ht * 0.55f), ccp(tx + hw * 0.55f, by + ht * 0.55f),
                    ccp(tx + sw, by + ht), ccColor4F{0.9f, 0.95f, 1.f, 0.95f});
            }
        }
        m_draw->drawSegment(ccp(0.f, 4.f), ccp(W, 4.f), 11.f, ccColor4F{0.92f, 0.96f, 1.f, 1.f});

        for (auto& p : m_p) {
            p.y += p.vy * dt;
            p.x += (p.vx + wind * p.z + S(m_t * 1.5f + p.phase) * 12.f * p.z) * dt;
            if (p.y < -5.f) {
                spawn(p, false);
                continue;
            }
            if (p.x < -5.f) p.x += W + 10.f;
            if (p.x > W + 5.f) p.x -= W + 10.f;
            if (p.z > 0.8f) {
                glow(ccp(p.x, p.y), p.size * 2.2f, ccColor4F{1.f, 1.f, 1.f, 0.18f}, 2);
            }
            m_draw->drawDot(ccp(p.x, p.y), p.size, ccColor4F{1.f, 1.f, 1.f, 0.55f + 0.4f * p.z});
        }
    }

    void themeSpring(float dt, float W, float H, float wind) {
        glow(ccp(W * 0.15f, H * 0.85f), 90.f, ccColor4F{1.f, 0.98f, 0.75f, 0.25f}, 5);
        m_draw->drawDot(ccp(W * 0.15f, H * 0.85f), 22.f, ccColor4F{1.f, 0.99f, 0.85f, 1.f});

        for (int i = 0; i < 4; i++) {
            float cx = std::fmod(i * 180.f + m_t * (6.f + i * 2.f), W + 240.f) - 120.f;
            cloud(cx, H * (0.7f + 0.08f * S(i * 2.f)), 46.f + 10.f * i, 0.85f);
        }

        drawHills(62.f, 18.f, 0.008f, m_t * 0.1f, ccColor4F{0.55f, 0.78f, 0.5f, 1.f});
        drawHills(40.f, 14.f, 0.012f, 1.f + m_t * 0.15f, ccColor4F{0.42f, 0.72f, 0.34f, 1.f});

        for (int i = 0; i < 15; i++) {
            float fx = W * (i + 0.5f) / 15.f;
            float fy = 40.f + 14.f * S(fx * 0.012f + 1.f + m_t * 0.15f);
            float sw = S(m_t * 1.4f + i) * 2.f;
            m_draw->drawSegment(ccp(fx, fy - 8.f), ccp(fx + sw, fy + 4.f), 0.9f, ccColor4F{0.2f, 0.5f, 0.2f, 1.f});
            ccColor4F fc = ccColor4F{0.7f, 0.5f, 1.f, 1.f};
            if (i % 4 == 0) fc = ccColor4F{1.f, 0.5f, 0.6f, 1.f};
            if (i % 4 == 1) fc = ccColor4F{1.f, 1.f, 1.f, 1.f};
            if (i % 4 == 2) fc = ccColor4F{1.f, 0.9f, 0.3f, 1.f};
            m_draw->drawDot(ccp(fx + sw, fy + 5.f), 3.2f, fc);
            m_draw->drawDot(ccp(fx + sw, fy + 5.f), 1.2f, ccColor4F{1.f, 0.9f, 0.3f, 1.f});
        }

        for (float x = 0.f; x <= W; x += 7.f) {
            float gh = 14.f + 8.f * S(x * 0.2f) + 5.f * S(x * 0.07f);
            float sw = S(m_t * 1.6f + x * 0.05f) * 4.f + wind * 0.1f;
            m_draw->drawSegment(ccp(x, 0.f), ccp(x + sw, gh), 1.1f, ccColor4F{0.25f, 0.6f, 0.22f, 1.f});
        }

        for (int b = 0; b < 3; b++) {
            float bx = W * (0.3f + 0.25f * b) + S(m_t * 0.6f + b * 2.f) * 90.f;
            float by = H * 0.35f + S(m_t * 0.9f + b * 1.3f) * 30.f + b * 20.f;
            float flap = std::fabs(S(m_t * 14.f + b)) * 5.f + 1.f;
            ccColor4F wc = (b % 2 == 0) ? ccColor4F{1.f, 0.6f, 0.2f, 0.95f} : ccColor4F{0.5f, 0.7f, 1.f, 0.95f};
            m_draw->drawDot(ccp(bx - flap, by), 3.f, wc);
            m_draw->drawDot(ccp(bx + flap, by), 3.f, wc);
            m_draw->drawDot(ccp(bx, by), 1.2f, ccColor4F{0.15f, 0.1f, 0.1f, 1.f});
        }

        for (auto& p : m_p) {
            p.y += p.vy * dt;
            p.x += (p.vx + wind * 0.5f + S(m_t * 1.2f + p.phase) * 12.f) * dt;
            if (p.y < -5.f || p.x > W + 5.f) {
                spawn(p, false);
                continue;
            }
            ccColor4F c = ccColor4F{1.f, 0.92f, 0.4f, 0.9f};
            if (p.kind == 0) c = ccColor4F{1.f, 0.7f, 0.82f, 0.95f};
            if (p.kind == 1) c = ccColor4F{1.f, 1.f, 1.f, 0.9f};
            m_draw->drawDot(ccp(p.x, p.y), p.size, c);
        }
    }

    void themeSummer(float dt, float W, float H) {
        float sx = W * 0.85f;
        float sy = H * 0.8f;
        glow(ccp(sx, sy), 110.f + S(m_t * 1.5f) * 6.f, ccColor4F{1.f, 0.95f, 0.5f, 0.22f}, 6);
        for (int i = 0; i < 14; i++) {
            float a = m_t * 0.3f + i * 0.4488f;
            float r2 = 78.f + S(m_t * 2.f + i) * 9.f;
            m_draw->drawSegment(
                ccp(sx + std::cos(a) * 42.f, sy + std::sin(a) * 42.f),
                ccp(sx + std::cos(a) * r2, sy + std::sin(a) * r2),
                1.4f, ccColor4F{1.f, 0.95f, 0.5f, 0.35f});
        }
        m_draw->drawDot(ccp(sx, sy), 34.f, ccColor4F{1.f, 0.9f, 0.3f, 1.f});
        m_draw->drawDot(ccp(sx, sy), 26.f, ccColor4F{1.f, 0.97f, 0.7f, 1.f});

        for (int i = 0; i < 3; i++) {
            float cx = std::fmod(i * 260.f + m_t * (8.f + i * 3.f), W + 260.f) - 130.f;
            cloud(cx, H * (0.62f + 0.1f * S(i * 1.7f)), 52.f + 12.f * i, 0.9f);
        }

        for (int b = 0; b < 3; b++) {
            float bx = std::fmod(m_t * 40.f + b * 260.f, W + 80.f) - 40.f;
            float by = H * 0.52f + b * 18.f + S(m_t + b) * 6.f;
            float fl = S(m_t * 8.f + b) * 5.f;
            ccColor4F bc{0.1f, 0.15f, 0.25f, 0.9f};
            m_draw->drawSegment(ccp(bx - 9.f, by + fl), ccp(bx, by), 0.9f, bc);
            m_draw->drawSegment(ccp(bx, by), ccp(bx + 9.f, by + fl), 0.9f, bc);
        }

        for (auto& p : m_p) {
            p.x += (p.vx + S(m_t + p.phase) * 6.f) * dt;
            p.y += (p.vy + std::cos(m_t * 0.8f + p.phase) * 5.f) * dt;
            if (p.x < 0.f) p.x += W;
            if (p.x > W) p.x -= W;
            if (p.y > H) p.y = H * 0.3f;
            if (p.y < H * 0.3f) p.y = H;
            float a = 0.3f + 0.5f * (0.5f + 0.5f * S(m_t * 3.f + p.phase));
            m_draw->drawDot(ccp(p.x, p.y), p.size, ccColor4F{1.f, 1.f, 0.85f, a});
        }

        drawHills(34.f, 6.f, 0.02f, m_t * 1.2f, ccColor4F{0.3f, 0.7f, 0.98f, 1.f});
        drawHills(24.f, 6.f, 0.025f, 2.f + m_t * 1.6f, ccColor4F{0.18f, 0.55f, 0.9f, 1.f});
        drawHills(12.f, 5.f, 0.03f, 4.f + m_t * 2.f, ccColor4F{0.08f, 0.4f, 0.8f, 1.f});

        for (float x = 0.f; x <= W; x += 14.f) {
            float y = 34.f + 6.f * S(x * 0.02f + m_t * 1.2f);
            float a = 0.35f + 0.4f * (0.5f + 0.5f * S(m_t * 3.f + x));
            m_draw->drawDot(ccp(x, y + 5.f), 1.6f, ccColor4F{1.f, 1.f, 1.f, a});
        }
        for (int i = 0; i < 25; i++) {
            float qx = std::fmod(i * 61.7f, W);
            float qy = 4.f + std::fmod(i * 11.3f, 24.f);
            float a = std::max(0.f, S(m_t * 4.f + i * 2.f));
            m_draw->drawDot(ccp(qx, qy), 1.2f, ccColor4F{1.f, 1.f, 0.9f, a * 0.8f});
        }
    }

    void themeBlossom(float dt, float W, float H, float wind) {
        for (int i = 0; i < 5; i++) {
            float x0 = W * (0.1f + 0.2f * i) + S(m_t * 0.2f + i) * 15.f;
            m_draw->drawSegment(ccp(x0, H), ccp(x0 - 90.f, 0.f), 14.f, ccColor4F{1.f, 0.95f, 0.85f, 0.05f});
        }

        ccColor4F wood{0.33f, 0.2f, 0.18f, 1.f};
        m_draw->drawSegment(ccp(W + 10.f, H - 30.f), ccp(W * 0.78f, H - 55.f), 4.f, wood);
        m_draw->drawSegment(ccp(W * 0.78f, H - 55.f), ccp(W * 0.62f, H - 50.f), 3.f, wood);
        m_draw->drawSegment(ccp(W * 0.78f, H - 55.f), ccp(W * 0.7f, H - 90.f), 2.5f, wood);
        m_draw->drawSegment(ccp(W + 10.f, H - 70.f), ccp(W * 0.88f, H - 85.f), 3.f, wood);
        m_draw->drawSegment(ccp(W * 0.62f, H - 50.f), ccp(W * 0.54f, H - 66.f), 2.f, wood);

        CCPoint clusters[6] = {
            ccp(W * 0.78f, H - 55.f), ccp(W * 0.62f, H - 50.f), ccp(W * 0.7f, H - 90.f),
            ccp(W * 0.88f, H - 85.f), ccp(W * 0.95f, H - 45.f), ccp(W * 0.54f, H - 66.f)
        };
        for (int k = 0; k < 6; k++) {
            float sway = S(m_t + k) * 1.5f;
            glow(ccp(clusters[k].x, clusters[k].y + sway), 22.f, ccColor4F{1.f, 0.75f, 0.85f, 0.2f}, 3);
            for (int j = 0; j < 7; j++) {
                float a = j * 0.9f + k;
                m_draw->drawDot(
                    ccp(clusters[k].x + std::cos(a) * 10.f, clusters[k].y + std::sin(a) * 10.f + sway),
                    5.f, ccColor4F{1.f, 0.72f, 0.82f, 0.95f});
            }
            m_draw->drawDot(ccp(clusters[k].x, clusters[k].y + sway), 3.f, ccColor4F{1.f, 0.9f, 0.94f, 1.f});
        }

        for (auto& p : m_p) {
            p.y += p.vy * dt;
            p.x += (p.vx + wind * p.z + S(m_t * 2.f + p.phase) * 20.f * p.z) * dt;
            p.rot += p.rotV * dt;
            if (p.y < -10.f || p.x < -12.f) {
                spawn(p, false);
                continue;
            }
            ccColor4F c = ccColor4F{1.f, 0.86f, 0.92f, 0.55f + 0.4f * p.z};
            if (p.kind == 0) c = ccColor4F{1.f, 0.72f, 0.82f, 0.55f + 0.4f * p.z};
            petal(p.x, p.y, p.size, p.rot, m_t * 3.f + p.phase, c);
        }
    }

    void themeAurora(float W, float H) {
        ccColor4F cols[3] = {
            ccColor4F{0.2f, 1.f, 0.6f, 0.12f},
            ccColor4F{0.2f, 0.7f, 1.f, 0.12f},
            ccColor4F{0.7f, 0.4f, 1.f, 0.12f}
        };
        for (int i = 0; i < 3; i++) {
            for (float x = 0.f; x <= W + 8.f; x += 8.f) {
                float yc = H * (0.62f + 0.04f * i)
                    + S(x * 0.012f + m_t * 0.5f + i * 2.f) * 28.f
                    + S(x * 0.03f - m_t * 0.8f) * 8.f;
                float h = 38.f + S(x * 0.02f + m_t + i)
