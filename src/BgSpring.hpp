#pragma once
#include "BgCore.hpp"

namespace vanta {

// one wildflower: 0 daisy, 1 poppy, 2 tulip, 3 violet
inline void flower(BgState& s, float x, float y, float h, int type, float sway) {
    float tx = x + sway;
    float ty = y + h;
    seg(s, x, y - 1.f, tx, ty, 0.9f, C(0.2f, 0.5f, 0.2f, 1.f));
    seg(s, x + (tx - x) * 0.4f, y + h * 0.35f, x + (tx - x) * 0.4f + 4.f, y + h * 0.5f, 0.8f, C(0.22f, 0.55f, 0.22f, 1.f));
    if (type == 0) {
        for (int j = 0; j < 6; j++) {
            float a = j * 1.0472f;
            dot(s, tx + std::cos(a) * 2.9f, ty + std::sin(a) * 2.9f, 1.9f, C(1.f, 1.f, 1.f, 1.f));
        }
        dot(s, tx, ty, 1.5f, C(1.f, 0.85f, 0.2f, 1.f));
    } else if (type == 1) {
        for (int j = 0; j < 5; j++) {
            float a = j * 1.2566f;
            dot(s, tx + std::cos(a) * 2.3f, ty + std::sin(a) * 2.3f, 2.2f, C(0.95f, 0.18f, 0.15f, 1.f));
        }
        dot(s, tx, ty, 1.1f, C(0.12f, 0.08f, 0.08f, 1.f));
    } else if (type == 2) {
        dot(s, tx, ty, 2.6f, C(1.f, 0.4f, 0.55f, 1.f));
        tri(s, ccp(tx - 2.8f, ty + 0.5f), ccp(tx + 2.8f, ty + 0.5f), ccp(tx, ty + 5.f), C(1.f, 0.5f, 0.62f, 1.f));
    } else {
        for (int j = 0; j < 5; j++) {
            float a = j * 1.2566f + 0.4f;
            dot(s, tx + std::cos(a) * 2.f, ty + std::sin(a) * 2.f, 1.7f, C(0.6f, 0.45f, 0.95f, 1.f));
        }
        dot(s, tx, ty, 1.f, C(1.f, 0.9f, 0.3f, 1.f));
    }
}

inline void themeSpringX(BgState& s, float dt, float wind) {
    float W = s.W, H = s.H, t = s.t;

    // sun and soft light shafts
    glow(s, ccp(W * 0.15f, H * 0.85f), 105.f, C(1.f, 0.98f, 0.75f, 0.3f), 5);
    dot(s, W * 0.15f, H * 0.85f, 22.f, C(1.f, 0.99f, 0.88f, 1.f));
    for (int i = 0; i < 5; i++) {
        float x0 = W * (0.05f + 0.12f * i) + S(t * 0.2f + i) * 10.f;
        seg(s, x0, H, x0 + 130.f, 0.f, 16.f, C(1.f, 1.f, 0.85f, 0.04f));
    }

    // clouds
    for (int i = 0; i < 5; i++) {
        float cx = std::fmod(i * 170.f + t * (4.f + i * 2.f), W + 240.f) - 120.f;
        cloud(s, cx, H * (0.66f + 0.07f * S(i * 2.f)), 40.f + 9.f * i, 0.8f);
    }

    // far and middle hills with a little haze between them
    hills(s, 78.f, 18.f, 0.006f, t * 0.04f, C(0.62f, 0.82f, 0.74f, 1.f));
    rect(s, 0.f, 60.f, W, 96.f, C(1.f, 1.f, 0.95f, 0.10f));
    hills(s, 56.f, 16.f, 0.01f, 1.f + t * 0.08f, C(0.5f, 0.76f, 0.5f, 1.f));

    // blossoming trees on the middle hills
    for (int i = 0; i < 6; i++) {
        float tx = W * (i + 0.5f) / 6.f + S(i * 4.3f) * 20.f;
        float ty = hillY(56.f, 16.f, 0.01f, 1.f + t * 0.08f, tx);
        float sw = S(t * 1.1f + i) * 1.5f + wind * 0.05f;
        seg(s, tx, ty, tx + sw * 0.4f, ty + 18.f, 3.f, C(0.36f, 0.24f, 0.16f, 1.f));
        dot(s, tx + sw, ty + 26.f, 15.f, C(0.2f, 0.55f, 0.24f, 1.f));
        dot(s, tx - 9.f + sw, ty + 20.f, 10.f, C(0.26f, 0.62f, 0.28f, 1.f));
        dot(s, tx + 9.f + sw, ty + 21.f, 10.f, C(0.24f, 0.6f, 0.27f, 1.f));
        dot(s, tx + 5.f + sw, ty + 31.f, 6.f, C(0.32f, 0.68f, 0.32f, 0.9f));
        if (i % 2 == 0) {
            dot(s, tx + 4.f + sw, ty + 30.f, 3.f, C(1.f, 0.75f, 0.85f, 1.f));
        }
    }

    // meadow
    hills(s, 34.f, 12.f, 0.013f, 2.f + t * 0.12f, C(0.4f, 0.7f, 0.3f, 1.f));

    // stream winding through the meadow
    for (float x = -4.f; x <= W + 4.f; x += 5.f) {
        float yc = 17.f + 5.f * S(x * 0.015f + 1.2f);
        seg(s, x, yc - 5.f, x, yc + 5.f, 5.5f, C(0.42f, 0.72f, 0.92f, 1.f));
        seg(s, x, yc + 1.f, x, yc + 5.f, 5.5f, C(0.65f, 0.85f, 0.98f, 0.7f));
        seg(s, x, yc + 5.f, x, yc + 8.f, 5.5f, C(0.3f, 0.58f, 0.26f, 1.f));
        float a = S(x * 0.22f - t * 2.4f);
        if (a > 0.7f) {
            seg(s, x, yc + 1.f, x + 7.f, yc + 1.f, 1.2f, C(1.f, 1.f, 1.f, a * 0.7f));
        }
    }

    // grass with wind waves rolling across it
    for (float x = 0.f; x <= W; x += 4.5f) {
        float gy = hillY(34.f, 12.f, 0.013f, 2.f + t * 0.12f, x);
        float lean = S(x * 0.02f - t * 1.3f) * 3.5f + wind * 0.12f;
        float len = 10.f + 6.f * S(x * 0.37f) + 3.f * S(x * 0.11f);
        seg(s, x, gy - 2.f, x + lean, gy + len, 1.1f,
            C(0.2f + 0.1f * S(x), 0.52f + 0.12f * S(x * 0.5f), 0.2f, 1.f));
    }

    // wildflowers
    for (int i = 0; i < 26; i++) {
        float fx = W * (i + 0.5f) / 26.f + S(i * 7.1f) * 8.f;
        float fy = hillY(34.f, 12.f, 0.013f, 2.f + t * 0.12f, fx);
        float h = 9.f + 6.f * (0.5f + 0.5f * S(i * 3.3f));
        float sw = S(t * 1.8f + fx * 0.04f) * 3.f + wind * 0.1f;
        flower(s, fx, fy - 2.f, h, i % 4, sw);
    }

    // bees buzzing around the flowers
    for (int b = 0; b < 4; b++) {
        int fi = (b * 6) % 26;
        float fx0 = W * (fi + 0.5f) / 26.f;
        float fy0 = hillY(34.f, 12.f, 0.013f, 2.f + t * 0.12f, fx0) + 14.f;
        float bx = fx0 + 16.f * S(t * 1.6f + b * 2.f) + 6.f * S(t * 5.f + b);
        float by = fy0 + 8.f * S(t * 2.1f + b);
        dot(s, bx, by, 1.7f, C(1.f, 0.8f, 0.1f, 1.f));
        dot(s, bx - 0.9f, by, 1.f, C(0.1f, 0.1f, 0.1f, 1.f));
        dot(s, bx - 0.5f, by + 1.8f, 1.8f, C(1.f, 1.f, 1.f, 0.4f + 0.3f * S(t * 60.f + b)));
    }

    // butterflies
    for (int b = 0; b < 3; b++) {
        float cx = W * (0.3f + 0.22f * b);
        float cy = H * 0.4f + b * 22.f;
        float bx = cx + 90.f * S(t * 0.5f + b * 2.f);
        float by = cy + 28.f * S(t * 1.0f + b * 2.f);
        float flap = std::fabs(S(t * 13.f + b)) * 5.f + 1.f;
        ccColor4F wc = (b % 2 == 0) ? C(1.f, 0.6f, 0.2f, 0.95f) : C(0.5f, 0.7f, 1.f, 0.95f);
        dot(s, bx - flap, by, 3.f, wc);
        dot(s, bx + flap, by, 3.f, wc);
        dot(s, bx, by, 1.2f, C(0.15f, 0.1f, 0.1f, 1.f));
    }

    // dandelion seeds drifting on the breeze
    for (int i = 0; i < 9; i++) {
        float u = std::fmod(t * 0.03f + i * 0.11f, 1.f);
        float x = -20.f + u * (W + 40.f);
        float y = H * 0.2f + 60.f * S(u * 6.28f + i) + i * 8.f;
        for (int j = 0; j < 6; j++) {
            float a = j * 1.047f + t * 0.5f;
            seg(s, x, y, x + std::cos(a) * 3.5f, y + std::sin(a) * 3.5f, 0.5f, C(1.f, 1.f, 1.f, 0.8f));
        }
        dot(s, x, y, 0.9f, C(1.f, 1.f, 1.f, 0.9f));
    }

    // falling petals and pollen
    for (auto& p : s.p) {
        p.y += p.vy * dt;
        p.x += (p.vx + wind * 0.5f + S(t * 1.2f + p.phase) * 12.f) * dt;
        if (p.y < -5.f || p.x > W + 5.f) {
            spawn(s, p, false);
            continue;
        }
        ccColor4F c = C(1.f, 0.92f, 0.4f, 0.9f);
        if (p.kind == 0) c = C(1.f, 0.7f, 0.82f, 0.95f);
        if (p.kind == 1) c = C(1.f, 1.f, 1.f, 0.9f);
        dot(s, p.x, p.y, p.size, c);
    }
}

}
