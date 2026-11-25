#pragma once
#include "BgCore.hpp"

namespace vanta {

// tapered branch with a little natural wobble
inline void taper(BgState& s, float x0, float y0, float x1, float y1, float w0, float w1, ccColor4F c) {
    const int N = 6;
    float px = x0;
    float py = y0;
    for (int i = 1; i <= N; i++) {
        float u = i / (float)N;
        float x = x0 + (x1 - x0) * u + S(u * 3.f + x0 * 0.01f) * 2.5f * (1.f - u);
        float y = y0 + (y1 - y0) * u;
        seg(s, px, py, x, y, w0 + (w1 - w0) * u, c);
        px = x;
        py = y;
    }
}

// one cluster of blossoms
inline void bloom(BgState& s, float cx, float cy, float r, int seed) {
    const int n = 16;
    for (int j = 0; j < n; j++) {
        float a = j * 2.39996f + seed;
        float rr = r * std::sqrt((j + 0.5f) / n);
        float x = cx + std::cos(a) * rr;
        float y = cy + std::sin(a) * rr * 0.85f;
        float v = 0.5f + 0.5f * S(seed * 1.7f + j * 2.3f);
        dot(s, x, y, 3.2f + 1.4f * v, C(1.f, 0.66f + 0.2f * v, 0.78f + 0.14f * v, 0.93f));
    }
    dotA(s, cx, cy, r * 0.9f, C(1.f, 0.8f, 0.88f, 0.05f));
}

// dir = -1 leans left (tree on the right side), +1 leans right
inline void sakuraTree(BgState& s, float bx, float scale, float dir, int seed, float wind) {
    float H = s.H, t = s.t;
    ccColor4F wood = C(0.30f, 0.19f, 0.17f, 1.f);
    ccColor4F woodHi = C(0.42f, 0.28f, 0.24f, 1.f);
    float trunkTop = H * 0.42f * scale;
    float tx = bx + dir * 14.f * scale;

    for (int j = -2; j <= 2; j++) {
        seg(s, bx - 70.f * scale, 6.f + j * 2.5f, bx + 70.f * scale, 6.f + j * 2.5f, 3.f, C(0.f, 0.1f, 0.f, 0.06f));
    }

    taper(s, bx, -4.f, tx, trunkTop, 9.f * scale, 5.f * scale, wood);
    taper(s, bx + 1.5f, -4.f, tx + 1.5f, trunkTop, 2.f, 1.2f, woodHi);

    CCPoint tip[5] = {
        ccp(tx + dir * 95.f * scale, trunkTop + 52.f * scale),
        ccp(tx + dir * 40.f * scale, trunkTop + 88.f * scale),
        ccp(tx - dir * 50.f * scale, trunkTop + 62.f * scale),
        ccp(tx + dir * 130.f * scale, trunkTop + 20.f * scale),
        ccp(tx - dir * 20.f * scale, trunkTop + 110.f * scale)
    };

    for (int k = 0; k < 5; k++) {
        float sy = trunkTop * (0.75f + 0.05f * k);
        taper(s, tx, sy, tip[k].x, tip[k].y, 4.2f * scale, 1.2f, wood);
    }

    for (int k = 0; k < 5; k++) {
        float sy = trunkTop * (0.75f + 0.05f * k);
        for (int m = 0; m < 4; m++) {
            float u = 0.45f + m * 0.18f;
            float cx = tx + (tip[k].x - tx) * u;
            float cy = sy + (tip[k].y - sy) * u;
            float sw = S(t * 0.9f + k + m + seed) * 2.2f + wind * 0.05f;
            float r = 15.f * scale * (0.75f + 0.25f * S(k * 2.f + m + seed));
            bloom(s, cx + sw, cy + S(t * 0.7f + m + k) * 1.2f, r, seed + k * 7 + m);
        }
    }
}

inline void themeBlossomX(BgState& s, float dt, float wind) {
    float W = s.W, H = s.H, t = s.t;

    for (int i = 0; i < 5; i++) {
        float x0 = W * (0.1f + 0.2f * i) + S(t * 0.2f + i) * 15.f;
        seg(s, x0, H, x0 - 100.f, 0.f, 15.f, C(1.f, 0.95f, 0.85f, 0.05f));
    }

    hills(s, 52.f, 18.f, 0.007f, t * 0.03f, C(1.f, 0.88f, 0.92f, 1.f));
    hills(s, 32.f, 12.f, 0.011f, 2.f + t * 0.05f, C(0.97f, 0.8f, 0.87f, 1.f));
    hills(s, 22.f, 7.f, 0.015f, 1.f + t * 0.04f, C(0.62f, 0.8f, 0.5f, 1.f));

    // petal carpet on the grass
    float pcx = W * 0.42f;
    float pcy = 15.f;
    float prx = W * 0.2f;
    for (int i = 0; i < 70; i++) {
        float x = std::fmod(i * 41.3f, W);
        float y = 3.f + std::fmod(i * 7.9f, 22.f);
        if (std::fabs(x - pcx) < prx * 0.95f && y > 6.f && y < 24.f) continue;
        dot(s, x, y, 1.8f + (i % 3) * 0.5f, C(1.f, 0.78f, 0.86f, 0.9f));
    }

    // pond
    for (int yy = 5; yy <= 25; yy += 2) {
        float q = (yy - pcy) / 10.5f;
        float half = prx * std::sqrt(std::max(0.f, 1.f - q * q));
        float g = (yy - 5) / 20.f;
        seg(s, pcx - half, (float)yy, pcx + half, (float)yy, 2.6f, C(0.62f + 0.2f * g, 0.78f + 0.1f * g, 0.88f, 1.f));
    }
    for (int k = 0; k < 24; k++) {
        float a = k * 0.2618f;
        dot(s, pcx + std::cos(a) * prx * 1.02f, pcy + std::sin(a) * 10.8f, 2.2f, C(0.5f, 0.5f, 0.52f, 1.f));
    }
    for (int i = 0; i < 6; i++) {
        dot(s, pcx + (i - 2.5f) * 40.f, pcy + 2.f, 8.f, C(1.f, 0.78f, 0.86f, 0.25f));
    }
    for (int i = 0; i < 3; i++) {
        float age = std::fmod(t * 0.5f + i * 0.33f, 1.f);
        float rx = pcx + prx * 0.6f * S(i * 2.7f + 1.f);
        ellipse(s, rx, pcy + 2.f * S(i * 1.9f), 3.f + 16.f * age, 1.f + 4.f * age, 0.8f, C(1.f, 1.f, 1.f, 0.4f * (1.f - age)));
    }
    for (int i = 0; i < 14; i++) {
        float x = pcx + prx * 0.9f * S(t * 0.06f + i * 1.7f);
        float y = pcy + 7.f * S(t * 0.05f + i * 2.9f);
        dot(s, x, y, 1.5f, C(1.f, 0.78f, 0.86f, 0.95f));
    }
    for (int k = 0; k < 2; k++) {
        float ph = t * 0.25f + k * 3.1f;
        float x = pcx + prx * 0.7f * S(ph);
        float y = pcy + 4.f * S(ph * 1.7f + k);
        float dir = (std::cos(ph) >= 0.f) ? 1.f : -1.f;
        ccColor4F fc = (k == 0) ? C(1.f, 0.5f, 0.15f, 1.f) : C(1.f, 0.95f, 0.9f, 1.f);
        dot(s, x, y, 2.4f, fc);
        dot(s, x - dir * 3.5f, y, 2.f, fc);
        dot(s, x - dir * 6.5f, y, 1.4f, fc);
        float tw = S(t * 8.f + k) * 1.5f;
        tri(s, ccp(x - dir * 7.f, y), ccp(x - dir * 11.f, y + 2.5f + tw), ccp(x - dir * 11.f, y - 2.5f + tw), C(fc.r, fc.g, fc.b, 0.8f));
    }

    // stone lantern
    float lx = pcx + prx * 1.1f;
    float lf = 0.85f + 0.15f * S(t * 9.f);
    ccColor4F stone = C(0.5f, 0.5f, 0.52f, 1.f);
    rect(s, lx - 6.f, 6.f, lx + 6.f, 9.f, stone);
    rect(s, lx - 2.5f, 9.f, lx + 2.5f, 22.f, stone);
    rect(s, lx - 7.f, 22.f, lx + 7.f, 27.f, stone);
    rect(s, lx - 3.f, 23.f, lx + 3.f, 26.f, C(1.f, 0.85f, 0.5f, lf));
    tri(s, ccp(lx - 9.f, 27.f), ccp(lx + 9.f, 27.f), ccp(lx, 35.f), C(0.42f, 0.42f, 0.45f, 1.f));
    glow(s, ccp(lx, 24.f), 18.f, C(1.f, 0.8f, 0.4f, 0.12f * lf), 3);

    // the trees
    sakuraTree(s, W * 0.88f, 1.f, -1.f, 3, wind);
    sakuraTree(s, W * 0.08f, 0.7f, 1.f, 11, wind);

    // falling petals
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

    // glowing pollen motes in the sunlight
    for (int i = 0; i < 20; i++) {
        float x = std::fmod(i * 67.3f + t * (6.f + (i % 4) * 2.f), W);
        float y = H * 0.15f + std::fmod(i * 29.1f, H * 0.7f) + 8.f * S(t * 0.7f + i);
        float a = 0.3f + 0.5f * S(t * 1.4f + i);
        dotA(s, x, y, 1.2f, C(1.f, 0.95f, 0.8f, a * 0.6f));
    }
}

}
