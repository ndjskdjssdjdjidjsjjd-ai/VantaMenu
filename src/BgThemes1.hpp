#pragma once
#include "BgCore.hpp"

namespace vanta {

inline void themeFire(BgState& s, float dt, float wind) {
    float W = s.W, H = s.H, t = s.t;
    float fl = 0.85f + 0.15f * S(t * 9.f) * S(t * 5.3f);

    for (int i = 0; i < 9; i++) {
        float r = (115.f + S(t * 2.2f + i * 1.7f) * 22.f) * fl;
        glow(s, ccp(W * i / 8.f, -30.f), r, C(1.f, 0.4f, 0.05f, 0.24f), 5);
    }
    glow(s, ccp(W * 0.5f, H * 0.3f), W * 0.55f, C(1.f, 0.3f, 0.05f, 0.08f * fl), 4);

    for (float x = -8.f; x <= W + 8.f; x += 7.f) {
        float n = fbm(x * 0.03f, t * 4.f) * 0.6f + fbm(x * 0.09f + 7.f, t * 7.f) * 0.4f;
        float h = 80.f + 60.f * n + 25.f * S(x * 0.008f + t * 0.6f);
        float lean = S(x * 0.05f + t * 3.f) * 9.f + wind * 0.6f;
        tri(s, ccp(x - 9.f, 0.f), ccp(x + 9.f, 0.f), ccp(x + lean, h), C(0.8f, 0.1f, 0.02f, 0.5f));
        tri(s, ccp(x - 7.f, 0.f), ccp(x + 7.f, 0.f), ccp(x + lean * 0.8f, h * 0.72f), C(1.f, 0.42f, 0.05f, 0.62f));
        tri(s, ccp(x - 5.f, 0.f), ccp(x + 5.f, 0.f), ccp(x + lean * 0.55f, h * 0.45f), C(1.f, 0.8f, 0.25f, 0.78f));
        tri(s, ccp(x - 3.f, 0.f), ccp(x + 3.f, 0.f), ccp(x + lean * 0.3f, h * 0.22f), C(1.f, 0.98f, 0.8f, 0.85f));
    }

    for (int i = 0; i < 10; i++) {
        float y = H * 0.3f + i * 14.f;
        for (float x = 0.f; x < W; x += 30.f) {
            float y1 = y + S(x * 0.05f + t * 4.f + i) * 3.f;
            float y2 = y + S((x + 30.f) * 0.05f + t * 4.f + i) * 3.f;
            seg(s, x, y1, x + 30.f, y2, 1.f, C(1.f, 0.6f, 0.2f, 0.035f));
        }
    }

    for (auto& p : s.p) {
        p.age += dt;
        if (p.age >= p.maxLife) {
            spawn(s, p, false);
            continue;
        }
        float k = p.age / p.maxLife;
        if (p.kind == 1) {
            p.x += (p.vx + wind * 0.4f + S(t + p.phase) * 10.f) * dt;
            p.y += p.vy * dt;
            dot(s, p.x, p.y, p.size * (0.6f + k), C(0.1f, 0.08f, 0.08f, 0.16f * (1.f - k)));
        } else {
            float vx = p.vx + S(t * 3.f + p.phase) * 24.f + wind;
            p.x += vx * dt;
            p.y += p.vy * dt;
            float tw = 0.7f + 0.3f * S(t * 22.f + p.phase * 5.f);
            seg(s, p.x - vx * 0.05f, p.y - p.vy * 0.05f, p.x, p.y, p.size * (1.f - 0.5f * k),
                C(1.f, 0.55f - 0.4f * k, 0.1f, 0.6f * (1.f - k)));
            glow(s, ccp(p.x, p.y), p.size * 2.6f * (1.f - 0.6f * k), C(1.f, 0.5f, 0.1f, 0.35f * (1.f - k)), 2);
            dot(s, p.x, p.y, p.size * (1.f - 0.6f * k), C(1.f, 0.95f - 0.6f * k, 0.35f * (1.f - k), tw * (1.f - k)));
        }
    }
}

inline void themeWinter(BgState& s, float dt, float wind) {
    float W = s.W, H = s.H, t = s.t;

    for (int i = 0; i < 50; i++) {
        float sx = std::fmod(i * 97.3f, W);
        float sy = H * 0.4f + std::fmod(i * 53.7f, H * 0.6f);
        dot(s, sx, sy, 0.9f, C(1.f, 1.f, 1.f, 0.25f + 0.5f * (0.5f + 0.5f * S(t * 1.8f + i))));
    }

    float mx = W * 0.8f;
    float my = H * 0.78f;
    glow(s, ccp(mx, my), 80.f, C(0.8f, 0.9f, 1.f, 0.2f), 6);
    dot(s, mx, my, 24.f, C(0.95f, 0.97f, 1.f, 1.f));
    dot(s, mx - 7.f, my + 5.f, 5.f, C(0.82f, 0.86f, 0.93f, 1.f));
    dot(s, mx + 8.f, my - 6.f, 3.5f, C(0.85f, 0.88f, 0.94f, 1.f));

    for (float x = 0.f; x <= W + 16.f; x += 16.f) {
        float h1 = H * 0.3f + 42.f * S(x * 0.007f + 1.f) + 20.f * S(x * 0.021f);
        seg(s, x, 0.f, x, h1, 17.f, C(0.16f, 0.22f, 0.34f, 1.f));
        if (h1 > H * 0.33f) {
            seg(s, x, h1 - 14.f, x, h1, 17.f, C(0.82f, 0.9f, 0.97f, 1.f));
        }
        float h2 = H * 0.17f + 26.f * S(x * 0.011f + 3.f) + 12.f * S(x * 0.04f);
        seg(s, x, 0.f, x, h2, 17.f, C(0.09f, 0.15f, 0.24f, 1.f));
    }

    for (int i = 0; i < 10; i++) {
        float tx = W * (i + 0.5f) / 10.f + S(i * 3.1f) * 16.f;
        float th = 58.f + 28.f * (0.5f + 0.5f * S(i * 2.3f));
        float sw = S(t * 0.9f + i) * 1.8f + wind * 0.08f;
        seg(s, tx, 4.f, tx, 18.f, 2.6f, C(0.22f, 0.13f, 0.09f, 1.f));
        for (int k = 0; k < 3; k++) {
            float by = 14.f + th * 0.28f * k;
            float hw = th * (0.36f - 0.07f * k);
            float ht = th * 0.45f;
            float apx = tx + sw * (k + 1);
            tri(s, ccp(tx - hw, by), ccp(tx + hw, by), ccp(apx, by + ht), C(0.05f, 0.14f, 0.17f, 1.f));
            tri(s, ccp(tx - hw * 0.55f, by + ht * 0.55f), ccp(tx + hw * 0.55f, by + ht * 0.55f),
                ccp(apx, by + ht), C(0.92f, 0.96f, 1.f, 0.95f));
        }
    }

    seg(s, 0.f, 5.f, W, 5.f, 12.f, C(0.93f, 0.96f, 1.f, 1.f));
    for (float x = 0.f; x <= W; x += 20.f) {
        dot(s, x, 9.f + 4.f * S(x * 0.03f), 14.f, C(0.95f, 0.97f, 1.f, 1.f));
    }

    for (float x = -40.f; x < W + 40.f; x += 40.f) {
        float fy = 24.f + 8.f * S(x * 0.01f + t * 0.3f);
        seg(s, x, fy, x + 40.f, fy, 20.f, C(0.85f, 0.92f, 1.f, 0.06f));
    }

    for (int i = 0; i < 18; i++) {
        float gx = std::fmod(i * 73.1f, W);
        float gy = 8.f + std::fmod(i * 17.3f, 16.f);
        float a = std::max(0.f, S(t * 2.5f + i * 2.3f));
        a = a * a * a;
        seg(s, gx - 3.f, gy, gx + 3.f, gy, 0.7f, C(1.f, 1.f, 1.f, a));
        seg(s, gx, gy - 3.f, gx, gy + 3.f, 0.7f, C(1.f, 1.f, 1.f, a));
    }

    for (auto& p : s.p) {
        p.y += p.vy * dt;
        p.x += (p.vx + wind * p.z * 2.2f + S(t * 1.5f + p.phase) * 12.f * p.z) * dt;
        if (p.y < -5.f) {
            spawn(s, p, false);
            continue;
        }
        if (p.x < -5.f) p.x += W + 10.f;
        if (p.x > W + 5.f) p.x -= W + 10.f;
        if (p.z > 0.8f) {
            glow(s, ccp(p.x, p.y), p.size * 2.4f, C(1.f, 1.f, 1.f, 0.2f), 2);
        }
        dot(s, p.x, p.y, p.size, C(1.f, 1.f, 1.f, 0.4f + 0.55f * p.z));
    }
}

}
