#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include "Features.hpp"

using namespace geode::prelude;

// ---------- Macro bot + CPS + hold state (input level) ----------
class $modify(VantaBaseGameLayer, GJBaseGameLayer) {
    void handleButton(bool down, int button, bool isPlayer1) {
        auto pl = PlayLayer::get();
        if (pl && static_cast<GJBaseGameLayer*>(pl) == this) {
            int mode = vanta::macroMode();

            if (mode == 2 && !vanta::g_injecting) return;

            if (button == 1) {
                if (isPlayer1) vanta::g_hold1 = down;
                else vanta::g_hold2 = down;
                if (down) {
                    if (isPlayer1) vanta::g_click1 = true;
                    else vanta::g_click2 = true;
                }
            }

            if (down) vanta::registerClick();

            if (mode == 1 && !vanta::g_injecting) {
                vanta::g_macro.push_back({vanta::g_frame, down, button, isPlayer1});
            }
        }
        GJBaseGameLayer::handleButton(down, button, isPlayer1);
    }

    void processCommands(float dt, bool isHalfTick, bool isLastTick) {
        auto pl = PlayLayer::get();
        if (pl && static_cast<GJBaseGameLayer*>(pl) == this) {
            if (vanta::macroMode() == 2) {
                while (vanta::g_playIndex < vanta::g_macro.size() &&
                       vanta::g_macro[vanta::g_playIndex].frame <= vanta::g_frame) {
                    auto a = vanta::g_macro[vanta::g_playIndex++];
                    vanta::g_injecting = true;
                    this->handleButton(a.down, a.button, a.p1);
                    vanta::g_injecting = false;
                }
            }
            GJBaseGameLayer::processCommands(dt, isHalfTick, isLastTick);
            vanta::g_frame++;
            return;
        }
        GJBaseGameLayer::processCommands(dt, isHalfTick, isLastTick);
    }
};
