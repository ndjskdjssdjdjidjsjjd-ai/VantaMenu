#pragma once
#include "Features.hpp"
#include <cmath>
#include <cstdlib>
#include <vector>

namespace vanta {

struct Particle {
    float x = 0.f, y = 0.f, vx = 0.f, vy = 0.f;
    float size = 2.f, phase = 0.f, age = 0.f, maxLife = 1.f;
    int kind = 0;
};

class Background;
inline Background* g_bg = nullptr;

inline float R(float a, float b) {
    return a + (b - a) * (std::rand() / (float)RAND_MAX);
}

class Background : public CCNode {
protected:
    CCLayerGradient* m_grad = nullptr;
    CCDrawNode* m_draw = nullptr;
    Ref<CCNode> m_hide;
    std::vector<Particle> m_p;
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
        g_bgRefresh = []{ if (g_bg) g_bg->refresh(); };

        this->scheduleUpdate();
        this->refresh();
        return true;
    }

    void spawn(Particle& p, bool initial) {
        float W = m_win.width, H = m_win.height;
        p = Particle{};
        p.phase = R(0.f, 6.28f);
        switch (m_theme) {
            case 1: // fire embers
                p.x = R(0.f, W);
                p.y = initial ? R(0.f, H * 0.7f) : R(-10.f, 0.f);
                p.vx = R(-8.f, 8.f);
                p.vy = R(40.f, 120.f);
                p.size = R(1.5f, 4.5f);
                p.maxLife = R(1.4f, 3.2f);
                p.age = initial ? R(0.f, p.maxLife) : 0.f;
                break;
            case 2: // snow
                p.x = R(0.f, W);
                p.y = initial ? R(0.f, H) : H + 5.f;
                p.vx = R(-10.f, 10.f);
                p.vy = -R(20.f, 60.f);
                p.size = R(1.5f, 4.f);
                break;
            case 3: // spring petals + pollen
                p.x = R(0.f, W);
                p.y = initial ? R(0.f, H) : H + 5.f;
                p.vx = R(5.f, 18.f);
                p.vy = -R(8.f, 28.f);
                p.size = R(1.5f, 3.5f);
                p.kind = std::rand() % 3;
                break;
            case 4: // summer sparkles
                p.x = R(0.f, W);
                p.y = R(H * 0.2f, H);
                p.vx = R(-6.f, 6.f);
                p.vy = R(-4.f, 10.f);
                p.size = R(1.f, 3.f);
                break;
            case 5: // cherry blossom petals
                p.x = R(0.f, W * 1.3f);
                p.y = initial ? R(0.f, H) : H + 8.f;
                p.vx = -R(10.f, 30.f);
                p.vy = -R(25.f, 55.f);
                p.size = R(2.5f, 5.f);
                break;
            case 6: // aurora stars
            case 7: // stars
                p.x = R(0.f, W);
                p.y = R(H * 0.25f, H);
                p.size = R(0.6f, 1.8f);
                break;
            case 8: // rain
                p.x = R(0.f, W * 1.2f);
                p.y = initial ? R(0.f, H) : H + 10.f;
                p.vx = -R(60.f, 100.f);
                p.vy = -R(380.f, 560.f);
                p.size = R(8.f, 16.f);
                break;
            default:
                break;
        }
    }

    void setTheme(int t) {
        static ccColor3B const tops[] = {
            {0, 0, 0}, {15, 0, 0}, {10, 25, 60}, {120, 205, 255}, {30, 150, 255},
            {255, 190, 215}, {4, 8, 25}, {0, 0, 12}, {20, 25, 40}
        };
        static ccColor3B const bots[] = {
            {0, 0, 0}, {110, 25, 0}, {110, 160, 215}, {190, 245, 170}, {190, 235, 255},
            {255, 238, 243}, {8, 35, 45}, {25, 12, 55}, {65, 75, 95}
        };
        static int const base[] = {0, 90, 110, 50, 40, 60, 40, 120, 140};
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
        m_draw->clear();
    }

