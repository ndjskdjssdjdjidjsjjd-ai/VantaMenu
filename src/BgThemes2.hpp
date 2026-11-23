#pragma once
#include "BgCore.hpp"

namespace vanta {

inline void themeSpring(BgState& s, float dt, float wind) {
    float W = s.W, H = s.H, t = s.t;

    glow(s, ccp(W * 0.15f, H * 0.85f), 95.f, C(1.f, 0.98f, 0.75f, 0.28f), 5);
    dot(s, W * 0.15f, H * 0.85f, 22.f, C(1.f, 0.99f, 0.88f, 1.f));
    for (int i = 0; i < 4; i++) {
        float x0 = W * (0.05f + 0.12f * i) + S(t * 0.2f + i) * 10.f;
        seg(s, x0, H, x0 + 130.f, 0.f, 16.f, C(1.f, 1.f, 0.85f, 0.04f));
    }
    for (int i = 0; i < 5; i++) {
        float cx = std::fmod(i * 170.f + t * (4.f + i * 2.f), W + 240.f) - 120.f;
        cloud(s, cx, H * (0.66f + 0.07f * S(i * 2.f)), 40.f + 9.f * i, 0.8f);
    }

    hills(s, 78.f, 18.f, 0.006f, t * 0.04f, C(0.62f, 0.82f, 0.74f, 1.f));
    hills(s, 56.f, 16.f, 0.01f, 1.f + t * 0.08f, C(0.5f, 0.76f, 0.5f, 1.f));

    for (int i = 0; i < 6; i++) {
        float tx = W * (i + 0.5f) / 6.f + S(i * 4.3f) * 20.f;
        float ty = hillY(56.f, 16.f, 0.01f, 1.f + t * 0.08f, tx);
        float sw = S(t * 1.1f + i) * 1.5f + wind * 0.05f;
        seg(s, tx, ty, tx + sw * 0.4f, ty + 18.f, 3.f, C(0.36f, 0.24f, 0.16f, 1.f));
        dot(s, tx + sw, ty + 26.f, 15.f, C(0.2f, 0.55f, 0.24f, 1.f));
        dot(s, tx - 9.f + sw, ty + 20.f, 10.f, C(0.26f, 0.62f, 0.28f, 1.f));
        dot(s, tx + 9.f + sw, ty + 21.f, 10.f, C(0.24f, 0.6f, 0.27f, 1.f));
        if (i % 2 == 0) {
            dot(s, tx + 4.f + sw, ty + 30.f, 3.f, C(1.f, 0.75f, 0.85f, 1.f));
        }
    }

    hills(s, 34.f, 12.f, 0.013f, 2.f + t * 0.12f, C(0.4f, 0.7f, 0.3f, 1.f));

    for (float x = 0.f; x <= W; x += 6.f) {
        float gy = hillY(34.f, 12.f, 0.013f, 2.f + t * 0.12f, x);
        float gh = 12.f + 7.f * S(x * 0.2f) + 4.f * S(x * 0.07f);
        float sw = S(t * 1.8f + x * 0.04f) * 3.5f + wind * 0.12f;
        seg(s, x, gy - 2.f, x + sw, gy + gh, 1.2f, C(0.22f + 0.08f * S(x), 0.58f, 0.2f, 1.f));
    }

    for (int i = 0; i < 14; i++) {
        float fx = W * (i + 0.5f) / 14.f + S(i * 7.1f) * 10.f;
        float fy = hillY(34.f, 12.f, 0.013f, 2.f + t * 0.12f, fx);
        float sw = S(t * 1.8f + fx * 0.04f) * 3.f + wind * 0.1f;
        seg(s, fx, fy - 2.f, fx + sw, fy + 11.f, 0.9f, C(0.2f, 0.5f, 0.2f, 1.f));
        ccColor4F fc = C(0.7f, 0.5f, 1.f, 1.f);
        if (i % 4 == 0) fc = C(1.f, 0.5f, 0.6f, 1.f);
        if (i % 4 == 1) fc = C(1.f, 1.f, 1.f, 1.f);
        if (i % 4 == 2) fc = C(1.f, 0.9f, 0.3f, 1.f);
        for (int j = 0; j < 5; j++) {
            float a = j * 1.2566f + t * 0.3f;
            dot(s, fx + sw + std::cos(a) * 2.8f, fy + 12.f + std::sin(a) * 2.8f, 2.1f, fc);
        }
        dot(s, fx + sw, fy + 12.f, 1.3f, C(1.f, 0.9f, 0.3f, 1.f));
    }

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

    for (int b = 0; b < 4; b++) {
        float bx = std::fmod(t * (26.f + b * 4.f) + b * 170.f, W + 80.f) - 40.f;
        float by = H * 0.62f + 20.f * S(t * 0.7f + b * 1.7f) + b * 14.f;
        float fl = S(t * 9.f + b) * 5.f;
        seg(s, bx - 9.f, by + fl, bx, by, 1.f, C(0.15f, 0.18f, 0.22f, 0.85f));
        seg(s, bx, by, bx + 9.f, by + fl, 1.f, C(0.15f, 0.18f, 0.22f, 0.85f));
    }

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

inline void themeSummer(BgState& s, float dt, float wind) {
    float W = s.W, H = s.H, t = s.t;
    float sx = W * 0.8f;
    float sy = H * 0.78f;
    float hy = H * 0.34f;
    float pulse = 1.f + 0.03f * S(t * 1.4f);

    glow(s, ccp(sx, sy), 130.f * pulse, C(1.f, 0.95f, 0.55f, 0.25f), 6);
    for (int i = 0; i < 16; i++) {
        float a = t * 0.25f + i * 0.3927f;
        float r2 = 82.f + S(t * 2.f + i * 1.3f) * 10.f;
        seg(s, sx + std::cos(a) * 44.f, sy + std::sin(a) * 44.f,
            sx + std::cos(a) * r2, sy + std::sin(a) * r2, 1.5f, C(1.f, 0.95f, 0.5f, 0.3f));
    }
    dot(s, sx, sy, 34.f, C(1.f, 0.88f, 0.25f, 1.f));
    dot(s, sx, sy, 26.f, C(1.f, 0.97f, 0.72f, 1.f));
    for (int i = 1; i <= 3; i++) {
        float fx = sx + (W * 0.4f - sx) * i * 0.45f;
        float fy = sy + (H * 0.3f - sy) * i * 0.45f;
        dot(s, fx, fy, 10.f + 6.f * i, C(1.f, 0.9f, 0.6f, 0.05f));
    }

    for (int i = 0; i < 4; i++) {
        float cx = std::fmod(i * 210.f + t * (7.f + i * 3.f), W + 260.f) - 130.f;
        cloud(s, cx, H * (0.6f + 0.1f * S(i * 1.7f)), 50.f + 11.f * i, 0.9f);
    }

    for (int b = 0; b < 3; b++) {
        float bx = std::fmod(t * (34.f + b * 6.f) + b * 220.f, W + 80.f) - 40.f;
        float by = H * 0.52f + b * 18.f + S(t * 0.9f + b) * 7.f;
        float fl = S(t * 7.f + b * 2.f) * 5.f;
        seg(s, bx - 10.f, by + fl, bx, by, 1.1f, C(0.12f, 0.16f, 0.24f, 0.9f));
        seg(s, bx, by, bx + 10.f, by + fl, 1.1f, C(0.12f, 0.16f, 0.24f, 0.9f));
    }

    for (int b = 0; b < 6; b++) {
        float k = b / 5.f;
        rect(s, 0.f, hy * (1.f - (b + 1) / 6.f), W, hy * (1.f - b / 6.f),
            C(0.35f - 0.3f * k, 0.78f - 0.43f * k, 0.95f - 0.25f * k, 1.f));
    }

    for (int i = 0; i < 36; i++) {
        float k = std::fmod(i * 0.137f + t * 0.05f, 1.f);
        float gy = hy * (1.f - k);
        float gx = sx + S(i * 12.3f + t * 0.6f) * (20.f + 70.f * k);
        float a = std::max(0.f, S(t * 5.f + i * 3.7f));
        dot(s, gx, gy, 1.2f + 1.3f * k, C(1.f, 0.97f, 0.8f, 0.8f * a));
    }

    for (int l = 0; l < 4; l++) {
        float y0 = hy * (1.f - (l + 1) / 5.f);
        float sp = 1.f + l * 0.4f;
        for (float x = 0.f; x < W; x += 14.f) {
            float y1 = y0 + (2.f + l) * S(x * 0.03f + t * sp + l * 2.f);
            float y2 = y0 + (2.f + l) * S((x + 14.f) * 0.03f + t * sp + l * 2.f);
            seg(s, x, y1, x + 14.f, y2, 1.1f + 0.3f * l, C(1.f, 1.f, 1.f, 0.16f));
        }
    }

    rect(s, 0.f, 0.f, W, 14.f, C(0.95f, 0.88f, 0.66f, 1.f));
    for (float x = 0.f; x < W; x += 10.f) {
        float fy = 15.f + 3.f * S(x * 0.05f + t * 1.6f);
        seg(s, x, fy, x + 10.f, fy, 3.f, C(1.f, 1.f, 1.f, 0.55f));
    }

    float px = 34.f;
    float py = 12.f;
    float sway = S(t * 0.9f) * 3.f + wind * 0.1f;
    float tx = px;
    float ty = py;
    for (int k = 0; k < 8; k++) {
        float u = (k + 1) / 8.f;
        float nx = px + 14.f * u * u + sway * u;
        float ny = py + 78.f * u;
        seg(s, tx, ty, nx, ny, 4.5f - 2.f * u, C(0.36f, 0.24f, 0.14f, 1.f));
        tx = nx;
        ty = ny;
    }
    for (int j = 0; j < 7; j++) {
        float a = 0.2f + j * 0.45f;
        float fx = tx;
        float fy = ty;
        for (int m = 1; m <= 5; m++) {
            float u = m / 5.f;
            float nx = tx + std::cos(a) * 34.f * u + sway * u;
            float ny = ty + std::sin(a) * 34.f * u - 18.f * u * u;
            seg(s, fx, fy, nx, ny, 2.4f - 1.2f * u, C(0.12f, 0.5f, 0.2f, 1.f));
            fx = nx;
            fy = ny;
        }
    }

    for (auto& p : s.p) {
        p.x += (p.vx + S(t + p.phase) * 6.f) * dt;
        p.y += (p.vy + std::cos(t * 0.8f + p.phase) * 5.f) * dt;
        if (p.x < 0.f) p.x += W;
        if (p.x > W) p.x -= W;
        if (p.y > H) p.y = H * 0.3f;
        if (p.y < H * 0.3f) p.y = H;
        float a = 0.3f + 0.5f * (0.5f + 0.5f * S(t * 3.f + p.phase));
        dot(s, p.x, p.y, p.size, C(1.f, 1.f, 0.85f, a));
    }
}

}
