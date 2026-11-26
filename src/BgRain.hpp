#pragma once
#include "BgCore.hpp"

namespace vanta {

// dark storm cloud banks that light up when lightning strikes
inline void rainClouds(BgState& s) {
    float W = s.W, H = s.H, t = s.t;
    float fi = s.flash * (0.65f + 0.35f * S(t * 55.f));
    float lit = 0.5f * fi;
    for (int layer = 0; layer < 3; layer++) {
        float base = 0.07f + 0.04f * layer;
        for (int i = 0; i < 9; i++) {
            float cx = std::fmod(i * 140.f + t * (5.f + layer * 4.f) + layer * 60.f, W + 320.f) - 160.f;
            float cy = H * (0.9f - layer * 0.07f) + 5.f * S(t * 0.2f + i + layer);
            for (int k = 0; k < 5; k++) {
                float px = cx + (k - 2) * 38.f;
                float py = cy + S(k * 2.1f + i) * 8.f;
                float r = 52.f + 14.f * S(k * 1.3f + i * 0.7f) + layer * 10.f;
                dot(s, px, py, r, C(base + lit * 0.6f, base + 0.01f + lit * 0.65f, base + 0.04f + lit * 0.8f, 0.85f));
                dot(s, px, py + r * 0.25f, r * 0.7f, C(base + 0.03f + lit * 0.7f, base + 0.04f + lit * 0.72f, base + 0.07f + lit * 0.85f, 0.5f));
            }
        }
    }
    if (s.flash > 0.f && !s.bolt.empty()) {
        glow(s, s.bolt[0], 150.f, C(0.75f, 0.85f, 1.f, 0.5f * fi), 4);
    }
}

inline void themeRainX(BgState& s, float dt, float wind) {
    float W = s.W, H = s.H, t = s.t;
    float groundY = 14.f;
    float slant = wind * 3.f;

    // faint sheets of distant rain
    for (int i = 0; i < 6; i++) {
        float x = std::fmod(i * 131.f + t * (30.f + i * 5.f), W + 200.f) - 100.f;
        seg(s, x, H, x - 30.f - slant * 0.2f, 0.f, 32.f, C(0.7f, 0.78f, 0.9f, 0.03f));
    }

    // lightning
    s.nextFlash -= dt;
    if (s.nextFlash <= 0.f) {
        s.flash = 1.f;
        makeBolt(s);
        s.nextFlash = R(4.f, 9.f);
    }
    float fi = s.flash * (0.65f + 0.35f * S(t * 55.f));
    if (s.flash > 0.f) {
        rect(s, 0.f, 0.f, W, H, C(0.85f, 0.9f, 1.f, fi * 0.28f));
        glow(s, ccp(W * 0.5f, H * 0.8f), W * 0.6f, C(0.7f, 0.8f, 1.f, 0.25f * fi), 4);
        for (size_t i = 1; i < s.bolt.size(); i++) {
            segA(s, s.bolt[i - 1].x, s.bolt[i - 1].y, s.bolt[i].x, s.bolt[i].y, 5.f, C(0.6f, 0.75f, 1.f, 0.25f * fi));
            seg(s, s.bolt[i - 1].x, s.bolt[i - 1].y, s.bolt[i].x, s.bolt[i].y, 1.4f, C(1.f, 1.f, 1.f, fi));
        }
        for (size_t i = 0; i + 1 < s.bolt2.size(); i += 2) {
            seg(s, s.bolt2[i].x, s.bolt2[i].y, s.bolt2[i + 1].x, s.bolt2[i + 1].y, 0.8f, C(0.9f, 0.95f, 1.f, fi * 0.8f));
        }
        s.flash -= dt * 2.4f;
        if (s.flash < 0.f) s.flash = 0.f;
    }

    // rain in three depth layers
    for (auto& p : s.p) {
        float vx = p.vx + slant * (0.5f + 0.5f * p.z);
        p.x += vx * dt;
        p.y += p.vy * dt;
        if (p.x < -20.f) p.x += W + 40.f;
        if (p.y < groundY) {
            if (p.z > 0.55f && s.fx.size() < 150) {
                Particle drop;
                drop.x = p.x;
                drop.y = groundY;
                drop.kind = 0;
                drop.maxLife = 0.25f;
                s.fx.push_back(drop);
                Particle rip;
                rip.x = p.x;
                rip.y = 5.f + R(0.f, 8.f);
                rip.kind = 1;
                rip.maxLife = 0.8f;
                s.fx.push_back(rip);
            }
            spawn(s, p, false);
            continue;
        }
        bool isFar = p.z < 0.62f;
        bool isNear = p.z >= 0.85f;
        float len = isFar ? 0.02f : (isNear ? 0.045f : 0.03f);
        float w = isFar ? 0.4f : (isNear ? 0.95f : 0.6f);
        float a = isFar ? 0.14f : (isNear ? 0.42f : 0.26f);
        a += 0.3f * fi * p.z;
        seg(s, p.x, p.y, p.x - vx * len, p.y - p.vy * len, w, C(0.78f, 0.86f, 1.f, std::min(0.85f, a)));
    }

    // wet pavement
    rect(s, 0.f, 0.f, W, groundY, C(0.04f, 0.05f, 0.08f, 0.9f));
    rect(s, 0.f, groundY - 4.f, W, groundY, C(0.1f, 0.12f, 0.17f, 0.5f + 0.4f * fi));
    for (int i = 0; i < 14; i++) {
        float x = std::fmod(i * 61.3f, W);
        float wob = 2.f * S(t * 1.5f + i);
        segA(s, x + wob, 0.f, x + wob, groundY - 1.f, 3.f, C(1.f, 0.8f, 0.5f, 0.05f + 0.15f * fi));
    }
    for (int q = 0; q < 5; q++) {
        float cx = W * (0.12f + 0.19f * q);
        float hw = 22.f + 9.f * S(q * 2.f);
        seg(s, cx - hw, 7.f, cx + hw, 7.f, 4.f, C(0.45f, 0.5f, 0.62f, 0.28f + 0.45f * fi));
    }

    // ripples and splashes
    for (size_t i = 0; i < s.fx.size();) {
        auto& f = s.fx[i];
        f.age += dt;
        if (f.age > f.maxLife) {
            s.fx.erase(s.fx.begin() + i);
            continue;
        }
        float k = f.age / f.maxLife;
        if (f.kind == 1) {
            float r = f.age * 38.f;
            ellipse(s, f.x, f.y, r, r * 0.28f, 0.8f, C(0.8f, 0.9f, 1.f, 0.5f * (1.f - k)));
            if (r > 6.f) {
                ellipse(s, f.x, f.y, r * 0.6f, r * 0.17f, 0.6f, C(0.8f, 0.9f, 1.f, 0.3f * (1.f - k)));
            }
        } else {
            for (int d = -1; d <= 1; d++) {
                float dx = f.x + d * f.age * 40.f;
                float dy = groundY + 30.f * f.age - 140.f * f.age * f.age;
                if (dy > groundY - 2.f) {
                    dot(s, dx, dy, 1.f, C(0.8f, 0.9f, 1.f, 0.6f * (1.f - k)));
                }
            }
        }
        i++;
    }

    // low drifting mist
    for (int m = 0; m < 4; m++) {
        float x = std::fmod(m * 190.f + t * (9.f + m * 4.f), W + 300.f) - 150.f;
        seg(s, x, 18.f + m * 6.f, x + 190.f, 18.f + m * 6.f, 18.f, C(0.7f, 0.76f, 0.88f, 0.05f));
    }
}

}