    void drawHills(float base, float amp, float freq, float drift, ccColor4F col) {
        float W = m_win.width;
        for (float x = 0.f; x <= W + 8.f; x += 8.f) {
            float y = base + amp * std::sin(x * freq + drift);
            m_draw->drawSegment(ccp(x, 0.f), ccp(x, y), 4.5f, col);
        }
    }

    void drawStars() {
        for (auto& p : m_p) {
            float a = 0.35f + 0.65f * (0.5f + 0.5f * std::sin(m_t * 2.5f + p.phase));
            m_draw->drawDot(ccp(p.x, p.y), p.size, ccColor4F{1.f, 1.f, 1.f, a});
        }
    }

    void update(float dtRaw) override {
        if (m_theme == 0) return;
        float dt = dtRaw * m_speed;
        m_t += dt;
        m_draw->clear();

        float W = m_win.width, H = m_win.height;

        switch (m_theme) {
            case 1: { // FIRE
                for (int i = 0; i < 7; i++) {
                    float gx = W * i / 6.f;
                    float r = 90.f + std::sin(m_t * 2.f + i * 1.7f) * 18.f;
                    m_draw->drawDot(ccp(gx, -20.f), r, ccColor4F{1.f, 0.35f, 0.05f, 0.12f});
                }
                for (auto& p : m_p) {
                    p.age += dt;
                    if (p.age >= p.maxLife) { spawn(p, false); continue; }
                    p.x += (p.vx + std::sin(m_t * 3.f + p.phase) * 18.f) * dt;
                    p.y += p.vy * dt;
                    float t = p.age / p.maxLife;
                    m_draw->drawDot(ccp(p.x, p.y), p.size * (1.f - 0.7f * t),
                        ccColor4F{1.f, 0.85f - 0.7f * t, 0.1f * (1.f - t), 0.9f * (1.f - t)});
                }
                break;
            }
            case 2: { // WINTER
                m_draw->drawDot(ccp(W * 0.82f, H * 0.8f), 50.f, ccColor4F{0.8f, 0.9f, 1.f, 0.10f});
                m_draw->drawDot(ccp(W * 0.82f, H * 0.8f), 26.f, ccColor4F{0.93f, 0.97f, 1.f, 0.95f});
                m_draw->drawSegment(ccp(0.f, 6.f), ccp(W, 6.f), 8.f, ccColor4F{0.9f, 0.95f, 1.f, 1.f});
                for (auto& p : m_p) {
                    p.y += p.vy * dt;
                    p.x += (p.vx + std::sin(m_t * 1.5f + p.phase) * 14.f) * dt;
                    if (p.y < -5.f) { spawn(p, false); continue; }
                    if (p.x < -5.f) p.x += W + 10.f;
                    if (p.x > W + 5.f) p.x -= W + 10.f;
                    m_draw->drawDot(ccp(p.x, p.y), p.size, ccColor4F{1.f, 1.f, 1.f, 0.85f});
                }
                break;
            }
            case 3: { // SPRING
                m_draw->drawDot(ccp(W * 0.15f, H * 0.85f), 60.f, ccColor4F{1.f, 1.f, 0.7f, 0.15f});
                m_draw->drawDot(ccp(W * 0.15f, H * 0.85f), 28.f, ccColor4F{1.f, 0.98f, 0.7f, 0.95f});
                drawHills(40.f, 14.f, 0.012f, m_t * 0.2f, ccColor4F{0.45f, 0.78f, 0.35f, 1.f});
                drawHills(22.f, 10.f, 0.02f, 1.5f + m_t * 0.3f, ccColor4F{0.3f, 0.65f, 0.28f, 1.f});
                for (auto& p : m_p) {
                    p.y += p.vy * dt;
                    p.x += (p.vx + std::sin(m_t * 1.2f + p.phase) * 12.f) * dt;
                    if (p.y < -5.f || p.x > W + 5.f) { spawn(p, false); continue; }
                    ccColor4F c = p.kind == 0 ? ccColor4F{1.f, 0.7f, 0.82f, 0.95f}
                                : p.kind == 1 ? ccColor4F{1.f, 1.f, 1.f, 0.9f}
                                              : ccColor4F{1.f, 0.92f, 0.4f, 0.9f};
                    m_draw->drawDot(ccp(p.x, p.y), p.size, c);
                }
                break;
            }
            case 4: { // SUMMER
                float sx = W * 0.85f, sy = H * 0.8f;
                m_draw->drawDot(ccp(sx, sy), 70.f + std::sin(m_t * 1.5f) * 4.f, ccColor4F{1.f, 0.95f, 0.5f, 0.10f});
                m_draw->drawDot(ccp(sx, sy), 50.f, ccColor4F{1.f, 0.92f, 0.4f, 0.20f});
                for (int i = 0; i < 12; i++) {
                    float a = m_t * 0.3f + i * 0.5236f;
                    float r2 = 70.f + std::sin(m_t * 2.f + i) * 8.f;
                    m_draw->drawSegment(
                        ccp(sx + std::cos(a) * 42.f, sy + std::sin(a) * 42.f),
                        ccp(sx + std::cos(a) * r2, sy + std::sin(a) * r2),
                        1.5f, ccColor4F{1.f, 0.95f, 0.5f, 0.35f});
                }
                m_draw->drawDot(ccp(sx, sy), 34.f, ccColor4F{1.f, 0.88f, 0.25f, 1.f});

                for (auto& p : m_p) {
                    p.x += (p.vx + std::sin(m_t + p.phase) * 6.f) * dt;
                    p.y += (p.vy + std::cos(m_t * 0.8f + p.phase) * 5.f) * dt;
                    if (p.x < 0.f) p.x += W;
                    if (p.x > W) p.x -= W;
                    if (p.y > H) p.y = H * 0.2f;
                    if (p.y < H * 0.2f) p.y = H;
                    float a = 0.4f + 0.6f * (0.5f + 0.5f * std::sin(m_t * 3.f + p.phase));
                    m_draw->drawDot(ccp(p.x, p.y), p.size, ccColor4F{1.f, 1.f, 0.8f, a});
                }

                drawHills(28.f, 6.f, 0.02f, m_t * 1.2f, ccColor4F{0.25f, 0.65f, 0.95f, 1.f});
                drawHills(18.f, 6.f, 0.025f, 2.f + m_t * 1.6f, ccColor4F{0.15f, 0.5f, 0.88f, 1.f});
                drawHills(8.f, 5.f, 0.03f, 4.f + m_t * 2.f, ccColor4F{0.08f, 0.38f, 0.78f, 1.f});
                break;
            }
            case 5: { // CHERRY BLOSSOM
                ccColor4F wood{0.35f, 0.2f, 0.18f, 1.f};
                m_draw->drawSegment(ccp(W + 10.f, H - 30.f), ccp(W * 0.78f, H - 55.f), 4.f, wood);
                m_draw->drawSegment(ccp(W * 0.78f, H - 55.f), ccp(W * 0.62f, H - 50.f), 3.f, wood);
                m_draw->drawSegment(ccp(W * 0.78f, H - 55.f), ccp(W * 0.7f, H - 90.f), 2.5f, wood);
                m_draw->drawSegment(ccp(W + 10.f, H - 70.f), ccp(W * 0.88f, H - 85.f), 3.f, wood);

                CCPoint clusters[] = {
                    ccp(W * 0.78f, H - 55.f), ccp(W * 0.62f, H - 50.f), ccp(W * 0.7f, H - 90.f),
                    ccp(W * 0.88f, H - 85.f), ccp(W * 0.95f, H - 45.f), ccp(W * 0.55f, H - 62.f)
                };
                for (int k = 0; k < 6; k++) {
                    float sway = std::sin(m_t + k) * 1.5f;
                    for (int j = 0; j < 6; j++) {
                        float a = j * 1.05f + k;
                        m_draw->drawDot(
                            ccp(clusters[k].x + std::cos(a) * 9.f, clusters[k].y + std::sin(a) * 9.f + sway),
                            5.f, ccColor4F{1.f, 0.72f, 0.82f, 0.95f});
                    }
                }

                for (auto& p : m_p) {
                    p.y += p.vy * dt;
                    p.x += (p.vx + std::sin(m_t * 2.f + p.phase) * 20.f) * dt;
                    if (p.y < -8.f || p.x < -10.f) { spawn(p, false); continue; }
                    m_draw->drawDot(ccp(p.x, p.y), p.size, ccColor4F{1.f, 0.72f, 0.82f, 0.95f});
                    float ox = std::cos(p.phase + m_t * 2.f) * p.size * 0.5f;
                    float oy = std::sin(p.phase + m_t * 2.f) * p.size * 0.5f;
                    m_draw->drawDot(ccp(p.x + ox, p.y + oy), p.size * 0.6f, ccColor4F{1.f, 0.88f, 0.93f, 0.95f});
                }
                break;
            }
            case 6: { // AURORA
                ccColor4F cols[3] = {
                    {0.2f, 1.f, 0.6f, 0.12f}, {0.2f, 0.7f, 1.f, 0.12f}, {0.7f, 0.4f, 1.f, 0.12f}
                };
                for (int i = 0; i < 3; i++) {
                    for (float x = 0.f; x <= W + 8.f; x += 8.f) {
                        float yc = H * (0.62f + 0.04f * i)
                            + std::sin(x * 0.012f + m_t * 0.5f + i * 2.f) * 28.f
                            + std::sin(x * 0.03f - m_t * 0.8f) * 8.f;
                        float h = 38.f + std::sin(x * 0.02f + m_t + i) * 12.f;
                        m_draw->drawSegment(ccp(x, yc - h), ccp(x, yc + h), 4.5f, cols[i]);
                    }
                }
                drawStars();
                break;
            }
            case 7: { // STARS
                m_draw->drawDot(ccp(W * 0.25f, H * 0.55f), 110.f, ccColor4F{0.5f, 0.2f, 0.9f, 0.06f});
                m_draw->drawDot(ccp(W * 0.75f, H * 0.4f), 130.f, ccColor4F{0.2f, 0.4f, 0.9f, 0.05f});
                drawStars();

                m_ssTimer -= dt;
                if (!m_ssOn && m_ssTimer <= 0.f) {
                    m_ssOn = true;
                    m_ssX = R(W * 0.3f, W);
                    m_ssY = R(H * 0.6f, H);
                    m_ssTimer = R(3.f, 7.f);
                }
                if (m_ssOn) {
                    m_ssX -= 420.f * dt;
                    m_ssY -= 180.f * dt;
                    m_draw->drawSegment(ccp(m_ssX, m_ssY), ccp(m_ssX + 60.f, m_ssY + 25.f), 1.2f,
                        ccColor4F{1.f, 1.f, 1.f, 0.6f});
                    m_draw->drawDot(ccp(m_ssX, m_ssY), 2.f, ccColor4F{1.f, 1.f, 1.f, 1.f});
                    if (m_ssX < -80.f || m_ssY < -40.f) m_ssOn = false;
                }
                break;
            }
            case 8: { // RAIN
                m_nextFlash -= dt;
                if (m_nextFlash <= 0.f) {
                    m_flash = 1.f;
                    m_nextFlash = R(4.f, 9.f);
                }
                if (m_flash > 0.f) {
                    CCPoint q[4] = {ccp(0.f, 0.f), ccp(W, 0.f), ccp(W, H), ccp(0.f, H)};
                    m_draw->drawPolygon(q, 4, ccColor4F{0.85f, 0.9f, 1.f, m_flash * 0.45f},
                        0.f, ccColor4F{0.f, 0.f, 0.f, 0.f});
                    m_flash -= dt * 2.5f;
                }
                for (auto& p : m_p) {
                    p.x += p.vx * dt;
                    p.y += p.vy * dt;
                    if (p.y < 0.f) { spawn(p, false); continue; }
                    m_draw->drawSegment(ccp(p.x, p.y),
                        ccp(p.x - p.vx * 0.025f, p.y - p.vy * 0.025f),
                        0.7f, ccColor4F{0.75f, 0.85f, 1.f, 0.55f});
                }
                break;
            }
            default:
                break;
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
