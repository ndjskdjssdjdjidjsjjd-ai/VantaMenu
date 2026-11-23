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

inline float R(float a, float b) {
    return a + (b - a) * (std::rand() / (float)RAND_MAX);
}

inline float S(float v) {
    return std::sin(v);
}

inline float fbm(float x, float t) {
    return S(x + t) * 0.5f + S(x * 2.3f - t * 1.3f) * 0.3f + S(x * 5.1f + t * 2.1f) * 0.2f;
}

inline ccColor4F C(float r, float g, float b, float a) {
    return ccColor4F{r, g, b, a};
}

struct BgState {
    CCDrawNode* draw = nullptr;
    std::vector<Particle> p;
    std::vector<Particle> fx;
    std::vector<CCPoint> bolt;
    std::vector<CCPoint> bolt2;
    int theme = 0;
    float W = 0.f, H = 0.f, t = 0.f;
    float flash = 0.f, nextFlash = 5.f;
    bool ssOn = false;
    float ssX = 0.f, ssY = 0.f, ssTimer = 3.f;
};

inline void dot(BgState& s, float x, float y, float r, ccColor4F c) {
    s.draw->drawDot(ccp(x, y), r, c);
}

inline void seg(BgState& s, float x1, float y1, float x2, float y2, float w, ccColor4F c) {
    s.draw->drawSegment(ccp(x1, y1), ccp(x2, y2), w, c);
}

inline void rect(BgState& s, float x1, float y1, float x2, float y2, ccColor4F c) {
    CCPoint q[4] = {ccp(x1, y1), ccp(x2, y1), ccp(x2, y2), ccp(x1, y2)};
    s.draw->drawPolygon(q, 4, c, 0.f, C(0.f, 0.f, 0.f, 0.f));
}

inline void ellipse(BgState& s, float x, float y, float rx, float ry, float w, ccColor4F c) {
    float px = x + rx;
    float py = y;
    for (int i = 1; i <= 20; i++) {
        float a = i * 0.31416f;
        float nx = x + std::cos(a) * rx;
        float ny = y + std::sin(a) * ry;
        seg(s, px, py, nx, ny, w, c);
        px = nx;
        py = ny;
    }
}

inline void glow(BgState& s, CCPoint c, float r, ccColor4F col, int layers) {
    for (int i = 0; i < layers; i++) {
        float k = float(i) / float(layers);
        dot(s, c.x, c.y, r * (1.f - k),
            C(col.r, col.g, col.b, col.a * (0.25f + 0.75f * k) * 2.f / layers));
    }
}

inline void tri(BgState& s, CCPoint a, CCPoint b, CCPoint c, ccColor4F col) {
    CCPoint pts[3] = {a, b, c};
    s.draw->drawPolygon(pts, 3, col, 0.f, C(0.f, 0.f, 0.f, 0.f));
}

inline void petal(BgState& s, float x, float y, float size, float rot, float flip, ccColor4F col) {
    float w = size * (0.35f + 0.65f * std::fabs(std::cos(flip)));
    float c = std::cos(rot);
    float sn = std::sin(rot);
    CCPoint v[4] = {
        ccp(x - size * c, y - size * sn),
        ccp(x + w * sn, y - w * c),
        ccp(x + size * c, y + size * sn),
        ccp(x - w * sn, y + w * c)
    };
    s.draw->drawPolygon(v, 4, col, 0.f, C(0.f, 0.f, 0.f, 0.f));
}

inline void cloud(BgState& s, float x, float y, float size, float a) {
    ccColor4F c = C(1.f, 1.f, 1.f, a);
    dot(s, x, y, size * 0.5f, c);
    dot(s, x + size * 0.45f, y + size * 0.1f, size * 0.4f, c);
    dot(s, x - size * 0.45f, y + size * 0.05f, size * 0.38f, c);
    dot(s, x + size * 0.1f, y + size * 0.3f, size * 0.38f, c);
}

