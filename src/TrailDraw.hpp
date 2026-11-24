#pragma once
#include "Features.hpp"
#include <algorithm>
#include <deque>

namespace vanta {

struct TrailPt {
    CCRect r;
    bool hold = false;
    int frame = 0;
};

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

// player hitbox: Classic (GD look), Neon or Glass
inline void styledBox(CCDrawNode* n, CCRect const& r, ccColor4F c) {
    int style = getIndex("hb-style");
    float th = hbThick();
    float fill = hbFill();
    ccColor4F white{1.f, 1.f, 1.f, 1.f};

    if (style == 0) {
        ccColor4F red{1.f, 0.12f, 0.12f, 1.f};
        ccColor4F blue{0.2f, 0.45f, 1.f, 1.f};
        CCRect inner = scaledRect(r, 0.5f);
        fillBox(n, r, 0.f, withA(red, 0.10f + fill * 0.4f));
        strokeBox(n, r, 0.f, th, withA(red, 0.95f));
        fillBox(n, inner, 0.f, withA(blue, 0.2f + fill * 0.4f));
        strokeBox(n, inner, 0.f, th, withA(blue, 0.95f));
        n->drawDot(centerOf(r), 0.9f, withA(white, 0.8f));
    } else if (style == 1) {
        strokeBox(n, r, 1.5f, th * 3.4f, withA(c, 0.10f));
        strokeBox(n, r, 0.8f, th * 2.0f, withA(c, 0.22f));
        fillBox(n, r, 0.f, withA(c, fill));
        strokeBox(n, r, 0.f, th, withA(c, 1.f));
        strokeBox(n, r, -0.5f, th * 0.4f, withA(white, 0.7f));
        strokeBox(n, scaledRect(r, 0.5f), 0.f, th, ccColor4F{0.45f, 0.85f, 1.f, 0.95f});
    } else {
        fillBox(n, r, 0.f, withA(c, 0.15f + fill));
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
        strokeBox(n, scaledRect(r, 0.5f), 0.f, th, ccColor4F{0.45f, 0.85f, 1.f, 0.95f});
    }
}

inline void drawPlayerBox(CCDrawNode* n, PlayerObject* p, bool p2) {
    if (!p) return;
    CCRect r = p->getObjectRect();
    ccColor4F c = colorOf("hb-color");
    if (p2) c = ccColor4F{c.g, c.b, c.r, 1.f};
    styledBox(n, r, c);
}

// analysis aids: frame ticks and press/release markers
inline void drawMarkers(CCDrawNode* node, std::deque<TrailPt> const& pts) {
    size_t n = pts.size();
    if (n < 2) return;
    bool clicks = isOn("trail-markers");
    bool ticks = isOn("trail-ticks");
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

// hitbox trail: centre line + clean boxes (Boxes / Neon / Ghost) + analysis markers
inline void drawTrail(CCDrawNode* node, std::deque<TrailPt> const& pts, bool p2, bool dead) {
    size_t n = pts.size();
    if (n == 0) return;

    int style = getIndex("trail-style");
    bool inside = isOn("trail-inside");
    bool hold = isOn("trail-hold");
    bool smart = isOn("trail-smart");
    ccColor4F base = trailColor(p2);
    ccColor4F ins = colorOf("trail-color-inside");
    ccColor4F holdC = colorOf("trail-color-hold");
    float th = hbThick();

    CCPoint prev = ccp(0.f, 0.f);
    bool havePrev = false;
    CCPoint lastBox = ccp(0.f, 0.f);
    bool haveBox = false;

    for (size_t i = 0; i < n; i++) {
        float k = float(i + 1) / float(n);
        float fade = 0.12f + 0.88f * k;
        CCRect const& r = pts[i].r;
        ccColor4F c = (hold && pts[i].hold) ? holdC : base;
        CCPoint ctr = centerOf(r);

        // centre line: shows the exact path frame by frame
        if (havePrev) {
            float w = (style == 1) ? 0.6f + 1.2f * k : 0.7f;
            node->drawSegment(prev, ctr, w, withA(c, 0.55f * fade));
        }

        bool drawBox = true;
        if (smart && haveBox && i + 1 < n) {
            float minD = std::max(r.size.width, r.size.height) * 0.75f;
            float dx = ctr.x - lastBox.x;
            float dy = ctr.y - lastBox.y;
            bool changed = i > 0 && pts[i].hold != pts[i - 1].hold;
            if (dx * dx + dy * dy < minD * minD && !changed) drawBox = false;
        }

        if (drawBox) {
            if (inside) {
                fillBox(node, r, 0.f, withA(ins, 0.28f * fade));
            }
            if (style == 0) {
                strokeBox(node, r, 0.f, th, withA(c, fade));
            } else if (style == 1) {
                if (i + 80 >= n) {
                    strokeBox(node, r, 1.f, th * 3.f, withA(c, 0.12f * fade));
                }
                strokeBox(node, r, 0.f, th, withA(c, fade));
            } else {
                CCRect g = scaledRect(r, 0.45f + 0.55f * k);
                fillBox(node, g, 0.f, withA(c, 0.35f * fade));
            }
            lastBox = ctr;
            haveBox = true;
        }

        prev = ctr;
        havePrev = true;
    }

    drawMarkers(node, pts);

    if (dead) {
        strokeBox(node, pts[n - 1].r, 2.f, th * 2.f, ccColor4F{1.f, 1.f, 1.f, 0.9f});
    }
}

// level object hitbox: kind 0 = solid (blue), 1 = hazard (red), 2 = special (green)
inline void objectBox(CCDrawNode* n, CCRect const& r, int kind) {
    if (r.size.width < 1.f || r.size.height < 1.f) return;
    ccColor4F c = ccColor4F{0.25f, 0.55f, 1.f, 1.f};
    if (kind == 1) c = ccColor4F{1.f, 0.2f, 0.25f, 1.f};
    if (kind == 2) c = ccColor4F{0.25f, 1.f, 0.5f, 1.f};

    int style = getIndex("hb-style");
    float th = hbThick();
    float fill = hbFill();

    if (style == 1) {
        strokeBox(n, r, 0.8f, th * 2.2f, withA(c, 0.16f));
        fillBox(n, r, 0.f, withA(c, fill * 0.7f));
        strokeBox(n, r, 0.f, th, withA(c, 1.f));
    } else if (style == 2) {
        fillBox(n, r, 0.f, withA(c, 0.12f + fill * 0.5f));
        strokeBox(n, r, 0.f, th, ccColor4F{1.f, 1.f, 1.f, 0.85f});
    } else {
        fillBox(n, r, 0.f, withA(c, 0.10f + fill * 0.4f));
        strokeBox(n, r, 0.f, th, withA(c, 0.95f));
    }
}

// highlights the object that killed the player
inline void drawDeath(CCDrawNode* n, CCRect const& obj, CCRect const& pl) {
    fillBox(n, obj, 1.5f, ccColor4F{1.f, 0.1f, 0.1f, 0.35f});
    strokeBox(n, obj, 1.5f, 2.f, ccColor4F{1.f, 1.f, 1.f, 0.95f});
    strokeBox(n, obj, 3.5f, 3.f, ccColor4F{1.f, 0.2f, 0.2f, 0.3f});
    n->drawSegment(centerOf(pl), centerOf(obj), 0.8f, ccColor4F{1.f, 1.f, 1.f, 0.6f});
}

}
