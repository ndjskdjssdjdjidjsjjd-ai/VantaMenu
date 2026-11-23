#pragma once
#include <Geode/Geode.hpp>
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
    int defIndex = 0;
    std::function<void()> apply;
};

inline float g_speed = 1.f;

inline bool isOn(std::string const& id) {
    return Mod::get()->getSavedValue<bool>(id, false);
}

inline int getIndex(std::string const& id, int def) {
    return Mod::get()->getSavedValue<int>(id, def);
}

inline void applySpeed() {
    static float const values[] = {0.25f, 0.5f, 0.75f, 1.f, 1.5f, 2.f, 3.f};
    int i = getIndex("speed", 3);
    g_speed = (i >= 0 && i < 7) ? values[i] : 1.f;
}

// ADD NEW FEATURES HERE: one line each
inline std::vector<Feature> const& features() {
    static std::vector<Feature> list = {
        {"noclip", "Noclip", "Survive every hazard.", "Player", Kind::Toggle, {}, 0, nullptr},
        {"hide-player", "Hide Player", "Makes player 1 invisible.", "Player", Kind::Toggle, {}, 0, nullptr},
        {"speed", "Speedhack", "Change the game speed.", "Player", Kind::Cycle,
            {"0.25x", "0.5x", "0.75x", "1x", "1.5x", "2x", "3x"}, 3, []{ applySpeed(); }},
        {"unlock-all", "Unlock All Icons", "Every icon and color unlocked.", "Global", Kind::Toggle, {}, 0, nullptr},
        {"fps", "Show FPS", "Shows the FPS counter.", "Global", Kind::Toggle, {}, 0,
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
