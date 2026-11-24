#pragma once
#include "Core.hpp"

namespace vanta {

// ADD NEW FEATURES HERE: one entry each
inline std::vector<Feature> const& features() {
    static std::vector<Feature> list = {
        // ---------- Player ----------
        {"noclip", "Noclip", "Survive every hazard.", "Player", Kind::Toggle, {}, 0, nullptr},
        {"hide-player", "Hide Player", "Makes player 1 invisible.", "Player", Kind::Toggle, {}, 0, nullptr},
        {"respawn-custom", "Custom Respawn", "Use your own respawn delay.", "Player", Kind::Toggle, {}, 0, nullptr},
        {"respawn-time", "Respawn Time", "Seconds after death (0 = instant).", "Player", Kind::Number, {}, 0, nullptr,
            1.f, 0.f, 10.f},
        {"startpos", "StartPos Switcher", "Arrows to switch Start Pos.", "Player", Kind::Toggle, {}, 0, nullptr},

        // ---------- Hitbox ----------
        {"show-hitbox", "Show Hitboxes", "Master switch: nothing below shows without it.", "Hitbox", Kind::Toggle, {}, 0, nullptr},
        {"hb-objects", "Object Hitboxes", "Styled boxes: blue blocks, red hazards, green orbs.", "Hitbox", Kind::Toggle, {}, 1, nullptr},
        {"hb-player", "Player Hitbox", "Styled player box (needs Show Hitboxes).", "Hitbox", Kind::Toggle, {}, 1, nullptr},
        {"hb-style", "Hitbox Style", "Classic = red outer, blue inner box.", "Hitbox", Kind::Cycle,
            {"Classic", "Neon", "Glass"}, 0, nullptr},
        {"hb-thick", "Outline Thickness", "How thick the lines are.", "Hitbox", Kind::Cycle,
            {"Thin", "Normal", "Thick"}, 1, nullptr},
        {"hb-fill", "Fill Opacity", "Fill inside the player box.", "Hitbox", Kind::Cycle,
            {"0%", "15%", "30%", "50%"}, 1, nullptr},
        {"hb-color", "Hitbox Color", "Color for Neon and Glass styles.", "Hitbox", Kind::Color, {}, 0xFF3B5C, nullptr},
        {"hitbox-trail", "Hitbox Trail", "Needs Show Hitboxes.", "Hitbox", Kind::Toggle, {}, 1, nullptr},
        {"trail-on-death", "Trail On Death", "Also show the trail after you die.", "Hitbox", Kind::Toggle, {}, 0, nullptr},
        {"trail-style", "Trail Style", "Boxes, Neon or Ghost.", "Hitbox", Kind::Cycle,
            {"Boxes", "Neon", "Ghost"}, 1, nullptr},
        {"trail-smart", "Smart Spacing", "Skips overlapping boxes so the trail stays clean.", "Hitbox", Kind::Toggle, {}, 1, nullptr},
        {"trail-length-value", "Trail Length", "Frames the trail keeps (1-2000).", "Hitbox", Kind::Number, {}, 0, nullptr,
            100.f, 1.f, 2000.f},
        {"trail-color", "Trail Color", "Normal trail color.", "Hitbox", Kind::Color, {}, 0x4DE6FF, nullptr},
        {"trail-markers", "Click Markers", "Green dot = press, red dot = release.", "Hitbox", Kind::Toggle, {}, 1, nullptr},
        {"trail-ticks", "Frame Ticks", "Tick every 10 frames, tall tick every 60.", "Hitbox", Kind::Toggle, {}, 1, nullptr},
        {"trail-deathmark", "Death Marker", "Highlights the object that killed you.", "Hitbox", Kind::Toggle, {}, 1, nullptr},
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
        {"obj-opacity", "Building Opacity", "Fades blocks and decoration (hazards stay).", "Graphics", Kind::Cycle,
            {"100%", "75%", "50%", "25%", "10%"}, 0, nullptr},
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
        {"menu-lock", "Lock Menu Position", "Stops the menu window from being dragged.", "Global", Kind::Toggle, {}, 0, nullptr},
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
