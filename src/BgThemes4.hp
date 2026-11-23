#pragma once
#include "BgCore.hpp"

namespace vanta {

inline void themeStars(BgState& s, float dt) {
    float W = s.W, H = s.H, t = s.t;

    glow(s, ccp(W * 0.25f, H * 0.55f), 120.f * (1.f + 0.05f * S(t * 0.4f)), C(0.5f, 0.2f, 0.9f, 0.08f), 4);
    glow(s, ccp(W * 0.75f, H * 0.4f), 140.f, C(0.2f, 0.4f, 0.9f, 0.07f), 4);
    glow(s, ccp(W * 0.5f, H * 0.7f), 100.f, C(0.9f, 0.3f, 0.5f, 0.05f), 4);

    for (int i = 0; i < 150; i++) {
        float u = i / 150.f;
        float px = W * u;
        float py = H * (0.2f + 0.6f * u) + S(i * 12.9f) * 30.f;
        dot(s, px, py, 0.8f + 0.6f * std::fabs(S(i * 3.3f)), C(0.88f, 0.88f, 1.f, 0.12f + 0.12f * S(t + i)));
    }

    stars(s);

    for (int i = 0; i < 6; i++) {
        float fx = W * (i + 0.5f) / 6.5f;
        float fy = H * (0.35f + 0.5f * std::fmod(i * 0.37f, 1.f));
        float a = 0.5f + 0.5f * S(t * 1.5f + i);
        seg(s, fx - 6.f, fy, fx + 6.f, fy, 0.8f, C(1.f, 1.f, 1.f, 0.5f * a));
        seg(s, fx, fy - 6.f, fx, fy + 6.f, 0.8f, C(1.f, 1.f, 1.f, 0.5f * a));
        dot(s, fx, fy, 2.f, C(1.f, 1.f, 1.f, 0.9f * a));
    }

    float px = W * 0.18f;
    float py = H * 0.3f + 3.f * S(t * 0.2f);
    float tilt = -0.35f;
    glow(s, ccp(px, py), 48.f, C(0.6f, 0.5f, 0.9f, 0.12f), 4);

    for (int pass = 0; pass < 2; pass++) {
        if (pass == 1) {
            dot(s, px, py, 18.f, C(0.5f, 0.4f, 0.75f, 1.f));
            dot(s, px - 5.f, py + 5.f, 12.f, C(0.7f, 0.62f, 0.92f, 0.5f));
            dot(s, px + 6.f, py - 5.f, 15.f, C(0.1f, 0.08f, 0.2f, 0.35f));
        }
        for (int k = 0; k < 40; k++) {
            float a = k * 0.157f;
            bool front = S(a) < 0.f;
            if ((pass == 1) != front) continue;
            float ex = std::cos(a) * 38.f;
            float ey = S(a) * 10.f;
            float rx = ex * std::cos(tilt) - ey * std::sin(tilt);
            float ry = ex * std::sin(tilt) + ey * std::cos(tilt);
            dot(s, px + rx, py + ry, 1.3f, C(0.85f, 0.8f, 1.f, 0.6f));
        }
    }

    s.ssTimer -= dt;
    if (!s.ssOn && s.ssTimer <= 0.f) {
        s.ssOn = true;
        s.ssX = R(W * 0.3f, W);
        s.ssY = R(H * 0.6f, H);
        s.ssTimer = R(3.f, 7.f);
    }
    if (s.ssOn) {
        s.ssX -= 420.f * dt;
        s.ssY -= 180.f * dt;
        for (int k = 0; k < 6; k++) {
            float off = k * 16.f;
            seg(s, s.ssX + off, s.ssY + off * 0.42f, s.ssX + off + 16.f, s.ssY + (off + 16.f) * 0.42f,
                1.4f - k * 0.2f, C(1.f, 1.f, 1.f, 0.75f - k * 0.12f));
        }
        dot(s, s.ssX, s.ssY, 2.2f, C(1.f, 1.f, 1.f, 1.f));
        if (s.ssX < -110.f || s.ssY < -40.f) s.ssOn = false;
    }
}

inline void themeRain(BgState& s, float dt, float wind) {
    float W = s.W, H = s.H, t = s.t;
    float groundY = 14.f;

    for (int i = 0; i < 8; i++) {
        float cx = std::fmod(i * 120.f + t * (8.f + (i % 3) * 4.f), W + 240.f) - 120.f;
        float cy = H + 4.f - (i % 3) * 10.f;
        dot(s, cx, cy, 70.f + 10.f * (i % 3), C(0.05f, 0.06f, 0.09f, 0.5f));
        if (s.flash > 0.f) {
            dot(s, cx, cy - 6.f, 60.f, C(0.6f, 0.7f, 0.95f, s.flash * 0.12f));
        }
    }

    s.nextFlash -= dt;
    if (s.nextFlash <= 0.f) {
        s.flash = 1.f;
        makeBolt(s);
        s.nextFlash = R(4.f, 9.f);
    }
    if (s.flash > 0.f) {
        rect(s, 0.f, 0.f, W, H, C(0.85f, 0.9f, 1.f, s.flash * 0.3f));
        for (size_t i = 1; i < s.bolt.size(); i++) {
            seg(s, s.bolt[i - 1].x, s.bolt[i - 1].y, s.bolt[i].x, s.bolt[i].y, 3.f,
                C(0.7f, 0.8f, 1.f, s.flash * 0.25f));
            seg(s, s.bolt[i - 1].x, s.bolt[i - 1].y, s.bolt[i].x, s.bolt[i].y, 1.2f,
                C(1.f, 1.f, 1.f, s.flash));
        }
        for (size_t i = 0; i + 1 < s.bolt2.size(); i += 2) {
            seg(s, s.bolt2[i].x, s.bolt2[i].y, s.bolt2[i + 1].x, s.bolt2[i + 1].y, 0.8f,
                C(0.9f, 0.95f, 1.f, s.flash * 0.8f));
        }
        s.flash -= dt * 2.8f;
    }

    for (auto& p : s.p) {
        float vx = p.vx + wind * 3.f;
        p.x += vx * dt;
        p.y += p.vy * dt;
        if (p.y < groundY) {
            if (s.fx.size() < 140) {
                Particle drop;
                drop.x = p.x;
                drop.kind = 0;
                drop.maxLife = 0.25f;
                s.fx.push_back(drop);
                Particle rip;
                rip.x = p.x;
                rip.kind = 1;
                rip.maxLife = 0.8f;
                s.fx.push_back(rip);
            }
            spawn(s, p, false);
            continue;
        }
        seg(s, p.x, p.y, p.x - vx * 0.025f, p.y - p.vy * 0.025f, 0.5f + 0.4f * p.z,
            C(0.75f, 0.85f, 1.f, 0.25f + 0.35f * p.z));
    }

    rect(s, 0.f, 0.f, W, groundY, C(0.04f, 0.05f, 0.09f, 0.8f));
    for (int i = 0; i < 10; i++) {
        float gx = std::fmod(i * 61.3f, W);
        seg(s, gx, 0.f, gx, groundY - 2.f, 3.f, C(0.6f, 0.7f, 0.9f, 0.05f + s.flash * 0.2f));
    }

    for (size_t i = 0; i < s.fx.size();) {
        auto& f = s.fx[i];
        f.age += dt;
        if (f.age > f.maxLife) {
            s.fx.erase(s.fx.begin() + i);
            continue;
        }
        float k = f.age / f.maxLife;
        if (f.kind == 1) {
            float r = f.age * 45.f;
            ellipse(s, f.x, 9.f, r, r * 0.28f, 0.8f, C(0.8f, 0.9f, 1.f, 0.5f * (1.f - k)));
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

    for (float x = -50.f; x < W + 50.f; x += 50.f) {
        float fy = 20.f + 6.f * S(x * 0.01f + t * 0.25f);
        seg(s, x, fy, x + 50.f, fy, 22.f, C(0.7f, 0.78f, 0.9f, 0.05f));
    }
}

}
