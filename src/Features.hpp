#pragma once
#include <Geode/Geode.hpp>
#include <Geode/utils/file.hpp>
#include <chrono>
#include <cstdlib>
#include <deque>
#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <vector>

using namespace geode::prelude;

namespace vanta {

enum class Kind { Toggle, Cycle, Number, Color, Button };

struct Feature {
    std::string id;
    std::string name;
    std::string desc;
    std::string category;
    Kind kind = Kind::Toggle;
    std::vector<std::string> options; // button: options[0] = button label
    int defIndex = 0; // toggle: 1 = on | cycle: default option | color: packed 0xRRGGBB
    std::function<void()> apply;
    float defNum = 0.f; // number only
    float minNum = 0.f;
    float maxNum = 0.f;
    std::function<void()> action; // button only
};

inline std::vector<Feature> const& features();

inline bool g_booting = false;
inline void (*g_bgRefresh)() = nullptr;
inline void bgRefresh() {
    if (g_bgRefresh) g_bgRefresh();
}

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

inline float getNum(std::string const& id) {
    auto f = findFeature(id);
    float def = f ? f->defNum : 0.f;
    return Mod::get()->getSavedValue<float>(id, def);
}

inline void setNum(Feature const& f, float v) {
    if (v < f.minNum) v = f.minNum;
    if (v > f.maxNum) v = f.maxNum;
    Mod::get()->setSavedValue<float>(f.id, v);
}

inline ccColor4F colorOf(std::string const& id) {
    int c = getIndex(id);
    return ccColor4F{
        ((c >> 16) & 255) / 255.f,
        ((c >> 8) & 255) / 255.f,
        (c & 255) / 255.f,
        1.f
    };
}

// ---------- speed ----------
inline float g_speed = 1.f;

inline void applySpeed() {
    float v = getNum("speed-value");
    if (v < 0.05f) v = 0.05f;
    if (v > 10.f) v = 10.f;
    g_speed = v;
}

// ---------- FPS ----------
inline void applyFps() {
    float fps = getNum("fps-changer");
    if (fps < 1.f) return; // 0 = leave the game alone
    CCDirector::get()->setAnimationInterval(1.0 / fps);
}

// ---------- hitbox trail ----------
inline int trailLength() {
    int n = (int)getNum("trail-length-value");
    if (n < 1) n = 1;
    if (n > 2000) n = 2000;
    return n;
}

inline ccColor4F trailColor(bool p2) {
    auto c = colorOf("trail-color");
    return p2 ? ccColor4F{c.g, c.b, c.r, 1.f} : c;
}

// ---------- holding state (for hold trail) ----------
inline bool g_hold1 = false;
inline bool g_hold2 = false;

// ---------- label customizers ----------
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

// ---------- graphics overlays ----------
inline GLubyte dimAlpha() {
    static GLubyte const v[] = {0, 64, 128, 191};
    int i = getIndex("gfx-brightness");
    return v[(i >= 0 && i < 4) ? i : 0];
}

inline ccColor3B tintColor() {
    static ccColor3B const v[] = {
        {0, 0, 0}, {255, 150, 40}, {60, 140, 255}, {255, 90, 200}, {60, 255, 120}
    };
    int i = getIndex("gfx-tint");
    return v[(i >= 0 && i < 5) ? i : 0];
}

inline GLubyte tintAlpha() {
    return getIndex("gfx-tint") == 0 ? 0 : 50;
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

// ---------- texture packs ----------
inline std::filesystem::path packsDir() {
    auto d = Mod::get()->getConfigDir() / "packs";
    std::error_code ec;
    std::filesystem::create_directories(d, ec);
    return d;
}

inline std::vector<std::string> listPacks() {
    std::vector<std::string> out;
    std::error_code ec;
    for (auto const& e : std::filesystem::directory_iterator(packsDir(), ec)) {
        std::error_code ec2;
        if (e.is_directory(ec2)) out.push_back(e.path().filename().string());
    }
    return out;
}

inline bool packOn(std::string const& name) {
    return Mod::get()->getSavedValue<bool>("pack-" + name, false);
}

inline void addEnabledPacks() {
    auto fu = CCFileUtils::get();
    for (auto const& n : listPacks()) {
        std::string id = "vanta-pack-" + n;
        fu->removeTexturePack(id);
        if (packOn(n)) {
            CCTexturePack pack;
            pack.m_id = id;
            pack.m_paths.push_back((packsDir() / n).string());
            fu->addTexturePack(pack);
        }
    }
}

inline void reloadTextures() {
    addEnabledPacks();
    GameManager::get()->reloadAll(false, false, true);
}

// ADD NEW FEATURES HERE: one entry each
inline std::vector<Feature> const& features() {
    static std::vector<Feature> list = {
        // ---------- Player ----------
        {"noclip", "Noclip", "Survive every hazard.", "Player", Kind::Toggle, {}, 0, nullptr},
        {"hide-player", "Hide Player", "Makes player 1 invisible.", "Player", Kind::Toggle, {}, 0, nullptr},

        // ---------- Hitbox ----------
        {"show-hitbox", "Show Hitboxes", "Draw hitboxes in levels.", "Hitbox", Kind::Toggle, {}, 0, nullptr},
        {"hitbox-trail", "Hitbox Trail", "Shows with hitboxes and on death.", "Hitbox", Kind::Toggle, {}, 1, nullptr},
        {"trail-length-value", "Trail Length", "Frames the trail keeps (1-2000).", "Hitbox", Kind::Number, {}, 0, nullptr,
            100.f, 1.f, 2000.f},
        {"trail-color", "Trail Color", "Normal trail color.", "Hitbox", Kind::Color, {}, 0x4DE6FF, nullptr},
        {"trail-inside", "Inside Trail", "Fills the inside of the trail.", "Hitbox", Kind::Toggle, {}, 1, nullptr},
        {"trail-color-inside", "Inside Trail Color", "Color of the inside fill.", "Hitbox", Kind::Color, {}, 0xFF2828, nullptr},
        {"trail-hold", "Hold Trail", "Trail changes color while holding.", "Hitbox", Kind::Toggle, {}, 1, nullptr},
        {"trail-color-hold", "Hold Trail Color", "Color used while holding.", "Hitbox", Kind::Color, {}, 0x32FF50, nullptr},

        // ---------- Labels ----------
        {"labels", "Labels", "Master switch for all labels.", "Labels", Kind::Toggle, {}, 1, nullptr},
        {"label-fps", "FPS Label", "Shows your FPS.", "Labels", Kind::Toggle, {}, 1, nullptr},
        {"label-cps", "CPS Label", "Shows clicks per second.", "Labels", Kind::Toggle, {}, 1, nullptr},
        {"label-time", "Time Label", "Shows time in the attempt.", "Labels", Kind::Toggle, {}, 0, nullptr},
        {"label-attempt", "Attempt Label", "Shows the attempt number.", "Labels", Kind::Toggle, {}, 0, nullptr},
        {"label-percent", "Percent Label", "Shows level percent.", "Labels", Kind::Toggle, {}, 0, nullptr},
        {"label-nc-acc", "Noclip Accuracy", "Percent of frames not hitting.", "Labels", Kind::Toggle, {}, 1, nullptr},
        {"label-nc-deaths", "Noclip Deaths", "Times you would have died.", "Labels", Kind::Toggle, {}, 1, nullptr},
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
        {"speed-value", "Speed Changer", "Game speed, type any number.", "Macro", Kind::Number, {}, 0,
            []{ applySpeed(); }, 1.f, 0.05f, 10.f},

        // ---------- Graphics ----------
        {"gfx-ldm", "Low Detail Mode", "Turns the game's LDM on or off.", "Graphics", Kind::Toggle, {}, 0,
            []{
                if (g_booting) return;
                GameManager::get()->setGameVariable("0042", isOn("gfx-ldm"));
            }},
        {"auto-ldm", "Auto LDM", "LDM on for levels that support it.", "Graphics", Kind::Toggle, {}, 0, nullptr},
        {"gfx-brightness", "Brightness", "Darken the level view.", "Graphics", Kind::Cycle,
            {"Normal", "-25%", "-50%", "-75%"}, 0, nullptr},
        {"gfx-tint", "Screen Tint", "Color filter over the level.", "Graphics", Kind::Cycle,
            {"None", "Warm", "Cool", "Pink", "Green"}, 0, nullptr},
        {"fps-changer", "FPS Changer", "Type your FPS (0 = off).", "Graphics", Kind::Number, {}, 0,
            []{ applyFps(); }, 0.f, 0.f, 1000.f},
        {"fps", "Show Stats", "Shows the built-in stats counter.", "Graphics", Kind::Toggle, {}, 0,
            []{ CCDirector::get()->setDisplayStats(isOn("fps")); }},

        // ---------- Background ----------
        {"bg-theme", "Menu Background", "Blossom = cherry blossom.", "Background", Kind::Cycle,
            {"None", "Fire", "Winter", "Spring", "Summer", "Blossom", "Aurora", "Stars", "Rain"}, 0,
            []{ bgRefresh(); }},
        {"bg-speed", "Animation Speed", "How fast the background moves.", "Background", Kind::Cycle,
            {"0.5x", "1x", "2x"}, 1, []{ bgRefresh(); }},
        {"bg-density", "Effect Density", "How many particles are drawn.", "Background", Kind::Cycle,
            {"Low", "Medium", "High"}, 1, []{ bgRefresh(); }},

        // ---------- Textures (pack rows are added by the menu) ----------
        {"tex-open", "Open Packs Folder", "Put texture pack folders in here.", "Textures", Kind::Button,
            {"Open"}, 0, nullptr, 0.f, 0.f, 0.f,
            []{ file::openFolder(packsDir()); }},
        {"tex-reload", "Reload Textures", "Applies the packs you enabled.", "Textures", Kind::Button,
            {"Reload"}, 0, nullptr, 0.f, 0.f, 0.f,
            []{ reloadTextures(); }},

        // ---------- Global ----------
        {"cheat-indicator", "Cheat Indicator", "Circle: green = clean, red = cheating.", "Global", Kind::Toggle, {}, 1, nullptr},
        {"disable-cheats", "Disable All Cheats", "Noclip, speed, macro, unlock all.", "Global", Kind::Button,
            {"Disable"}, 0, nullptr, 0.f, 0.f, 0.f,
            []{
                auto m = Mod::get();
                m->setSavedValue<bool>("noclip", false);
                m->setSavedValue<bool>("unlock-all", false);
                m->setSavedValue<int>("macro-mode", 0);
                m->setSavedValue<float>("speed-value", 1.f);
                applySpeed();
            }},
        {"no-newbest-orbs", "No New Best Orbs", "Hides the orb reward on new best.", "Global", Kind::Toggle, {}, 0, nullptr},
        {"unlock-all", "Unlock All Icons", "Every icon and color unlocked.", "Global", Kind::Toggle, {}, 0, nullptr},
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
    g_booting = true;
    for (auto const& f : features()) {
        if (f.apply) f.apply();
    }
    g_booting = false;
}

}
