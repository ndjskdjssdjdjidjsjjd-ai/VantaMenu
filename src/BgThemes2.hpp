#pragma once
#include "BgCore.hpp"

namespace vanta {

inline void themeSummer(BgState& s, float dt) {
    auto d = s.draw;
    float W = s.W;
    float H = s.H;
    float sx = W * 0.85f;
    float sy = H * 0.8f;

    glow(s, ccp(sx, sy), 110.f + S(s.t * 1.5f) * 6.f, ccColor4F{1.f, 0.95f, 0.5f, 0.22f}, 6);
    for (int i = 0; i < 14; i++) {
        float a = s.t * 0.3f + i * 0.4488f;
        float r2 = 78.f + S(s.t * 2.f + i) * 9.f;
        d->drawSegment(
            ccp(sx + std::cos(a) * 42.f, sy + std::sin(a) * 42.f),
            ccp(sx + std::cos(a) * r2, sy + std::sin(a) * r2),
            1.4f, ccColor4F{1.f, 0.95f, 0.5f, 0.35f});
    }
    d->drawDot(ccp(sx, sy), 34.f, ccColor4F{1.f, 0.9f, 0.3f, 1.f});
    d->drawDot(ccp(sx, sy), 26.f, ccColor4F{1.f, 0.97f, 0.7f, 1.f});

    for (int i = 0; i < 3; i++) {
        float cx = std::fmod(i * 260.f + s.t * (8.f + i * 3.f), W + 260.f) - 130.f;
        cloud(s, cx, H * (0.62f + 0.1f * S(i * 1.7f)), 52.f + 12.f * i, 0.9f);
    }

    for (int b = 0; b < 3; b++) {
        float bx = std::fmod(s.t * 40.f + b * 260.f, W + 80.f) - 40.f;
        float by = H * 0.52f + b * 18.f + S(s.t + b) * 6.f;
        float fl = S(s.t * 8.f + b) * 5.f;
        ccColor4F bc{0.1f, 0.15f, 0.25f, 0.9f};
        d->drawSegment(ccp(bx - 9.f, by + fl), ccp(bx, by), 0.9f, bc);
        d->drawSegment(ccp(bx, by), ccp(bx + 9.f, by + fl), 0.9f, bc);
    }

    for (auto& p : s.p) {
        p.x += (p.vx + S(s.t + p.phase) * 6.f) * dt;
        p.y += (p.vy + std::cos(s.t * 0.8f + p.phase) * 5.f) * dt;
        if (p.x < 0.f) p.x += W;
        if (p.x > W) p.x -= W;
        if (p.y > H) p.y = H * 0.3f;
        if (p.y < H * 0.3f) p.y = H;
        float a = 0.3f + 0.5f * (0.5f + 0.5f * S(s.t * 3.f + p.phase));
        d->drawDot(ccp(p.x, p.y), p.size, ccColor4F{1.f, 1.f, 0.85f, a});
    }

    hills(s, 34.f, 6.f, 0.02f, s.t * 1.2f, ccColor4F{0.3f, 0.7f, 0.98f, 1.f});
    hills(s, 24.f, 6.f, 0.025f, 2.f + s.t * 1.6f, ccColor4F{0.18f, 0.55f, 0.9f, 1.f});
    hills(s, 12.f, 5.f, 0.03f, 4.f + s.t * 2.f, ccColor4F{0.08f, 0.4f, 0.8f, 1.f});

    for (float x = 0.f; x <= W; x += 14.f) {
        float y = 34.f + 6.f * S(x * 0.02f + s.t * 1.2f);
        float a = 0.35f + 0.4f * (0.5f + 0.5f * S(s.t * 3.f + x));
        d->drawDot(ccp(x, y + 5.f), 1.6f, ccColor4F{1.f, 1.f, 1.f, a});
    }
    for (int i = 0; i < 25; i++) {
        float qx = std::fmod(i * 61.7f, W);
        float qy = 4.f + std::fmod(i * 11.3f, 24.f);
        float a = std::max(0.f, S(s.t * 4.f + i * 2.f));
        d->drawDot(ccp(qx, qy), 1.2f, ccColor4F{1.f, 1.f, 0.9f, a * 0.8f});
    }
}

inline void themeBlossom(BgState& s, float dt, float wind) {
    auto d = s.draw;
    float W = s.W;
    float H = s.H;

    for (int i = 0; i < 5; i++) {
        float x0 = W * (0.1f + 0.2f * i) + S(s.t * 0.2f + i) * 15.f;
        d->drawSegment(ccp(x0, H), ccp(x0 - 90.f, 0.f), 14.f, ccColor4F{1.f, 0.95f, 0.85f, 0.05f});
    }

    ccColor4F wood{0.33f, 0.2f, 0.18f, 1.f};
    d->drawSegment(ccp(W + 10.f, H - 30.f), ccp(W * 0.78f, H - 55.f), 4.f, wood);
    d->drawSegment(ccp(W * 0.78f, H - 55.f), ccp(W * 0.62f, H - 50.f), 3.f, wood);
    d->drawSegment(ccp(W * 0.78f, H - 55.f), ccp(W * 0.7f, H - 90.f), 2.5f, wood);
    d->drawSegment(ccp(W + 10.f, H - 70.f), ccp(W * 0.88f, H - 85.f), 3.f, wood);
    d->drawSegment(ccp(W * 0.62f, H - 50.f), ccp(W * 0.54f, H - 66.f), 2.f, wood);

    CCPoint clusters[6] = {
        ccp(W * 0.78f, H - 55.f), ccp(W * 0.62f, H - 50.f), ccp(W * 0.7f, H - 90.f),
        ccp(W * 0.88f, H - 85.f), ccp(W * 0.95f, H - 45.f), ccp(W * 0.54f, H - 66.f)
    };
    for (int k = 0; k < 6; k++) {
        float sway = S(s.t + k) * 1.5f;
        glow(s, ccp(clusters[k].x, clusters[k].y + sway), 22.f, ccColor4F{1.f, 0.75f, 0.85f, 0.2f}, 3);
        for (int j = 0; j < 7; j++) {
            float a = j * 0.9f + k;
            d->drawDot(
                ccp(clusters[k].x + std::cos(a) * 10.f, clusters[k].y + std::sin(a) * 10.f + sway),
                5.f, ccColor4F{1.f, 0.72f, 0.82f, 0.95f});
        }
        d->drawDot(ccp(clusters[k].x, clusters[k].y + sway), 3.f, ccColor4F{1.f, 0.9f, 0.94f, 1.f});
    }

    for (auto& p : s.p) {
        p.y += p.vy * dt;
        p.x += (p.vx + wind * p.z + S(s.t * 2.f + p.phase) * 20.f * p.z) * dt;
        p.rot += p.rotV * dt;
        if (p.y < -10.f || p.x < -12.f) {
            spawn(s, p, false);
            continue;
        }
        ccColor4F c{1.f, 0.86f, 0.92f, 0.55f + 0.4f * p.z};
        if (p.kind == 0) c = ccColor4F{1.f, 0.72f, 0.82f, 0.55f + 0.4f * p.z};
        petal(s, p.x, p.y, p.size, p.rot, s.t * 3.f + p.phase, c);
    }
}

}
