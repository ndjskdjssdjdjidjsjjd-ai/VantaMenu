#pragma once
#include "Features.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <deque>
#include <vector>

namespace vanta {

struct TrailPt {
    CCRect r;
    bool hold = false;
    int frame = 0;
};

// animated effects: click rings, death rings and shards
struct HbFx {
    CCPoint p = ccp(0.f, 0.f);
    CCPoint v = ccp(0.f, 0.f);
    float age = 0.f;
    float life = 0.4f;
    float size = 20.f;
    int kind = 0; // 0 = click ring, 1 = death ring, 2 = shard
};

inline bool animOn() {
    return isOn("hb-anim");
}

inline float rnd(float a, float b) {
    return a + (b - a) * (std::rand() / (float)RAND_MAX);
}

inline ccColor4F withA(ccColor4F c, float a) {
    return ccColor4F{c.r, c.g, c.b, a};
}

inline void quadOf(CCRect const& r, float g, CCPoint* q) {
    q[0] = ccp(r.getMinX() - g, r.getMinY() - g);
    q[1] = ccp(r.getMaxX() + g, r.getMinY() - g);
    q[2] = ccp(r.getMaxX() + g, r.getMaxY() + g);
    q[3] = ccp(r.getMinX() - g, r.getMaxY() + g);
}

inline void fillBox(CCDrawNode* n, CCRect const& r, float g, ccColor4F c) {
    CCPoint q[4];
    quadOf(r, g, q);
    n->drawPolygon(q, 4, c, 0.f, ccColor4F{0.f, 0.f, 0.f, 0.f});
}

inline void strokeBox(CCDrawNode* n, CCRect const& r, float g, float w, ccColor4F c) {
    CCPoint q[4];
    quadOf(r, g, q);
    n->drawPolygon(q, 4, ccColor4F{0.f, 0.f, 0.f, 0.f}, w, c);
}

inline CCPoint centerOf(CCRect const& r) {
    return ccp(r.origin.x + r.size.width * 0.5f, r.origin.y + r.size.height * 0.5f);
}

inline CCRect scaledRect(CCRect const& r, float k) {
    CCPoint c = centerOf(r);
    float w = r.size.width * k;
    float h = r.size.height * k;
    return CCRect(c.x - w * 0.5f, c.y - h * 0.5f, w, h);
}

// ---------- effects ----------
inline void spawnRing(std::vector<HbFx>& v, CCPoint p, int kind, float size, float life) {
    HbFx e;
    e.p = p;
    e.kind = kind;
    e.size = size;
    e.life = life;
    v.push_back(e);
}

inline void spawnShards(std::vector<HbFx>& v, CCPoint p) {
    for (int i = 0; i < 18; i++) {
        HbFx e;
        e.p = p;
        e.kind = 2;
        float a = rnd(0.f, 6.2831853f);
        float sp = rnd(60.f, 220.f);
        e.v = ccp(std::cos(a) * sp, std::sin(a) * sp + 60.f);
        e.size = rnd(1.5f, 4.f);
        e.life = rnd(0.45f, 0.9f);
        v.push_back(e);
    }
}

inline void drawFx(CCDrawNode* n, std::vector<HbFx>& v, float dt) {
    for (size_t i = 0; i < v.size();) {
        auto& e = v[i];
        e.age += dt;
        if (e.age >= e.life) {
            v.erase(v.begin() + i);
            continue;
        }
        float k = e.age / e.life;
        if (e.kind == 2) {
            e.v.y -= 260.f * dt;
            e.p.x += e.v.x * dt;
            e.p.y += e.v.y * dt;
            float s = e.size * (1.f - 0.6f * k);
            n->drawDot(e.p, s, ccColor4F{1.f, 0.35f + 0.4f * (1.f - k), 0.2f, 1.f - k});
        } else {
            float ease = 1.f - (1.f - k) * (1.f - k);
            float r = e.size * ease;
            CCPoint pts[28];
            for (int j = 0; j < 28; j++) {
                float a = j * 6.2831853f / 28.f;
                pts[j] = ccp(e.p.x + std::cos(a) * r, e.p.y + std::sin(a) * r);
            }
            ccColor4F c = (e.kind == 0)
                ? ccColor4F{0.4f, 1.f, 0.7f, 1.f - k}
                : ccColor4F{1.f, 0.25f, 0.25f, 1.f - k};
            n->drawPolygon(pts, 28, ccColor4F{c.r, c.g, c.b, 0.08f * (1.f - k)}, 1.6f * (1.f - k) + 0.4f, c);
        }
        ++i;
    }
}

// ---------- player hitbox: Classic (GD look), Neon or Glass ----------
inline void styledBox(CCDrawNode* n, CCRect const& r, ccColor4F c, bool holding) {
    int style = getIndex("hb-style");
    float th = hbThick();
    float fill = hbFill();
    bool an = animOn();
    float br = an ? 0.85f + 0.15f * std::sin(g_anim * 5.f) : 1.f;
    float hl = (an && holding) ? 1.f : 0.f;
    ccColor4F white{1.f, 1.f, 1.f, 1.f};
    ccColor4F cyan{0.45f, 0.85f, 1.f, 0.95f};

    if (style == 0) {
        ccColor4F red{1.f, 0.12f, 0.12f, 1.f};
        ccColor4F blue{0.2f, 0.45f, 1.f, 1.f};
        CCRect inner = scaledRect(r, 0.5f);
        strokeBox(n, r, 0.f, th * 2.6f, ccColor4F{0.f, 0.f, 0.f, 0.55f});
        strokeBox(n, r, 1.2f, th * 2.6f, withA(red, 0.10f * br + 0.14f * hl));
        fillBox(n, r, 0.f, withA(red, 0.18f + fill * 0.4f + 0.08f * hl));
        strokeBox(n, r, 0.f, th, withA(red, 0.95f));
        fillBox(n, inner, 0.f, withA(blue, 0.3f + fill * 0.4f + 0.1f * hl));
        strokeBox(n, inner, 0.f, th, withA(blue, 0.95f));
        n->drawDot(centerOf(r), 0.9f, withA(white, 0.8f));
    } else if (style == 1) {
        strokeBox(n, r, 1.5f, th * 3.4f, withA(c, 0.10f * br + 0.12f * hl));
        strokeBox(n, r, 0.8f, th * 2.0f, withA(c, 0.22f * br));
        fillBox(n, r, 0.f, withA(c, fill));
        strokeBox(n, r, 0.f, th, withA(c, 1.f));
        strokeBox(n, r, -0.5f, th * 0.4f, withA(white, 0.7f + 0.3f * hl));
        strokeBox(n, scaledRect(r, 0.5f), 0.f, th, cyan);
    } else {
        fillBox(n, r, 0.f, withA(c, 0.15f + fill + 0.1f * hl));
        strokeBox(n, r, 0.f, th, withA(white, 0.9f));
        float x0 = r.getMinX();
        float x1 = r.getMaxX();
        float y0 = r.getMinY();
        float y1 = r.getMaxY();
        float L = 5.f;
        ccColor4F wc = withA(white, 0.95f);
        n->drawSegment(ccp(x0, y0), ccp(x0 + L, y0), th, wc);
        n->drawSegment(ccp(x0, y0), ccp(x0, y0 + L), th, wc);
        n->drawSegment(ccp(x1, y0), ccp(x1 - L, y0), th, wc);
        n->drawSegment(ccp(x1, y0), ccp(x1, y0 + L), th, wc);
        n->drawSegment(ccp(x0, y1), ccp(x0 + L, y1), th, wc);
        n->drawSegment(ccp(x0, y1), ccp(x0, y1 - L), th, wc);
        n->drawSegment(ccp(x1, y1), ccp(x1 - L, y1), th, wc);
        n->drawSegment(ccp(x1, y1), ccp(x1, y1 - L), th, wc);
        n->drawSegment(ccp(x0 + 2.f, y1 - 1.5f), ccp(x1 - 2.f, y1 - 1.5f), 0.6f, withA(white, 0.4f));
        strokeBox(n, scaledRect(r, 0.5f), 0.f, th, cyan);
    }
}

inline void drawPlayerBox(CCDrawNode* n, PlayerObject* p, bool p2, bool holding) {
    if (!p) return;
    CCRect r = p->getObjectRect();
    ccColor4F c = ccColor4F{1.f, 0.23f, 0.36f, 1.f};
    if (p2) c = ccColor4F{c.g, c.b, c.r, 1.f};
    styledBox(n, r, c, holding);
}

// ---------- analysis aids: frame ticks and press/release markers ----------
inline void drawMarkers(CCDrawNode* node, std::deque<TrailPt> const& pts) {
    size_t n = pts.size();
    if (n < 2) return;
    bool clicks = isOn("trail-markers");
    bool ticks = false;
    if (!clicks && !ticks) return;

    ccColor4F green{0.2f, 1.f, 0.4f, 1.f};
    ccColor4F red{1.f, 0.35f, 0.35f, 1.f};

    for (size_t i = 1; i < n; i++) {
        CCRect const& r = pts[i].r;
        CCPoint c = centerOf(r);
        float k = float(i + 1) / float(n);
        float top = r.getMaxY();
        float bot = r.getMinY();
        int fr = pts[i].frame;

        if (ticks) {
            if (fr % 60 == 0) {
                node->drawSegment(ccp(c.x, bot - 7.f), ccp(c.x, top + 7.f), 1.2f,
                    ccColor4F{1.f, 1.f, 1.f, 0.85f * k});
            } else if (fr % 10 == 0) {
                node->drawSegment(ccp(c.x, bot - 3.f), ccp(c.x, top + 3.f), 0.8f,
                    ccColor4F{1.f, 1.f, 1.f, 0.45f * k});
            }
        }

        if (clicks && pts[i].hold != pts[i - 1].hold) {
            if (pts[i].hold) {
                node->drawSegment(ccp(c.x, top), ccp(c.x, top + 8.f), 0.9f, withA(green, 0.9f * k));
                node->drawDot(ccp(c.x, top + 9.f), 2.6f, withA(green, k));
            } else {
                node->drawSegment(ccp(c.x, bot), ccp(c.x, bot - 8.f), 0.9f, withA(red, 0.9f * k));
                node->drawDot(ccp(c.x, bot - 9.f), 2.6f, withA(red, k));
            }
        }
    }
}

// ---------- velocity arrow ----------
inline void drawArrow(CCDrawNode* n, CCPoint from, CCPoint vel) {
    float len = std::sqrt(vel.x * vel.x + vel.y * vel.y);
    if (len < 0.05f) return;
    CCPoint to = ccp(from.x + vel.x * 8.f, from.y + vel.y * 8.f);
    ccColor4F c{1.f, 0.95f, 0.4f, 0.95f};
    n->drawSegment(from, to, 1.2f, c);
    float dx = vel.x / len;
    float dy = vel.y / len;
    float a = 2.6f;
    float ca = std::cos(a);
    float sa = std::sin(a);
    CCPoint h1 = ccp(to.x + (dx * ca - dy * sa) * 6.f, to.y + (dx * sa + dy * ca) * 6.f);
    CCPoint h2 = ccp(to.x + (dx * ca + dy * sa) * 6.f, to.y + (-dx * sa + dy * ca) * 6.f);
    n->drawSegment(to, h1, 1.2f, c);
    n->drawSegment(to, h2, 1.2f, c);
}

// ---------- hitbox trail ----------
inline void drawTrail(CCDrawNode* node, std::deque<TrailPt> const& pts, bool p2, bool dead) {
    size_t n = pts.size();
    if (n == 0) return;

    int style = getIndex("trail-style");
    bool inside = isOn("trail-inside");
    bool hold = isOn("trail-hold");
    bool smart = true;
    bool motion = isOn("trail-motion");
    bool an = animOn();
    ccColor4F base = trailColor(p2);
    ccColor4F ins = colorOf("trail-color-inside");
    ccColor4F holdC = colorOf("trail-color-hold");
    float th = hbThick();

    // a bright pulse that travels along the trail from tail to head
    float pulseIdx = an ? std::fmod(g_anim * 0.9f, 1.f) * float(n) : -1000.f;
    float pulseW = std::max(4.f, float(n) * 0.06f);

    CCPoint prev = ccp(0.f, 0.f);
    CCRect prevR;
    bool havePrev = false;
    CCPoint lastBox = ccp(0.f, 0.f);
    bool haveBox = false;

    for (size_t i = 0; i < n; i++) {
        float k = float(i + 1) / float(n);
        float fade = 0.3f + 0.7f * k;
        float d = std::fabs(float(i) - pulseIdx);
        float boost = d < pulseW ? 1.f - d / pulseW : 0.f;
        float fa = std::min(1.f, fade + 0.5f * boost);
        float tt = th * (1.f + 0.8f * boost);

        CCRect const& r = pts[i].r;
        ccColor4F c = (hold && pts[i].hold) ? holdC : base;
        CCPoint ctr = centerOf(r);

        if (havePrev) {
            ccColor4F lc = c;
            if (motion) {
                float dy = ctr.y - prev.y;
                if (dy > 0.4f) lc = ccColor4F{0.3f, 0.9f, 1.f, 1.f};
                else if (dy < -0.4f) lc = ccColor4F{1.f, 0.55f, 0.2f, 1.f};
                else lc = ccColor4F{1.f, 1.f, 1.f, 1.f};
            }
            float w = (1.2f + 1.4f * k) * (1.f + 0.8f * boost);
            node->drawSegment(prev, ctr, w + 1.6f, ccColor4F{0.f, 0.f, 0.f, 0.4f * fa});
            node->drawSegment(prev, ctr, w, withA(lc, 0.9f * fa));

            if (style == 3) {
                CCPoint q[4] = {
                    ccp(prev.x, prevR.getMinY()), ccp(ctr.x, r.getMinY()),
                    ccp(ctr.x, r.getMaxY()), ccp(prev.x, prevR.getMaxY())
                };
                node->drawPolygon(q, 4, withA(c, 0.30f * fa), 0.f, ccColor4F{0.f, 0.f, 0.f, 0.f});
                node->drawSegment(q[0], q[1], 0.8f, withA(c, 0.8f * fa));
                node->drawSegment(q[3], q[2], 0.8f, withA(c, 0.8f * fa));
            }
        }

        bool drawBox = (style != 3) || (i + 1 == n);
        if (drawBox && smart && haveBox && i + 1 < n) {
            float minD = std::max(r.size.width, r.size.height) * 0.75f;
            float dx = ctr.x - lastBox.x;
            float dy = ctr.y - lastBox.y;
            bool changed = i > 0 && pts[i].hold != pts[i - 1].hold;
            if (dx * dx + dy * dy < minD * minD && !changed) drawBox = false;
        }

        if (drawBox) {
            if (inside) {
                fillBox(node, r, 0.f, withA(ins, 0.4f * fa));
            }
            if (style == 2) {
                CCRect g = scaledRect(r, 0.45f + 0.55f * k);
                fillBox(node, g, 0.f, withA(c, 0.5f * fa));
            } else {
                strokeBox(node, r, 0.f, tt * 2.4f, ccColor4F{0.f, 0.f, 0.f, 0.45f * fa});
                if (style == 1 && (i + 80 >= n || boost > 0.f)) {
                    strokeBox(node, r, 1.f, tt * 3.f, withA(c, 0.16f * fa));
                }
                strokeBox(node, r, 0.f, tt * 1.2f, withA(c, fa));
            }
            lastBox = ctr;
            haveBox = true;
        }

        prev = ctr;
        prevR = r;
        havePrev = true;
    }

    drawMarkers(node, pts);

    // the newest frame is always drawn crisp so you can see where you are
    strokeBox(node, pts[n - 1].r, 0.f, th * 1.5f, ccColor4F{1.f, 1.f, 1.f, 0.95f});
    if (dead) {
        strokeBox(node, pts[n - 1].r, 2.f, th * 2.f, ccColor4F{1.f, 1.f, 1.f, 0.9f});
    }
}

// ---------- level object hitbox: 0 = solid (blue), 1 = hazard (red), 2 = special (green) ----------
inline void objectBox(CCDrawNode* n, CCRect const& r, int kind, float prox) {
    if (r.size.width < 1.f || r.size.height < 1.f) return;
    ccColor4F c = ccColor4F{0.25f, 0.55f, 1.f, 1.f};
    if (kind == 1) c = ccColor4F{1.f, 0.2f, 0.25f, 1.f};
    if (kind == 2) c = ccColor4F{0.25f, 1.f, 0.5f, 1.f};

    bool an = animOn();
    if (!an) prox = 0.f;
    float pulse = (an && kind == 1) ? 0.85f + 0.15f * std::sin(g_anim * 6.f) : 1.f;

    int style = getIndex("hb-style");
    float th = hbThick() * (1.f + 0.6f * prox);
    float fill = hbFill();

    // dark halo so the box stays visible on any background
    strokeBox(n, r, 0.f, th * 2.4f, ccColor4F{0.f, 0.f, 0.f, 0.55f});

    // objects close to the player glow brighter
    if (prox > 0.02f) {
        strokeBox(n, r, 1.2f, th * 2.4f, withA(c, 0.22f * prox));
    }

    if (style == 1) {
        strokeBox(n, r, 0.8f, th * 2.2f, withA(c, 0.16f * pulse));
        fillBox(n, r, 0.f, withA(c, fill * 0.7f + 0.2f * prox));
        strokeBox(n, r, 0.f, th, withA(c, 1.f));
    } else if (style == 2) {
        fillBox(n, r, 0.f, withA(c, (0.2f + fill * 0.5f) * pulse + 0.2f * prox));
        strokeBox(n, r, 0.f, th, ccColor4F{1.f, 1.f, 1.f, 0.85f});
    } else {
        fillBox(n, r, 0.f, withA(c, (0.18f + fill * 0.45f) * pulse + 0.2f * prox));
        strokeBox(n, r, 0.f, th, withA(c, 0.95f));
    }
}

// highlights the object that killed the player
inline void drawDeath(CCDrawNode* n, CCRect const& obj, CCRect const& pl) {
    float p = animOn() ? 0.8f + 0.2f * std::sin(g_anim * 10.f) : 1.f;
    fillBox(n, obj, 1.5f, ccColor4F{1.f, 0.1f, 0.1f, 0.35f * p});
    strokeBox(n, obj, 1.5f, 2.f, ccColor4F{1.f, 1.f, 1.f, 0.95f});
    strokeBox(n, obj, 3.5f, 3.f, ccColor4F{1.f, 0.2f, 0.2f, 0.3f * p});
    n->drawSegment(centerOf(pl), centerOf(obj), 0.8f, ccColor4F{1.f, 1.f, 1.f, 0.6f});
}

}
