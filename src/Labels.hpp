#pragma once
#include "Features.hpp"

namespace vanta {

struct LabelData {
    float fps = 60.f;
    float time = 0.f;
    int attempts = 0;
    float percent = 0.f;
    float x = 0.f;
    float y = 0.f;
    float velY = 0.f;
    int ncFrames = 0;
    int ncHit = 0;
    int ncDeaths = 0;
};

inline void updateLabel(CCLabelBMFont* label, LabelData const& d, CCSize win) {
    if (!isOn("labels")) {
        label->setVisible(false);
        return;
    }

    std::string s;
    if (isOn("label-fps")) s += fmt::format("FPS: {}\n", (int)d.fps);
    if (isOn("label-cps")) s += fmt::format("CPS: {}\n", cps());
    if (isOn("label-time")) s += fmt::format("Time: {:.2f}s\n", d.time);
    if (isOn("label-attempt")) s += fmt::format("Attempt: {}\n", d.attempts);
    if (isOn("label-percent")) s += fmt::format("Percent: {:.2f}%\n", d.percent);
    if (isOn("label-xy")) s += fmt::format("X: {:.1f}  Y: {:.1f}\n", d.x, d.y);
    if (isOn("label-vel")) s += fmt::format("Vel Y: {:.1f}\n", d.velY);

    if (isOn("noclip")) {
        float accPct = 100.f;
        if (d.ncFrames > 0) {
            accPct = 100.f * float(d.ncFrames - d.ncHit) / float(d.ncFrames);
        }
        if (isOn("label-nc-acc")) {
            s += "NC Acc: " + fmt::format("{:.2f}", accPct) + "%\n";
        }
        if (isOn("label-nc-deaths")) {
            s += "NC Deaths: " + std::to_string(d.ncDeaths) + "\n";
        }
    }

    if (isOn("label-macro")) {
        int mode = macroMode();
        if (mode == 1) {
            s += fmt::format("Macro: Recording ({})\n", g_macro.size());
        } else if (mode == 2) {
            s += fmt::format("Macro: Playing {}/{}\n", g_playIndex, g_macro.size());
        }
    }

    label->setVisible(true);
    label->setString(s.c_str());
    label->setScale(0.35f * labelScale());
    label->setOpacity((GLubyte)(255.f * labelOpacity()));
    label->setColor(labelColor());

    float pad = 6.f;
    switch (getIndex("label-pos")) {
        case 1:
            label->setAnchorPoint({1.f, 1.f});
            label->setAlignment(kCCTextAlignmentRight);
            label->setPosition({win.width - pad, win.height - pad});
            break;
        case 2:
            label->setAnchorPoint({0.f, 0.f});
            label->setAlignment(kCCTextAlignmentLeft);
            label->setPosition({pad, pad});
            break;
        case 3:
            label->setAnchorPoint({1.f, 0.f});
            label->setAlignment(kCCTextAlignmentRight);
            label->setPosition({win.width - pad, pad});
            break;
        default:
            label->setAnchorPoint({0.f, 1.f});
            label->setAlignment(kCCTextAlignmentLeft);
            label->setPosition({26.f, win.height - pad});
            break;
    }
}

}
