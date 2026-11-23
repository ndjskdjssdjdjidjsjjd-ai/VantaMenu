#pragma once
#include <Geode/Geode.hpp>
#include <chrono>
#include <deque>
#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <vector>

using namespace geode::prelude;

namespace vanta {

enum class Kind { Toggle, Cycle };

struct Feature {
    std::string id;
    std::string name;
    std::string desc;
    std::string category;
    Kind kind = Kind::Toggle;
    std::vector<std::string> options;
    int defIndex = 0; // toggles: 1 = on by default
    std::function<void()> apply;
};

inline std::vector<Feature> const& features();

inline Feature const* findFeature(std::string const& id) {
    for (auto const& f : features()) {
        if (f.id == id) return &f;
    }
    return nullptr;
}

inline bool isOn(std::string const& id) {
    auto f = findFeature(id);
    bool def = f ? f->defIndex != 0 : false;
    return Mod::get()->getSavedValue<bool>(id, def);
}

inline int getIndex(std::string const& id) {
    auto f = findFeature(id);
    int def = f ? f->defIndex : 0;
    int i = Mod::get()->getSavedValue<int>(id, def);
    if (f && !f->options.empty()) {
        if (i < 0 || i >= (int)f->options.size()) i = def;
    }
    return i;
}

// ---------- speed ----------
inline float g_speed = 1.f;

inline void applySpeed() {
    static float const values[] = {0.25f, 0.5f, 0.75f, 1.f, 1.5f, 2.f, 3.f};
    int i = getIndex("speed");
    g_speed = (i >= 0 && i < 7) ? values[i] : 1.f;
}

// ---------- customizers ----------
inline int trailLength() {
    static int const v[] = {20, 50, 100, 200, 400};
    int i = getIndex("trail-length");
    return v[(i >= 0 && i < 5) ? i : 2];
}

inline float labelScale() {
    static float const v[] = {0.5f, 0.75f, 1.f, 1.25f, 1.5f};
    int i = getIndex("label-scale");
    return v[(i >= 0 && i < 5) ? i : 2];
}

inline float labelOpacity() {
    static float const v[] = {0.25f, 0.5f, 0.75f, 1.f};
    int i = getIndex("label-opacity");
    return v[(i >= 0 && i < 4) ? i : 3];
}

inline ccColor3B labelColor() {
    static ccColor3B const v[] = {
        {255, 255, 255}, {80, 220, 255}, {90, 255, 120}, {255, 120, 220}, {255, 210, 80}
    };
    int i = getIndex("label-color");
    return v[(i >= 0 && i < 5) ? i : 0];
}

// ---------- CPS ----------
inline std::deque<std::chrono::steady_clock::time_point> g_clicks;

inline void registerClick() {
    g_clicks.push_back(std::chrono::steady_clock::now());
}

inline int cps() {
    auto now = std::chrono::steady_clock::now();
    while (!g_clicks.empty() &&
           std::chrono::duration<float>(now - g_clicks.front()).count() > 1.f) {
        g_clicks.pop_front();
    }
    return (int)g_clicks.size();
}

// ---------- macro bot ----------
struct MacroAction {
    int frame;
    bool down;
    int button;
    bool p1;
};

inline std::vector<MacroAction> g_macro;
inline int g_frame = 0;
inline size_t g_playIndex = 0;
inline bool g_injecting = false;

// 0 = Off, 1 = Record, 2 = Play
inline int macroMode() {
    return getIndex("macro-mode");
}

inline void saveMacro(int levelID) {
    auto dir = Mod::get()->getSaveDir() / "macros";
    std::error_code ec;
    std::filesystem::create_directories(dir, ec);
    std::ofstream out(dir / fmt::format("{}.vmacro", levelID));
    for (auto const& a : g_macro) {
        out << a.frame << ' ' << (a.down ? 1 : 0) << ' ' << a.button << ' ' << (a.p1 ? 1 : 0) << '\n';
    }
}

inline void loadMacro(int levelID) {
    g_macro.clear();
    std::ifstream in(Mod::get()->getSaveDir() / "macros" / fmt::format("{}.vmacro", levelID));
    int f, d, b, p;
    while (in >> f >> d >> b >> p) {
        g_macro.push_back({f, d != 0, b, p != 0});
    }
}

// ADD NEW FEATURES HERE: one entry each
inline std::vector<Feature> const& features() {
    static std::vector<Feature> list = {
        // ---------- Player ----------
        {"noclip", "Noclip", "Survive every hazard.", "Player", Kind::Toggle, {}, 0, nullptr},
        {"hide-player", "Hide Player", "Makes player 1 invisible.", "Player", Kind::Toggle, {}, 0, nullptr},
        {"speed", "Speedhack", "Change the game speed.", "Player", Kind::Cycle,
            {"0.25x", "0.5x", "0.75x", "1x", "1.5x", "2x", "3x"}, 3, []{ applySpeed(); }},

        // ---------- Hitbox ----------
        {"show-hitbox", "Show Hitboxes", "Draw hitboxes in levels.", "Hitbox", Kind::Toggle, {}, 0, nullptr},
        {"hitbox-trail", "Hitbox Trail", "Trail shows with hitboxes and on death.", "Hitbox", Kind::Toggle, {}, 1, nullptr},
        {"trail-length", "Trail Length", "How many frames the trail keeps.", "Hitbox", Kind::Cycle,
            {"20", "50", "100", "200", "400"}, 2, nullptr},

        // ---------- Labels ----------
        {"labels", "Labels", "Master switch for all labels.", "Labels", Kind::Toggle, {}, 1, nullptr},
        {"label-fps", "FPS Label", "Shows your FPS.", "Labels", Kind::Toggle, {}, 1, nullptr},
        {"label-cps", "CPS Label", "Shows clicks per second.", "Labels", Kind::Toggle, {}, 1, nullptr},
        {"label-time", "Time Label", "Shows time in the attempt.", "Labels", Kind::Toggle, {}, 0, nullptr},
        {"label-attempt", "Attempt Label", "Shows the attempt number.", "Labels", Kind::Toggle, {}, 0, nullptr},
        {"label-percent", "Percent Label", "Shows level percent.", "Labels", Kind::Toggle, {}, 0, nullptr},
        {"label-pos", "Label Position", "Which corner the labels sit in.", "Labels", Kind::Cycle,
            {"Top Left", "Top Right", "Bottom Left", "Bottom Right"}, 0, nullptr},
        {"label-scale", "Label Scale", "Size of the labels.", "Labels", Kind::Cycle,
            {"0.5x", "0.75x", "1x", "1.25x", "1.5x"}, 2, nullptr},
        {"label-opacity", "Label Opacity", "How see-through the labels are.", "Labels", Kind::Cycle,
            {"25%", "50%", "75%", "100%"}, 3, nullptr},
        {"label-color", "Label Color", "Color of the labels.", "Labels", Kind::Cycle,
            {"White", "Cyan", "Green", "Pink", "Gold"}, 0, nullptr},

        // ---------- Macro ----------
        {"macro-mode", "Macro Bot", "Record your run, then replay it.", "Macro", Kind::Cycle,
            {"Off", "Record", "Play"}, 0, nullptr},
        {"label-macro", "Macro Status", "Shows recording/playback status.", "Macro", Kind::Toggle, {}, 1, nullptr},

        // ---------- Global ----------
        {"cheat-indicator", "Cheat Indicator", "Dot: green = clean, red = cheating.", "Global", Kind::Toggle, {}, 1, nullptr},
        {"unlock-all", "Unlock All Icons", "Every icon and color unlocked.", "Global", Kind::Toggle, {}, 0, nullptr},
        {"fps", "Show Stats", "Shows the built-in stats counter.", "Global", Kind::Toggle, {}, 0,
            []{ CCDirector::get()->setDisplayStats(isOn("fps")); }},
    };
    return list;
}

inline std::vector<std::string> categories() {
    std::vector<std::string> out;
    for (auto const& f : features()) {
        bool found = false;
        for (auto const& c : out) if (c == f.category) found = true;
        if (!found) out.push_back(f.category);
    }
    return out;
}

inline void applyAll() {
    for (auto const& f : features()) {
        if (f.apply) f.apply();
    }
}

}
