#pragma once
#include "Features.hpp"
#include <deque>

namespace vanta {

struct TrailPt {
    CCRect r;
    bool hold = false;
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

// styled player hitbox (Neon / Glass / Outline)
inline void styledBox(CCDrawNode* n, CCRect const& r, ccColor4F c) {
    int style = getIndex("hb-style");
    float th = hbThick();
    float fill = hbFill();
    ccColor4F white{1.f, 1.f, 1.f, 1.f};

    if (style == 0) {
        strokeBox(n, r, 1.5f, th * 3.4f, withA(c, 0.10f));
        strokeBox(n, r, 0.8f, th * 2.0f, withA(c, 0.22f));
        fillBox(n, r, 0.f, withA(c, fill));
        strokeBox(n, r, 0.f, th, withA(c, 1.f));
        strokeBox(n, r, -0.5f, th * 0.4f, withA(white, 0.7f));
    } else if (style == 1) {
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
    } else {
        fillBox(n, r, 0.f, withA(c, fill));
        strokeBox(n, r, 0.f, th * 1.2f, withA(c, 1.f));
    }
}

inline void drawPlayerBox(CCDrawNode* n, PlayerObject* p, bool p2) {
    if (!p) return;
    CCRect r = p->getObjectRect();
    ccColor4F c = colorOf("hb-color");
    if (p2) c = ccColor4F{c.g, c.b, c.r, 1.f};
    styledBox(n, r, c);
    strokeBox(n, scaledRect(r, 0.5f), 0.f, hbThick(), ccColor4F{0.45f, 0.85f, 1.f, 0.95f});
}

// hitbox trail (Boxes / Neon / Ghost)
inline void drawTrail(CCDrawNode* node, std::deque<TrailPt> const& pts, bool p2, bool dead) {
    size_t n = pts.size();
    if (n == 0) return;

    int style = getIndex("trail-style");
    bool inside = isOn("trail-inside");
    bool hold = isOn("trail-hold");
    ccColor4F base = trailColor(p2);
    ccColor4F ins = colorOf("trail-color-inside");
    ccColor4F holdC = colorOf("trail-color-hold");
    float th = hbThick();

    CCPoint prev = ccp(0.f, 0.f);
    bool havePrev = false;

    for (size_t i = 0; i < n; i++) {
        float k = float(i + 1) / float(n);
        float fade = 0.1f + 0.9f * k * k;
        CCRect const& r = pts[i].r;
        ccColor4F c = (hold && pts[i].hold) ? holdC : base;
        CCPoint ctr = centerOf(r);

        if (inside) {
            fillBox(node, r, 0.f, withA(ins, 0.32f * fade));
        }

        if (style == 0) {
            strokeBox(node, r, 0.f, th, withA(c, fade));
        } else if (style == 1) {
            if (i + 120 >= n) {
                strokeBox(node, r, 1.f, th * 3.f, withA(c, 0.12f * fade));
            }
            strokeBox(node, r, 0.f, th, withA(c, fade));
            if (havePrev) {
                node->drawSegment(prev, ctr, 0.6f + 1.4f * k, withA(c, 0.5f * fade));
            }
        } else {
            CCRect g = scaledRect(r, 0.45f + 0.55f * k);
            fillBox(node, g, 0.f, withA(c, 0.35f * fade));
        }

        prev = ctr;
        havePrev = true;
    }

    if (dead) {
        strokeBox(node, pts[n - 1].r, 2.f, th * 2.f, ccColor4F{1.f, 1.f, 1.f, 0.9f});
    }
}

}
