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
    std::vector<std::string> options;
    int defIndex = 0;
    std::function<void()> apply;
    float defNum = 0.f;
    float minNum = 0.f;
    float maxNum = 0.f;
    std::function<void()> action;
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

// ---------- speed / fps ----------
inline float g_speed = 1.f;

inline void applySpeed() {
    float v = getNum("speed-value");
    if (v < 0.05f) v = 0.05f;
    if (v > 10.f) v = 10.f;
    g_speed = v;
}

inline void applyFps() {
    float fps = getNum("fps-changer");
    if (fps < 1.f) return;
    CCDirector::get()->setAnimationInterval(1.0 / fps);
}

// ---------- respawn ----------
inline float respawnTime() {
    float v = getNum("respawn-time");
    if (v < 0.f) v = 0.f;
    if (v > 10.f) v = 10.f;
    return v;
}

// ---------- hitbox / trail ----------
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

inline float hbThick() {
    static float const v[] = {1.2f, 1.8f, 2.8f};
    int i = getIndex("hb-thick");
    return v[(i >= 0 && i < 3) ? i : 1];
}

inline float hbFill() {
    static float const v[] = {0.f, 0.15f, 0.3f, 0.5f};
    int i = getIndex("hb-fill");
    return v[(i >= 0 && i < 4) ? i : 1];
}

inline bool g_hold1 = false;
inline bool g_hold2 = false;
inline bool g_click1 = false;
inline bool g_click2 = false;
inline float g_anim = 0.f;

// ---------- labels ----------
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

// ---------- brightness ----------
inline GLubyte dimAlpha() {
    static GLubyte const v[] = {0, 64, 128, 191};
    int i = getIndex("gfx-brightness");
    return v[(i >= 0 && i < 4) ? i : 0];
}

// ---------- building opacity ----------
inline float objOpacity() {
    float v = getNum("obj-opacity-pct");
    if (v < 0.f) v = 0.f;
    if (v > 100.f) v = 100.f;
    return v / 100.f;
}

// which level objects the Building Opacity feature touches
inline bool objAffects(GameObjectType t) {
    int tg = getIndex("obj-target");
    bool solid = (t == GameObjectType::Solid || t == GameObjectType::Slope);
    bool deco = (t == GameObjectType::Decoration);
    bool hazard = (t == GameObjectType::Hazard);
    if (tg == 0) return solid;
    if (tg == 1) return deco;
    if (tg == 2) return solid || deco;
    return !hazard;
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

}
