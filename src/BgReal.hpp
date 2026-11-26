#pragma once
#include "BgCore.hpp"
#include "BgRain.hpp"

namespace vanta {

// ---------- helpers ----------

// layered mountain/hill silhouette; cap = snow colour (alpha 0 = none)
inline void ridgeLine(BgState& s, float base, float amp, float seed, float drift, ccColor4F col, ccColor4F cap) {
    for (float x = -10.f; x <= s.W + 10.f; x += 10.f) {
        float n = fbm(x * 0.006f + seed, s.t * drift);
        float y = base + amp * n;
        seg(s, x, 0.f, x, y, 10.5f, col);
        if (cap.a > 0.f && n > 0.2f) {
            seg(s, x, y - 9.f * (n - 0.1f), x, y, 10.5f, cap);
        }
    }
}

// night city with lit windows
inline void skyline(BgState& s) {
    float W = s.W, t = s.t;
    float gy = 14.f;
    for (int layer = 0; layer < 2; layer++) {
        float base = gy + layer * 6.f;
        float shade = (layer == 0) ? 0.03f : 0.06f;
        int count = (layer == 0) ? 13 : 16;
        for (int i = 0; i < count; i++) {
            float bx = i * (W / (count - 2.f)) - 10.f + layer * 14.f;
            float bw = 26.f + std::fmod(i * 37.1f + layer * 11.f, 22.f);
            float bh = 36.f + std::fmod(i * 53.7f + layer * 29.f, 80.f);
            rect(s, bx, base, bx + bw, base + bh, C(shade, shade + 0.01f, shade + 0.03f, 1.f));
            if (layer == 0 && i == 5) {
                float blink = std::max(0.f, S(t * 3.f));
                glow(s, ccp(bx + bw * 0.5f, base + bh + 2.f), 8.f, C(1.f, 0.1f, 0.1f, 0.8f * blink), 3);
            }
            if (layer == 1) continue;
            for (float wy = base + 6.f; wy < base + bh - 4.f; wy += 11.f) {
                for (float wx = bx + 4.f; wx < bx + bw - 4.f; wx += 10.f) {
                    float h = std::fabs(S(wx * 12.9898f + wy * 78.233f) * 43758.5453f);
                    h -= std::floor(h);
                    if (h < 0.55f) continue;
                    float a = 0.55f + 0.25f * S(t * 1.5f + wx);
                    rect(s, wx, wy, wx + 4.f, wy + 5.f, C(1.f, 0.85f, 0.5f, a));
                }
            }
        }
    }
}

// ---------- far backdrops: drawn before each theme ----------

inline void drawBackdrop(BgState& s, float wind) {
    float W = s.W, H = s.H, t = s.t;
    switch (s.theme) {
        case 1: {
            float fl = 0.85f + 0.15f * S(t * 9.f) * S(t * 5.3f);
            float vx = W * 0.72f;
            ridgeLine(s, H * 0.2f, 26.f, 1.f, 0.01f, C(0.09f, 0.02f, 0.02f, 1.f), C(0.f, 0.f, 0.f, 0.f));
            tri(s, ccp(vx - 130.f, 0.f), ccp(vx + 130.f, 0.f), ccp(vx, H * 0.42f), C(0.06f, 0.015f, 0.015f, 1.f));
            segA(s, vx - 4.f, H * 0.41f, vx - 36.f, H * 0.14f, 2.f, C(1.f, 0.4f, 0.05f, 0.7f * fl));
            segA(s, vx + 4.f, H * 0.41f, vx + 40.f, H * 0.12f, 1.6f, C(1.f, 0.45f, 0.08f, 0.6f * fl));
            glow(s, ccp(vx, H * 0.42f), 80.f, C(1.f, 0.4f, 0.05f, 0.4f * fl), 4);
            for (int k = 0; k < 10; k++) {
                float age = std::fmod(t * 0.25f + k / 10.f, 1.f);
                float px = vx + 10.f * age + S(t * 0.5f + k) * 14.f * age + wind * age;
                float py = H * 0.42f + age * H * 0.45f;
                dot(s, px, py, 8.f + 20.f * age, C(0.12f, 0.1f, 0.1f, 0.35f * (1.f - age)));
            }
            break;
        }
        case 2: {
            ridgeLine(s, H * 0.34f, 48.f, 3.f, 0.01f, C(0.33f, 0.43f, 0.6f, 1.f), C(0.88f, 0.93f, 1.f, 0.95f));
            ridgeLine(s, H * 0.25f, 38.f, 6.f, 0.012f, C(0.22f, 0.31f, 0.47f, 1.f), C(0.8f, 0.87f, 0.98f, 0.9f));
            break;
        }
        case 3: {
            ridgeLine(s, H * 0.32f, 34.f, 8.f, 0.01f, C(0.6f, 0.74f, 0.84f, 1.f), C(0.f, 0.f, 0.f, 0.f));
            ridgeLine(s, H * 0.25f, 26.f, 11.f, 0.012f, C(0.54f, 0.72f, 0.74f, 1.f), C(0.f, 0.f, 0.f, 0.f));
            break;
        }
        case 4: {
            float hy = H * 0.34f;
            for (float x = W * 0.38f; x <= W * 0.62f; x += 6.f) {
                float u = (x - W * 0.38f) / (W * 0.24f);
                float h = 22.f * S(u * 3.14159f) * (0.7f + 0.3f * fbm(x * 0.05f, 0.f));
                seg(s, x, hy - 4.f, x, hy - 4.f + h, 6.5f, C(0.3f, 0.5f, 0.5f, 1.f));
            }
            break;
        }
        case 5: {
            float peak = H * 0.5f;
            float capH = 58.f;
            float half = W * 0.3f * (capH / peak);
            tri(s, ccp(0.f, 0.f), ccp(W * 0.6f, 0.f), ccp(W * 0.3f, peak), C(0.8f, 0.76f, 0.9f, 1.f));
            tri(s, ccp(W * 0.3f - half, peak - capH), ccp(W * 0.3f + half, peak - capH), ccp(W * 0.3f, peak),
                C(0.98f, 0.98f, 1.f, 1.f));
            ridgeLine(s, H * 0.22f, 22.f, 9.f, 0.01f, C(0.97f, 0.8f, 0.87f, 1.f), C(0.f, 0.f, 0.f, 0.f));
            break;
        }
        case 7: {
            glow(s, ccp(W * 0.35f + 20.f * S(t * 0.05f), H * 0.62f), 170.f, C(0.6f, 0.2f, 0.85f, 0.07f), 5);
            glow(s, ccp(W * 0.62f, H * 0.55f + 10.f * S(t * 0.04f)), 150.f, C(0.2f, 0.5f, 0.95f, 0.06f), 5);
            float gx = W * 0.74f;
            float gy = H * 0.7f;
            for (int k = 0; k < 140; k++) {
                float a = k * 0.2f + t * 0.03f;
                float r = 4.f + k * 0.5f;
                float arm = (k % 2 == 0) ? 0.f : 3.14159f;
                float x = gx + std::cos(a + arm) * r;
                float y = gy + std::sin(a + arm) * r * 0.32f;
                dotA(s, x, y, (k % 3 == 0) ? 1.3f : 0.8f, C(0.85f, 0.85f, 1.f, 0.35f * (1.f - k / 160.f)));
            }
            glow(s, ccp(gx, gy), 24.f, C(1.f, 0.9f, 0.7f, 0.2f), 3);
            break;
        }
        case 8: {
            rainClouds(s);
            skyline(s);
            break;
        }
        default:
            break;
    }
}

// ---------- extra life: drawn after each theme and its props ----------

inline void fishJumps(BgState& s) {
    float W = s.W, H = s.H, t = s.t;
    float sea = H * 0.34f * 0.45f;
    for (int f = 0; f < 2; f++) {
        float ph = std::fmod(t * 0.14f + f * 0.5f, 1.f);
        float x0 = W * (0.28f + 0.34f * f);
        if (ph < 0.2f) {
            float u = ph / 0.2f;
            float x = x0 + u * 36.f;
            float y = sea + 26.f * S(u * 3.14159f);
            ccColor4F fc = C(0.82f, 0.88f, 0.95f, 1.f);
            dot(s, x, y, 2.6f, fc);
            seg(s, x - 3.f, y, x - 9.f, y + (1.f - 2.f * u) * 4.f, 1.6f, fc);
        } else if (ph < 0.45f) {
            float v = (ph - 0.2f) / 0.25f;
            ellipse(s, x0 + 36.f, sea, 4.f + 18.f * v, 1.5f + 5.f * v, 0.9f, C(1.f, 1.f, 1.f, 0.5f * (1.f - v)));
        }
    }
}

inline void drawExtras(BgState& s, float wind) {
    float W = s.W, H = s.H, t = s.t;
    switch (s.theme) {
        case 1: {
            for (int i = 0; i < 36; i++) {
                float x = std::fmod(i * 97.1f + t * (5.f + (i % 5) * 2.f) + S(t * 0.7f + i) * 14.f + 1000.f, W + 20.f);
                float y = H - std::fmod(t * (12.f + (i % 7) * 3.f) + i * 53.3f, H + 20.f);
                dot(s, x, y, 0.9f + (i % 3) * 0.6f, C(0.28f, 0.25f, 0.25f, 0.55f));
            }
            break;
        }
        case 2: {
            for (int i = 0; i < 14; i++) {
                float sp = 40.f + (i % 5) * 14.f;
                float x = std::fmod(i * 83.3f + S(t * 0.4f + i) * 30.f + 2000.f, W);
                float y = H + 10.f - std::fmod(t * sp + i * 61.7f, H + 20.f);
                dot(s, x, y, 5.f + (i % 4) * 2.f, C(1.f, 1.f, 1.f, 0.10f));
            }
            float g = std::max(0.f, S(t * 0.9f + 1.f));
            if (g > 0.2f) {
                for (int i = 0; i < 10; i++) {
                    float x = std::fmod(i * 71.f + t * 260.f, W + 80.f) - 40.f;
                    float y = H * 0.2f + std::fmod(i * 37.7f, H * 0.6f);
                    seg(s, x, y, x - 40.f * g, y - 4.f * g, 0.7f, C(1.f, 1.f, 1.f, 0.35f * g));
                }
            }
            break;
        }
        case 3: {
            for (int i = 0; i < 3; i++) {
                float cx = std::fmod(i * 320.f + t * 10.f, W + 400.f) - 200.f;
                for (int j = -3; j <= 3; j++) {
                    float q = j / 3.5f;
                    float half = 90.f * std::sqrt(std::max(0.f, 1.f - q * q));
                    seg(s, cx - half, 38.f + j * 8.f, cx + half, 38.f + j * 8.f, 9.f, C(0.1f, 0.2f, 0.1f, 0.10f));
                }
            }
            for (int i = 0; i < 16; i++) {
                float x = std::fmod(i * 57.3f, W);
                float y = 10.f + std::fmod(i * 9.1f, 24.f);
                float a = std::max(0.f, S(t * 3.f + i * 1.9f));
                a = a * a * a;
                segA(s, x - 3.f, y, x + 3.f, y, 0.8f, C(1.f, 1.f, 1.f, a));
                segA(s, x, y - 3.f, x, y + 3.f, 0.8f, C(1.f, 1.f, 1.f, a));
            }
            break;
        }
        case 4: {
            fishJumps(s);
            break;
        }
        case 5: {
            for (int k = 0; k < 16; k++) {
                float u = std::fmod(t * 0.05f + k * 0.03f, 1.f);
                float x = W * 1.1f - u * W * 1.3f;
                float y = H * 0.55f + 40.f * S(u * 9.42f + t * 0.4f) + k * 1.5f;
                petal(s, x, y, 3.2f, u * 10.f + k, t * 3.f + k, C(1.f, 0.78f, 0.86f, 0.9f));
            }
            for (int i = 0; i < 8; i++) {
                float cx = std::fmod(i * 173.f + t * 6.f + S(i * 1.f) * 40.f + 500.f, W + 100.f) - 50.f;
                float cy = 18.f + (i % 3) * 10.f;
                seg(s, cx - 14.f, cy, cx + 14.f, cy, 7.f, C(1.f, 0.98f, 0.85f, 0.10f + 0.05f * S(t * 1.3f + i)));
            }
            break;
        }
        case 6: {
            for (int k = 0; k < 7; k++) {
                float y = 3.f + k * 5.2f;
                for (float x = 0.f; x < W; x += 14.f) {
                    float g = 0.5f + 0.5f * S(x * 0.012f + t * 0.3f);
                    float w = 8.f + 5.f * S(x * 0.05f + t * 1.2f + k);
                    float ox = 6.f * S(t * 0.8f + k + x * 0.04f);
                    segA(s, x + ox, y, x + ox + w, y, 1.3f,
                        C(0.2f + 0.5f * (1.f - g), 0.3f + 0.9f * g, 0.6f + 0.3f * g, 0.13f * (1.f - k / 8.f)));
                }
            }
            break;
        }
        case 7: {
            for (int m = 0; m < 2; m++) {
                float ph = std::fmod(t * 0.16f + m * 0.5f, 1.f);
                if (ph > 0.14f) continue;
                float u = ph / 0.14f;
                float px = W * (0.95f - 0.45f * m) - u * 340.f;
                float py = H * 0.92f - 20.f * m - u * 150.f;
                float a = 1.f - u;
                for (int k = 0; k < 5; k++) {
                    float tx = px + k * 14.f;
                    float ty = py + k * 6.2f;
                    segA(s, tx, ty, tx + 14.f, ty + 6.2f, 1.6f - k * 0.25f, C(1.f, 1.f, 1.f, a * (0.8f - k * 0.15f)));
                }
                dotA(s, px, py, 2.4f, C(1.f, 1.f, 1.f, a));
            }
            break;
        }
        case 8: {
            float ph = std::fmod(t * 0.07f, 1.f);
            float x = -80.f + ph * (W + 160.f);
            float y = 16.f;
            ccColor4F body = C(0.03f, 0.03f, 0.05f, 1.f);
            rect(s, x - 38.f, y - 2.f, x, y + 9.f, body);
            rect(s, x - 28.f, y + 9.f, x - 8.f, y + 15.f, body);
            triA(s, ccp(x, y + 4.f), ccp(x + 130.f, y - 6.f), ccp(x + 130.f, y + 20.f), C(1.f, 0.95f, 0.7f, 0.10f));
            glow(s, ccp(x, y + 4.f), 14.f, C(1.f, 0.95f, 0.75f, 0.45f), 3);
            segA(s, x - 38.f, y + 4.f, x - 44.f, y + 4.f, 2.f, C(1.f, 0.2f, 0.15f, 0.8f));
            segA(s, x, y - 5.f, x + 60.f, y - 5.f, 3.f, C(1.f, 0.95f, 0.7f, 0.12f));
            break;
        }
        default:
            break;
    }
}

}
