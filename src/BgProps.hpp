#pragma once
#include "BgCore.hpp"

namespace vanta {

// Small scene details drawn on top of each background (cabin, windmill, boat, ...)

inline void propsFire(BgState& s) {
    float W = s.W, t = s.t;
    float fl = 0.8f + 0.2f * S(t * 13.f) * S(t * 7.1f);
    for (float x = -10.f; x <= W + 10.f; x += 12.f) {
        float h = 16.f + 12.f * S(x * 0.02f) + 7.f * S(x * 0.07f + 1.f);
        seg(s, x, 0.f, x, h, 12.f, C(0.05f, 0.02f, 0.02f, 1.f));
    }
    for (int i = 0; i < 14; i++) {
        float x = std::fmod(i * 83.7f, W);
        float y = 3.f + std::fmod(i * 5.3f, 8.f);
        seg(s, x, y, x + 18.f + (i % 4) * 5.f, y + 2.f * S(i * 2.1f), 1.6f, C(1.f, 0.5f, 0.1f, 0.65f * fl));
        glow(s, ccp(x + 10.f, y), 12.f, C(1.f, 0.4f, 0.05f, 0.12f * fl), 2);
    }
}

inline void propsWinter(BgState& s, float wind) {
    float W = s.W, t = s.t;
    float cx = W * 0.14f;
    float base = 14.f;
    float fl = 0.85f + 0.15f * S(t * 11.f);
    rect(s, cx - 22.f, base, cx + 22.f, base + 22.f, C(0.12f, 0.08f, 0.07f, 1.f));
    rect(s, cx + 12.f, base + 22.f, cx + 18.f, base + 38.f, C(0.1f, 0.07f, 0.06f, 1.f));
    tri(s, ccp(cx - 28.f, base + 22.f), ccp(cx + 28.f, base + 22.f), ccp(cx, base + 42.f), C(0.18f, 0.12f, 0.1f, 1.f));
    tri(s, ccp(cx - 22.f, base + 26.f), ccp(cx + 22.f, base + 26.f), ccp(cx, base + 42.f), C(0.93f, 0.96f, 1.f, 1.f));
    rect(s, cx - 8.f, base + 6.f, cx + 6.f, base + 16.f, C(1.f, 0.78f, 0.35f, fl));
    glow(s, ccp(cx - 1.f, base + 11.f), 30.f, C(1.f, 0.7f, 0.3f, 0.25f * fl), 4);
    for (int k = 0; k < 6; k++) {
        float age = std::fmod(t * 0.4f + k / 6.f, 1.f);
        float px = cx + 15.f + wind * age * 3.f + S(t + k) * 3.f * age;
        float py = base + 40.f + age * 55.f;
        dot(s, px, py, 3.f + 6.f * age, C(0.75f, 0.78f, 0.85f, 0.28f * (1.f - age)));
    }
}

inline void propsSpring(BgState& s) {
    float W = s.W, t = s.t;
    float wx = W * 0.82f;
    float wy = hillY(56.f, 16.f, 0.01f, 1.f + t * 0.08f, wx) + 4.f;
    seg(s, wx, wy, wx, wy + 30.f, 9.f, C(0.92f, 0.87f, 0.8f, 1.f));
    tri(s, ccp(wx - 7.f, wy + 30.f), ccp(wx + 7.f, wy + 30.f), ccp(wx, wy + 40.f), C(0.6f, 0.25f, 0.2f, 1.f));
    float rot = t * 0.9f;
    for (int k = 0; k < 4; k++) {
        float a = rot + k * 1.5708f;
        float cx = std::cos(a);
        float cy = std::sin(a);
        seg(s, wx, wy + 33.f, wx + cx * 26.f, wy + 33.f + cy * 26.f, 1.8f, C(0.4f, 0.3f, 0.25f, 1.f));
        seg(s, wx + cx * 9.f, wy + 33.f + cy * 9.f, wx + cx * 26.f, wy + 33.f + cy * 26.f, 4.5f,
            C(0.96f, 0.95f, 0.9f, 0.92f));
    }
    dot(s, wx, wy + 33.f, 2.5f, C(0.3f, 0.2f, 0.18f, 1.f));
}

inline void propsSummer(BgState& s) {
    float W = s.W, H = s.H, t = s.t;
    float hy = H * 0.34f;
    float bx = std::fmod(t * 14.f, W + 120.f) - 60.f;
    float by = hy * 0.55f + 2.f * S(t * 1.5f);
    float tilt = 3.f * S(t * 1.2f);
    tri(s, ccp(bx - 16.f, by + 6.f), ccp(bx + 16.f, by + 6.f), ccp(bx + 10.f, by), C(0.95f, 0.95f, 0.98f, 1.f));
    seg(s, bx, by + 6.f, bx + tilt * 0.3f, by + 34.f, 1.4f, C(0.35f, 0.25f, 0.2f, 1.f));
    tri(s, ccp(bx + 1.f, by + 8.f), ccp(bx + 1.f + tilt * 0.3f, by + 33.f), ccp(bx + 16.f, by + 8.f), C(1.f, 1.f, 1.f, 1.f));
    tri(s, ccp(bx - 1.f, by + 9.f), ccp(bx - 1.f + tilt * 0.3f, by + 29.f), ccp(bx - 11.f, by + 9.f), C(1.f, 0.9f, 0.85f, 1.f));
    seg(s, bx - 20.f, by - 1.f, bx + 20.f, by - 1.f, 2.f, C(1.f, 1.f, 1.f, 0.35f));
}

inline void propsBlossom(BgState& s) {
    float W = s.W, t = s.t;
    float gx = W * 0.3f;
    float gy = 40.f;
    ccColor4F red = C(0.78f, 0.16f, 0.14f, 1.f);
    seg(s, gx - 16.f, gy, gx - 16.f, gy + 40.f, 4.f, red);
    seg(s, gx + 16.f, gy, gx + 16.f, gy + 40.f, 4.f, red);
    seg(s, gx - 26.f, gy + 41.f, gx + 26.f, gy + 41.f, 5.f, red);
    seg(s, gx - 20.f, gy + 30.f, gx + 20.f, gy + 30.f, 3.f, red);
    for (int k = 0; k < 5; k++) {
        float x = std::fmod(k * 120.f + t * 7.f, W + 200.f) - 100.f;
        seg(s, x, gy + 8.f, x + 90.f, gy + 8.f, 16.f, C(1.f, 0.95f, 0.97f, 0.06f));
    }
}

inline void propsAurora(BgState& s) {
    float W = s.W, H = s.H, t = s.t;
    float fx = W * 0.82f;
    float fy = H * 0.11f + 2.f;
    float fl = 0.75f + 0.25f * S(t * 17.f) * S(t * 9.3f);
    glow(s, ccp(fx, fy + 5.f), 50.f, C(1.f, 0.5f, 0.1f, 0.25f * fl), 4);
    tri(s, ccp(fx - 48.f, fy), ccp(fx - 18.f, fy), ccp(fx - 33.f, fy + 22.f), C(0.08f, 0.1f, 0.14f, 1.f));
    tri(s, ccp(fx - 33.f, fy + 22.f), ccp(fx - 18.f, fy), ccp(fx - 26.f, fy), C(0.5f, 0.3f, 0.12f, 0.8f * fl));
    tri(s, ccp(fx - 6.f, fy), ccp(fx + 6.f, fy), ccp(fx + S(t * 8.f) * 2.f, fy + 17.f * fl), C(1.f, 0.45f, 0.1f, 0.95f));
    tri(s, ccp(fx - 3.f, fy), ccp(fx + 3.f, fy), ccp(fx + S(t * 9.f), fy + 9.f * fl), C(1.f, 0.9f, 0.4f, 1.f));
    for (int k = 0; k < 6; k++) {
        float age = std::fmod(t * 0.7f + k / 6.f, 1.f);
        dot(s, fx + S(k * 5.f + t * 3.f) * 8.f * age, fy + 8.f + age * 42.f, 1.2f, C(1.f, 0.7f, 0.2f, 1.f - age));
    }
}

inline void propsStars(BgState& s) {
    float W = s.W, H = s.H, t = s.t;
    float x = std::fmod(t * 18.f, W + 60.f) - 30.f;
    float y = H * 0.86f - x * 0.08f;
    float blink = std::max(0.f, S(t * 3.f));
    dot(s, x, y, 1.6f, C(1.f, 1.f, 1.f, 0.4f + 0.6f * blink));
    float cx = std::fmod(t * 5.f + 200.f, W + 300.f) - 150.f;
    float cy = H * 0.72f - cx * 0.2f;
    glow(s, ccp(cx, cy), 8.f, C(0.8f, 0.95f, 1.f, 0.4f), 3);
    seg(s, cx, cy, cx + 55.f, cy + 11.f, 2.2f, C(0.7f, 0.9f, 1.f, 0.18f));
    seg(s, cx, cy, cx + 90.f, cy + 18.f, 1.2f, C(0.7f, 0.9f, 1.f, 0.10f));
}

inline void propsRain(BgState& s) {
    float W = s.W, t = s.t;
    float lx = W * 0.12f;
    float gy = 14.f;
    float fl = 0.9f + 0.1f * S(t * 30.f) * S(t * 7.f);
    ccColor4F post = C(0.05f, 0.06f, 0.09f, 1.f);
    seg(s, lx, gy, lx, gy + 62.f, 3.f, post);
    seg(s, lx, gy + 62.f, lx + 12.f, gy + 66.f, 2.5f, post);
    tri(s, ccp(lx + 14.f, gy + 62.f), ccp(lx - 20.f, gy), ccp(lx + 48.f, gy), C(1.f, 0.92f, 0.65f, 0.07f * fl));
    glow(s, ccp(lx + 14.f, gy + 62.f), 44.f, C(1.f, 0.9f, 0.6f, 0.28f * fl), 5);
    dot(s, lx + 14.f, gy + 63.f, 3.2f, C(1.f, 0.97f, 0.8f, 1.f));
    seg(s, lx - 10.f, gy - 3.f, lx + 38.f, gy - 3.f, 3.f, C(1.f, 0.9f, 0.6f, 0.25f * fl));
    for (int i = 0; i < 26; i++) {
        float x = lx - 14.f + std::fmod(i * 13.7f, 56.f);
        float y = gy + 66.f - std::fmod(t * 380.f + i * 27.3f, 66.f);
        segA(s, x, y, x + 1.f, y + 7.f, 0.8f, C(1.f, 0.95f, 0.75f, 0.35f * fl));
    }
}

inline void drawProps(BgState& s, float wind) {
    switch (s.theme) {
        case 1: propsFire(s); break;
        case 2: propsWinter(s, wind); break;
        case 3: propsSpring(s); break;
        case 4: propsSummer(s); break;
        case 5: propsBlossom(s); break;
        case 6: propsAurora(s); break;
        case 7: propsStars(s); break;
        case 8: propsRain(s); break;
        default: break;
    }
}

}
