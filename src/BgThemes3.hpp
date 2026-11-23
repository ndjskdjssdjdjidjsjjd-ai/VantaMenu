#pragma once
#include "BgCore.hpp"

namespace vanta {

inline void themeBlossom(BgState& s, float dt, float wind) {
    float W = s.W, H = s.H, t = s.t;

    for (int i = 0; i < 5; i++) {
        float x0 = W * (0.1f + 0.2f * i) + S(t * 0.2f + i) * 15.f;
        seg(s, x0, H, x0 - 100.f, 0.f, 15.f, C(1.f, 0.95f, 0.85f, 0.05f));
    }

    hills(s, 52.f, 18.f, 0.007f, t * 0.03f, C(1.f, 0.88f, 0.92f, 1.f));
    hills(s, 32.f, 12.f, 0.011f, 2.f + t * 0.05f, C(0.97f, 0.8f, 0.87f, 1.f));

    ccColor4F wood = C(0.33f, 0.2f, 0.18f, 1.f);
    seg(s, W + 10.f, H - 30.f, W * 0.78f, H - 55.f, 4.f, wood);
    seg(s, W * 0.78f, H - 55.f, W * 0.62f, H - 50.f, 3.f, wood);
    seg(s, W * 0.78f, H - 55.f, W * 0.7f, H - 90.f, 2.5f, wood);
    seg(s, W + 10.f, H - 70.f, W * 0.88f, H - 85.f, 3.f, wood);
    seg(s, W * 0.62f, H - 50.f, W * 0.54f, H - 66.f, 2.f, wood);

    CCPoint cl[6] = {
        ccp(W * 0.78f, H - 55.f), ccp(W * 0.62f, H - 50.f), ccp(W * 0.7f, H - 90.f),
        ccp(W * 0.88f, H - 85.f), ccp(W * 0.95f, H - 45.f), ccp(W * 0.54f, H - 66.f)
    };
    for (int k = 0; k < 6; k++) {
        float sway = S(t * 0.9f + k) * 1.8f + wind * 0.04f;
        glow(s, ccp(cl[k].x, cl[k].y + sway), 26.f, C(1.f, 0.75f, 0.85f, 0.22f), 3);
        for (int j = 0; j < 10; j++) {
            float a = j * 2.39996f + k;
            float r = 2.f + j * 1.5f;
            dot(s, cl[k].x + std::cos(a) * r, cl[k].y + std::sin(a) * r + sway, 4.6f,
                C(1.f, 0.7f + 0.06f * S(j + k * 2.f), 0.82f, 0.95f));
        }
        dot(s, cl[k].x, cl[k].y + sway, 3.f, C(1.f, 0.9f, 0.94f, 1.f));
    }

    for (float x = 0.f; x <= W; x += 10.f) {
        dot(s, x, 6.f + 3.f * S(x * 0.2f), 4.f, C(1.f, 0.8f, 0.88f, 0.9f));
    }

    for (int i = 0; i < 12; i++) {
        float bx = std::fmod(i * 131.7f + t * 8.f * (1.f + (i % 3) * 0.3f), W);
        float by = H * 0.2f + std::fmod(i * 57.3f, H * 0.7f);
        dot(s, bx, by, 8.f + 4.f * (i % 3), C(1.f, 0.9f, 0.95f, 0.06f));
    }

    for (auto& p : s.p) {
        p.y += p.vy * dt;
        p.x += (p.vx + wind * p.z * 1.6f + S(t * 2.f + p.phase) * 20.f * p.z) * dt;
        p.rot += p.rotV * dt;
        if (p.y < -10.f || p.x < -12.f) {
            spawn(s, p, false);
            continue;
        }
        ccColor4F c = C(1.f, 0.86f, 0.92f, 0.55f + 0.4f * p.z);
        if (p.kind == 0) c = C(1.f, 0.72f, 0.82f, 0.55f + 0.4f * p.z);
        petal(s, p.x, p.y, p.size, p.rot, t * 3.f + p.phase, c);
    }
}

inline void themeAurora(BgState& s) {
    float W = s.W, H = s.H, t = s.t;
    static float const cr[3] = {0.2f, 0.2f, 0.7f};
    static float const cg[3] = {1.f, 0.7f, 0.4f};
    static float const cb[3] = {0.55f, 1.f, 1.f};

    stars(s);

    for (int band = 0; band < 3; band++) {
        for (float x = 0.f; x <= W + 6.f; x += 6.f) {
            float yc = H * (0.64f + 0.05f * band)
                + S(x * 0.011f + t * 0.45f + band * 2.f) * 30.f
                + S(x * 0.031f - t * 0.8f) * 9.f;
            float h = 46.f + S(x * 0.017f + t + band) * 16.f + 10.f * S(x * 0.05f - t * 1.6f);
            float sh = 0.55f + 0.45f * S(x * 0.09f - t * 2.4f + band);
            float step = h * 1.4f / 4.f;
            for (int k = 0; k < 4; k++) {
                float y0 = yc - h + k * step;
                seg(s, x, y0, x, y0 + step, 6.5f, C(cr[band], cg[band], cb[band], (0.03f + 0.05f * k) * sh));
            }
            seg(s, x, yc + h * 0.35f - 2.f, x, yc + h * 0.4f, 6.5f, C(cr[band], cg[band], cb[band], 0.22f * sh));
        }
    }

    float ly = H * 0.11f;
    for (float x = 0.f; x <= W + 18.f; x += 18.f) {
        float mh = 34.f + 26.f * S(x * 0.01f) + 16.f * S(x * 0.027f + 2.f);
        seg(s, x, ly, x, ly + mh, 10.f, C(0.02f, 0.05f, 0.08f, 1.f));
        if (mh > 52.f) {
            seg(s, x, ly + mh - 8.f, x, ly + mh, 10.f, C(0.7f, 0.82f, 0.9f, 0.9f));
        }
    }
    rect(s, 0.f, 0.f, W, ly, C(0.02f, 0.06f, 0.09f, 1.f));
    for (int i = 0; i < 30; i++) {
        float rx = std::fmod(i * 47.3f + t * 6.f, W);
        float ry = std::fmod(i * 13.7f, ly - 2.f) + 1.f;
        float a = 0.05f + 0.08f * (0.5f + 0.5f * S(t * 2.f + i));
        seg(s, rx, ry, rx + 16.f, ry, 1.f, C(0.3f, 1.f, 0.7f, a));
    }
}

}