inline float hillY(float base, float amp, float freq, float drift, float x) {
    return base + amp * S(x * freq + drift);
}

inline void hills(BgState& s, float base, float amp, float freq, float drift, ccColor4F col) {
    for (float x = 0.f; x <= s.W + 8.f; x += 8.f) {
        seg(s, x, 0.f, x, hillY(base, amp, freq, drift, x), 4.5f, col);
    }
}

inline void stars(BgState& s) {
    for (auto& p : s.p) {
        float a = 0.35f + 0.65f * (0.5f + 0.5f * S(s.t * 2.5f + p.phase));
        dot(s, p.x, p.y, p.size, C(1.f, 1.f, 1.f, a));
    }
}

inline void makeBolt(BgState& s) {
    s.bolt.clear();
    s.bolt2.clear();
    float x = R(s.W * 0.2f, s.W * 0.8f);
    float y = s.H;
    s.bolt.push_back(ccp(x, y));
    while (y > 20.f) {
        float ny = y - R(14.f, 30.f);
        float nx = x + R(-18.f, 18.f);
        s.bolt.push_back(ccp(nx, ny));
        if (std::rand() % 4 == 0) {
            float bx = nx;
            float by = ny;
            float dir = (nx > s.W * 0.5f) ? -1.f : 1.f;
            for (int k = 0; k < 4; k++) {
                float bx2 = bx + R(-4.f, 22.f) * dir;
                float by2 = by - R(8.f, 18.f);
                s.bolt2.push_back(ccp(bx, by));
                s.bolt2.push_back(ccp(bx2, by2));
                bx = bx2;
                by = by2;
            }
        }
        x = nx;
        y = ny;
    }
}

inline void spawn(BgState& s, Particle& p, bool initial) {
    float W = s.W;
    float H = s.H;
    p = Particle{};
    p.phase = R(0.f, 6.28f);
    p.z = R(0.45f, 1.f);

    if (s.theme == 1) {
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
    } else if (s.theme == 2) {
        p.x = R(0.f, W);
        p.y = initial ? R(0.f, H) : H + 5.f;
        p.vx = R(-6.f, 6.f) * p.z;
        p.vy = -(15.f + 55.f * p.z);
        p.size = 1.f + 2.6f * p.z;
    } else if (s.theme == 3) {
        p.x = R(0.f, W);
        p.y = initial ? R(0.f, H) : H + 5.f;
        p.vx = R(4.f, 14.f) * p.z;
        p.vy = -R(6.f, 22.f) * p.z;
        p.size = R(1.2f, 3.f);
        p.kind = std::rand() % 3;
    } else if (s.theme == 4) {
        p.x = R(0.f, W);
        p.y = R(H * 0.3f, H);
        p.vx = R(-5.f, 5.f);
        p.vy = R(-3.f, 8.f);
        p.size = R(0.8f, 2.4f);
    } else if (s.theme == 5) {
        p.x = R(0.f, W * 1.3f);
        p.y = initial ? R(0.f, H) : H + 10.f;
        p.vx = -R(8.f, 26.f) * p.z;
        p.vy = -R(22.f, 50.f) * p.z;
        p.size = 2.5f + 3.f * p.z;
        p.rot = R(0.f, 6.28f);
        p.rotV = R(-2.f, 2.f);
        p.kind = std::rand() % 2;
    } else if (s.theme == 6 || s.theme == 7) {
        p.x = R(0.f, W);
        p.y = R(H * 0.25f, H);
        p.size = R(0.6f, 1.8f);
    } else if (s.theme == 8) {
        p.x = R(0.f, W * 1.2f);
        p.y = initial ? R(0.f, H) : H + 10.f;
        p.vx = -(60.f + 40.f * p.z);
        p.vy = -(380.f + 260.f * p.z);
        p.size = 8.f + 8.f * p.z;
    }
}

}
