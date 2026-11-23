#pragma once
#include "BgCore.hpp"

namespace vanta {

inline void themeFire(BgState& s, float dt, float wind) {
    auto d = s.draw;
    float W = s.W;
    for (int i = 0; i < 9; i++) {
        float gx = W * i / 8.f;
        float r = 110.f + S(s.t * 2.2f + i * 1.7f) * 22.f;
        glow(s, ccp(gx, -30.f), r, ccColor4F{1.f, 0.4f, 0.05f, 0.22f}, 5);
    }
    for (float x = -6.f; x <= W + 6.f; x += 9.f) {
        float n = S(x * 0.045f + s.t * 4.2f) * 0.5f
                + S(x * 0.13f - s.t * 6.1f) * 0.3f
                + S(x * 0.31f + s.t * 9.f) * 0.2f;
        float h = 70.f + 55.f * n + 25.f * S(x * 0.01f + s.t * 0.7f);
        float sway = S(x * 0.05f + s.t * 3.f) * 8.f;
        tri(s, ccp(x - 10.f, 0.f), ccp(x + 10.f, 0.f), ccp(x + sway, h),
            ccColor4F{0.85f, 0.12f, 0.02f, 0.55f});
        tri(s, ccp(x - 8.f, 0.f), ccp(x + 8.f, 0.f), ccp(x + sway * 0.8f, h * 0.72f),
            ccColor4F{1.f, 0.45f, 0.05f, 0.65f});
        tri(s, ccp(x - 5.f, 0.f), ccp(x + 5.f, 0.f), ccp(x + sway * 0.5f, h * 0.42f),
            ccColor4F{1.f, 0.85f, 0.3f, 0.8f});
    }
    for (auto& p : s.p) {
        p.age += dt;
        if (p.age >= p.maxLife) {
            spawn(s, p, false);
            continue;
        }
        float t = p.age / p.maxLife;
        if (p.kind == 1) {
            p.x += (p.vx + wind * 0.5f + S(s.t + p.phase) * 10.f) * dt;
            p.y += p.vy * dt;
            d->drawDot(ccp(p.x, p.y), p.size * (0.6f + t),
                ccColor4F{0.12f, 0.1f, 0.1f, 0.16f * (1.f - t)});
        } else {
            p.x += (p.vx + S(s.t * 3.f + p.phase) * 22.f + wind) * dt;
            p.y += p.vy * dt;
            float fl = 0.7f + 0.3f * S(s.t * 20.f + p.phase * 5.f);
            glow(s, ccp(p.x, p.y), p.size * 2.6f * (1.f - 0.6f * t),
                ccColor4F{1.f, 0.5f, 0.1f, 0.35f * (1.f - t)}, 2);
            d->drawDot(ccp(p.x, p.y), p.size * (1.f - 0.6f * t),
                ccColor4F{1.f, 0.95f - 0.6f * t, 0.35f * (1.f - t), fl * (1.f - t)});
        }
    }
}

inline void themeWinter(BgState& s, float dt, float wind) {
    auto d = s.draw;
    float W = s.W;
    float H = s.H;
    for (int i = 0; i < 45; i++) {
        float sx = std::fmod(i * 97.3f, W);
        float sy = H * 0.45f + std::fmod(i * 53.7f, H * 0.55f);
        float a = 0.3f + 0.5f * (0.5f + 0.5f * S(s.t * 1.8f + i));
        d->drawDot(ccp(sx, sy), 0.9f, ccColor4F{1.f, 1.f, 1.f, a});
    }
    glow(s, ccp(W * 0.8f, H * 0.78f), 75.f, ccColor4F{0.8f, 0.9f, 1.f, 0.2f}, 6);
    d->drawDot(ccp(W * 0.8f, H * 0.78f), 24.f, ccColor4F{0.95f, 0.97f, 1.f, 1.f});

    for (int i = 0; i < 9; i++) {
        float tx = W * (i + 0.5f) / 9.f + S(i * 3.1f) * 14.f;
        float th = 55.f + 28.f * (0.5f + 0.5f * S(i * 2.3f));
        float sw = S(s.t * 0.8f + i) * 1.5f;
        d->drawSegment(ccp(tx, 4.f), ccp(tx, 18.f), 2.5f, ccColor4F{0.25f, 0.15f, 0.1f, 1.f});
        for (int k = 0; k < 3; k++) {
            float by = 14.f + th * 0.28f * k;
            float hw = th * (0.36f - 0.07f * k);
            float ht = th * 0.45f;
            tri(s, ccp(tx - hw, by), ccp(tx + hw, by), ccp(tx + sw, by + ht),
                ccColor4F{0.07f, 0.16f, 0.2f, 1.f});
            tri(s, ccp(tx - hw * 0.55f, by + ht * 0.55f), ccp(tx + hw * 0.55f, by + ht * 0.55f),
                ccp(tx + sw, by + ht), ccColor4F{0.9f, 0.95f, 1.f, 0.95f});
        }
    }
    d->drawSegment(ccp(0.f, 4.f), ccp(W, 4.f), 11.f, ccColor4F{0.92f, 0.96f, 1.f, 1.f});

    for (auto& p : s.p) {
        p.y += p.vy * dt;
        p.x += (p.vx + wind * p.z + S(s.t * 1.5f + p.phase) * 12.f * p.z) * dt;
        if (p.y < -5.f) {
            spawn(s, p, false);
            continue;
        }
        if (p.x < -5.f) p.x += W + 10.f;
        if (p.x > W + 5.f) p.x -= W + 10.f;
        if (p.z > 0.8f) {
            glow(s, ccp(p.x, p.y), p.size * 2.2f, ccColor4F{1.f, 1.f, 1.f, 0.18f}, 2);
        }
        d->drawDot(ccp(p.x, p.y), p.size, ccColor4F{1.f, 1.f, 1.f, 0.55f + 0.4f * p.z});
    }
}

inline void themeSpring(BgState& s, float dt, float wind) {
    auto d = s.draw;
    float W = s.W;
    float H = s.H;
    glow(s, ccp(W * 0.15f, H * 0.85f), 90.f, ccColor4F{1.f, 0.98f, 0.75f, 0.25f}, 5);
    d->drawDot(ccp(W * 0.15f, H * 0.85f), 22.f, ccColor4F{1.f, 0.99f, 0.85f, 1.f});

    for (int i = 0; i < 4; i++) {
        float cx = std::fmod(i * 180.f + s.t * (6.f + i * 2.f), W + 240.f) - 120.f;
        cloud(s, cx, H * (0.7f + 0.08f * S(i * 2.f)), 46.f + 10.f * i, 0.85f);
    }

    hills(s, 62.f, 18.f, 0.008f, s.t * 0.1f, ccColor4F{0.55f, 0.78f, 0.5f, 1.f});
    hills(s, 40.f, 14.f, 0.012f, 1.f + s.t * 0.15f, ccColor4F{0.42f, 0.72f, 0.34f, 1.f});

    for (int i = 0; i < 15; i++) {
        float fx = W * (i + 0.5f) / 15.f;
        float fy = 40.f + 14.f * S(fx * 0.012f + 1.f + s.t * 0.15f);
        float sw = S(s.t * 1.4f + i) * 2.f;
        d->drawSegment(ccp(fx, fy - 8.f), ccp(fx + sw, fy + 4.f), 0.9f, ccColor4F{0.2f, 0.5f, 0.2f, 1.f});
        ccColor4F fc{0.7f, 0.5f, 1.f, 1.f};
        if (i % 4 == 0) fc = ccColor4F{1.f, 0.5f, 0.6f, 1.f};
        if (i % 4 == 1) fc = ccColor4F{1.f, 1.f, 1.f, 1.f};
        if (i % 4 == 2) fc = ccColor4F{1.f, 0.9f, 0.3f, 1.f};
        d->drawDot(ccp(fx + sw, fy + 5.f), 3.2f, fc);
        d->drawDot(ccp(fx + sw, fy + 5.f), 1.2f, ccColor4F{1.f, 0.9f, 0.3f, 1.f});
    }

    for (float x = 0.f; x <= W; x += 7.f) {
        float gh = 14.f + 8.f * S(x * 0.2f) + 5.f * S(x * 0.07f);
        float sw = S(s.t * 1.6f + x * 0.05f) * 4.f + wind * 0.1f;
        d->drawSegment(ccp(x, 0.f), ccp(x + sw, gh), 1.1f, ccColor4F{0.25f, 0.6f, 0.22f, 1.f});
    }

    for (int b = 0; b < 3; b++) {
        float bx = W * (0.3f + 0.25f * b) + S(s.t * 0.6f + b * 2.f) * 90.f;
        float by = H * 0.35f + S(s.t * 0.9f + b * 1.3f) * 30.f + b * 20.f;
        float flap = std::fabs(S(s.t * 14.f + b)) * 5.f + 1.f;
        ccColor4F wc{0.5f, 0.7f, 1.f, 0.95f};
        if (b % 2 == 0) wc = ccColor4F{1.f, 0.6f, 0.2f, 0.95f};
        d->drawDot(ccp(bx - flap, by), 3.f, wc);
        d->drawDot(ccp(bx + flap, by), 3.f, wc);
        d->drawDot(ccp(bx, by), 1.2f, ccColor4F{0.15f, 0.1f, 0.1f, 1.f});
    }

    for (auto& p : s.p) {
        p.y += p.vy * dt;
        p.x += (p.vx + wind * 0.5f + S(s.t * 1.2f + p.phase) * 12.f) * dt;
        if (p.y < -5.f || p.x > W + 5.f) {
            spawn(s, p, false);
            continue;
        }
        ccColor4F c{1.f, 0.92f, 0.4f, 0.9f};
        if (p.kind == 0) c = ccColor4F{1.f, 0.7f, 0.82f, 0.95f};
        if (p.kind == 1) c = ccColor4F{1.f, 1.f, 1.f, 0.9f};
        d->drawDot(ccp(p.x, p.y), p.size, c);
    }
}

}
