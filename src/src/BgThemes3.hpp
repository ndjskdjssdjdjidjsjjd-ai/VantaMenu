#pragma once
#include "BgCore.hpp"

namespace vanta {

inline void themeAurora(BgState& s) {
    auto d = s.draw;
    float W = s.W;
    float H = s.H;
    ccColor4F cols[3] = {
        ccColor4F{0.2f, 1.f, 0.6f, 0.12f},
        ccColor4F{0.2f, 0.7f, 1.f, 0.12f},
        ccColor4F{0.7f, 0.4f, 1.f, 0.12f}
    };
    for (int i = 0; i < 3; i++) {
        for (float x = 0.f; x <= W + 8.f; x += 8.f) {
            float yc = H * (0.62f + 0.04f * i)
                + S(x * 0.012f + s.t * 0.5f + i * 2.f) * 28.f
                + S(x * 0.03f - s.t * 0.8f) * 8.f;
            float h = 38.f + S(x * 0.02f + s.t + i) * 12.f;
            d->drawSegment(ccp(x, yc - h), ccp(x, yc + h), 4.5f, cols[i]);
            d->drawSegment(ccp(x, yc - h * 0.3f), ccp(x, yc + h * 0.3f), 4.5f,
                ccColor4F{cols[i].r, cols[i].g, cols[i].b, 0.14f});
        }
    }
    stars(s);
    for (float x = 0.f; x <= W + 20.f; x += 20.f) {
        float mh = 30.f + 25.f * S(x * 0.01f) + 15.f * S(x * 0.027f + 2.f);
        d->drawSegment(ccp(x, 0.f), ccp(x, mh), 10.f, ccColor4F{0.02f, 0.05f, 0.08f, 1.f});
    }
}

inline void themeStars(BgState& s, float dt) {
    auto d = s.draw;
    float W = s.W;
    float H = s.H;
    glow(s, ccp(W * 0.25f, H * 0.55f), 120.f, ccColor4F{0.5f, 0.2f, 0.9f, 0.08f}, 4);
    glow(s, ccp(W * 0.75f, H * 0.4f), 140.f, ccColor4F{0.2f, 0.4f, 0.9f, 0.07f}, 4);
    for (int i = 0; i < 60; i++) {
        float u = i / 60.f;
        float px = W * u;
        float py = H * (0.25f + 0.5f * u) + S(i * 12.9f) * 22.f;
        d->drawDot(ccp(px, py), 1.2f, ccColor4F{0.85f, 0.85f, 1.f, 0.25f});
    }
    stars(s);

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
        for (int k = 0; k < 4; k++) {
            float off = k * 18.f;
            d->drawSegment(ccp(s.ssX + off, s.ssY + off * 0.42f),
                ccp(s.ssX + off + 18.f, s.ssY + (off + 18.f) * 0.42f),
                1.3f - k * 0.25f, ccColor4F{1.f, 1.f, 1.f, 0.7f - k * 0.16f});
        }
        d->drawDot(ccp(s.ssX, s.ssY), 2.2f, ccColor4F{1.f, 1.f, 1.f, 1.f});
        if (s.ssX < -90.f || s.ssY < -40.f) s.ssOn = false;
    }
}

inline void themeRain(BgState& s, float dt) {
    auto d = s.draw;
    float W = s.W;
    float H = s.H;

    for (int i = 0; i < 6; i++) {
        d->drawDot(ccp(W * i / 5.f, H + 8.f), 75.f, ccColor4F{0.05f, 0.06f, 0.1f, 0.35f});
    }

    s.nextFlash -= dt;
    if (s.nextFlash <= 0.f) {
        s.flash = 1.f;
        makeBolt(s);
        s.nextFlash = R(4.f, 9.f);
    }
    if (s.flash > 0.f) {
        CCPoint q[4] = {ccp(0.f, 0.f), ccp(W, 0.f), ccp(W, H), ccp(0.f, H)};
        d->drawPolygon(q, 4, ccColor4F{0.85f, 0.9f, 1.f, s.flash * 0.3f},
            0.f, ccColor4F{0.f, 0.f, 0.f, 0.f});
        for (size_t i = 1; i < s.bolt.size(); i++) {
            d->drawSegment(s.bolt[i - 1], s.bolt[i], 3.f, ccColor4F{0.7f, 0.8f, 1.f, s.flash * 0.25f});
            d->drawSegment(s.bolt[i - 1], s.bolt[i], 1.2f, ccColor4F{1.f, 1.f, 1.f, s.flash});
        }
        s.flash -= dt * 2.8f;
    }

    for (auto& p : s.p) {
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        if (p.y < 8.f) {
            if (s.fx.size() < 120) {
                Particle sp;
                sp.x = p.x;
                sp.y = 8.f;
                s.fx.push_back(sp);
            }
            spawn(s, p, false);
            continue;
        }
        d->drawSegment(ccp(p.x, p.y),
            ccp(p.x - p.vx * 0.025f, p.y - p.vy * 0.025f),
            0.5f + 0.4f * p.z, ccColor4F{0.75f, 0.85f, 1.f, 0.25f + 0.35f * p.z});
    }

    for (size_t i = 0; i < s.fx.size();) {
        auto& sp = s.fx[i];
        sp.age += dt;
        if (sp.age > 0.25f) {
            s.fx.erase(s.fx.begin() + i);
            continue;
        }
        float a = 1.f - sp.age / 0.25f;
        for (int k = -1; k <= 1; k++) {
            float px = sp.x + k * sp.age * 40.f;
            float py = 8.f + 30.f * sp.age - 140.f * sp.age * sp.age;
            if (py > 6.f) {
                d->drawDot(ccp(px, py), 1.f, ccColor4F{0.8f, 0.9f, 1.f, 0.6f * a});
            }
        }
        i++;
    }

    d->drawSegment(ccp(0.f, 8.f), ccp(W, 8.f), 14.f, ccColor4F{0.7f, 0.75f, 0.85f, 0.08f});
}

}
