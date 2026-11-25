#pragma once
#include "Features.hpp"
#include <algorithm>

class VantaPopup : public geode::Popup {
protected:
    ScrollLayer* m_scroll = nullptr;
    CCPoint m_origin = ccp(0.f, 0.f);
    CCPoint m_lastTouch = ccp(0.f, 0.f);
    bool m_drag = false;
    CCLabelBMFont* m_catLabel = nullptr;
    std::string m_cat;

    bool init() {
        if (!Popup::init(480.f, 250.f)) return false;
        this->setTitle("VantaMenu");

        // restore the position the menu was left in
        m_origin = m_mainLayer->getPosition();
        this->applyOffset(
            Mod::get()->getSavedValue<float>("menu-dx", 0.f),
            Mod::get()->getSavedValue<float>("menu-dy", 0.f)
        );

        auto cats = vanta::categories();
        m_cat = cats.empty() ? "" : cats[0];

        auto tabs = CCMenu::create();
        tabs->setContentSize({m_size.width - 20.f, 28.f});
        tabs->setPosition({m_size.width / 2.f, m_size.height - 50.f});
        tabs->setLayout(RowLayout::create()->setGap(3.f));
        for (auto const& cat : cats) {
            auto spr = ButtonSprite::create(cat.c_str(), 52, true, "bigFont.fnt", "GJ_button_02.png", 26.f, .38f);
            auto btn = CCMenuItemExt::createSpriteExtra(spr, [this, cat](CCMenuItemSpriteExtra*) {
                m_cat = cat;
                this->rebuild();
            });
            tabs->addChild(btn);
        }
        tabs->updateLayout();
        m_mainLayer->addChild(tabs);

        m_catLabel = CCLabelBMFont::create("", "goldFont.fnt");
        m_catLabel->setScale(.5f);
        m_catLabel->setPosition({m_size.width / 2.f, m_size.height - 76.f});
        m_mainLayer->addChild(m_catLabel);

        this->rebuild();
        return true;
    }

    void rebuild() {
        if (m_scroll) m_scroll->removeFromParent();
        m_catLabel->setString(m_cat.c_str());

        float w = m_size.width - 40.f;
        float h = m_size.height - 105.f;

        m_scroll = ScrollLayer::create({w, h});
        m_scroll->setPosition({20.f, 14.f});
        m_scroll->m_contentLayer->setLayout(
            ColumnLayout::create()
                ->setAxisReverse(true)
                ->setAxisAlignment(AxisAlignment::End)
                ->setAutoGrowAxis(h)
                ->setGap(6.f)
        );

        for (auto const& f : vanta::features()) {
            if (f.category != m_cat) continue;
            m_scroll->m_contentLayer->addChild(makeRow(f, w));
        }

        if (m_cat == "Textures") {
            auto packs = vanta::listPacks();
            if (packs.empty()) {
                m_scroll->m_contentLayer->addChild(
                    baseRow("No packs found", "Open the packs folder and add a pack folder.", w)
                );
            } else {
                for (auto const& p : packs) {
                    m_scroll->m_contentLayer->addChild(makePackRow(p, w));
                }
            }
        }

        m_scroll->m_contentLayer->updateLayout();
        m_mainLayer->addChild(m_scroll);
    }

    static std::string numText(float v) {
        if (v <= 0.f) return "";
        return fmt::format("{}", v);
    }

    CCNode* baseRow(std::string const& n, std::string const& d, float w) {
        auto row = CCNode::create();
        row->setContentSize({w, 38.f});

        auto bg = CCScale9Sprite::create("square02b_001.png");
        bg->setContentSize({w, 38.f});
        bg->setColor({18, 18, 28});
        bg->setOpacity(210);
        bg->setPosition({w / 2.f, 19.f});
        row->addChild(bg);

        auto name = CCLabelBMFont::create(n.c_str(), "bigFont.fnt");
        name->setScale(.45f);
        name->setAnchorPoint({0.f, .5f});
        name->setPosition({10.f, 26.f});
        row->addChild(name);

        auto desc = CCLabelBMFont::create(d.c_str(), "chatFont.fnt");
        desc->setScale(.55f);
        desc->setColor({150, 150, 170});
        desc->setAnchorPoint({0.f, .5f});
        desc->setPosition({10.f, 11.f});
        row->addChild(desc);

        return row;
    }

    CCNode* makePackRow(std::string const& pack, float w) {
        auto row = baseRow(pack, "Enable it, then press Reload Textures.", w);

        auto menu = CCMenu::create();
        menu->setPosition({0.f, 0.f});

        auto toggle = CCMenuItemExt::createTogglerWithStandardSprites(.6f, [pack](CCMenuItemToggler* t) {
            Mod::get()->setSavedValue<bool>("pack-" + pack, !t->isToggled());
            vanta::addEnabledPacks();
        });
        toggle->toggle(vanta::packOn(pack));
        toggle->setPosition({w - 26.f, 19.f});
        menu->addChild(toggle);

        row->addChild(menu);
        return row;
    }

    CCNode* makeRow(vanta::Feature const& f, float w) {
        auto row = baseRow(f.name, f.desc, w);

        auto menu = CCMenu::create();
        menu->setPosition({0.f, 0.f});

        if (f.kind == vanta::Kind::Toggle) {
            auto toggle = CCMenuItemExt::createTogglerWithStandardSprites(.6f, [f](CCMenuItemToggler* t) {
                Mod::get()->setSavedValue<bool>(f.id, !t->isToggled());
                if (f.apply) f.apply();
            });
            toggle->toggle(vanta::isOn(f.id));
            toggle->setPosition({w - 26.f, 19.f});
            menu->addChild(toggle);
        }
        else if (f.kind == vanta::Kind::Cycle) {
            int idx = vanta::getIndex(f.id);
            auto spr = ButtonSprite::create(f.options[idx].c_str(), 80, true, "bigFont.fnt", "GJ_button_04.png", 24.f, .5f);
            auto btn = CCMenuItemExt::createSpriteExtra(spr, [f, spr](CCMenuItemSpriteExtra*) {
                int cur = vanta::getIndex(f.id);
                cur = (cur + 1) % (int)f.options.size();
                Mod::get()->setSavedValue<int>(f.id, cur);
                spr->setString(f.options[cur].c_str());
                if (f.apply) f.apply();
            });
            btn->setPosition({w - 55.f, 19.f});
            menu->addChild(btn);
        }
        else if (f.kind == vanta::Kind::Number) {
            auto input = TextInput::create(75.f, "off", "bigFont.fnt");
            input->setCommonFilter(CommonFilter::Float);
            input->setScale(.8f);
            input->setPosition({w - 52.f, 19.f});
            input->setString(numText(vanta::getNum(f.id)), false);
            input->setCallback([f](std::string const& s) {
                if (s.empty()) return;
                float v = (float)std::strtod(s.c_str(), nullptr);
                vanta::setNum(f, v);
                if (f.apply) f.apply();
            });
            row->addChild(input);
        }
        else if (f.kind == vanta::Kind::Color) {
            int packed = vanta::getIndex(f.id);
            auto swatch = CCScale9Sprite::create("square02b_001.png");
            swatch->setContentSize({22.f, 22.f});
            swatch->setColor({
                (GLubyte)((packed >> 16) & 255),
                (GLubyte)((packed >> 8) & 255),
                (GLubyte)(packed & 255)
            });
            swatch->setPosition({w - 100.f, 19.f});
            row->addChild(swatch);

            auto spr = ButtonSprite::create("Pick", 50, true, "bigFont.fnt", "GJ_button_04.png", 24.f, .5f);
            auto btn = CCMenuItemExt::createSpriteExtra(spr, [f, swatch](CCMenuItemSpriteExtra*) {
                int cur = vanta::getIndex(f.id);
                ccColor4B start = {
                    (GLubyte)((cur >> 16) & 255),
                    (GLubyte)((cur >> 8) & 255),
                    (GLubyte)(cur & 255),
                    255
                };
                auto popup = ColorPickPopup::create(start);
                popup->setCallback([f, swatch](ccColor4B const& c) {
                    int p = (c.r << 16) | (c.g << 8) | c.b;
                    Mod::get()->setSavedValue<int>(f.id, p);
                    swatch->setColor({c.r, c.g, c.b});
                });
                popup->show();
            });
            btn->setPosition({w - 45.f, 19.f});
            menu->addChild(btn);
        }
        else if (f.kind == vanta::Kind::Button) {
            std::string label = f.options.empty() ? "Run" : f.options[0];
            auto spr = ButtonSprite::create(label.c_str(), 70, true, "bigFont.fnt", "GJ_button_06.png", 24.f, .5f);
            auto btn = CCMenuItemExt::createSpriteExtra(spr, [this, f](CCMenuItemSpriteExtra*) {
                if (f.action) f.action();
                Notification::create("Done!", NotificationIcon::Success)->show();
                Loader::get()->queueInMainThread([self = Ref<VantaPopup>(this)] {
                    self->rebuild();
                });
            });
            btn->setPosition({w - 50.f, 19.f});
            menu->addChild(btn);
        }

        row->addChild(menu);
        return row;
    }

    void applyOffset(float dx, float dy) {
        auto win = CCDirector::get()->getWinSize();
        float limX = win.width / 2.f + m_size.width / 2.f - 80.f;
        float limY = win.height / 2.f + m_size.height / 2.f - 60.f;
        dx = std::max(-limX, std::min(limX, dx));
        dy = std::max(-limY, std::min(limY, dy));
        m_mainLayer->setPosition(ccp(m_origin.x + dx, m_origin.y + dy));
    }

    void finishDrag() {
        if (!m_drag) return;
        m_drag = false;
        auto p = m_mainLayer->getPosition();
        this->applyOffset(p.x - m_origin.x, p.y - m_origin.y);
        auto q = m_mainLayer->getPosition();
        Mod::get()->setSavedValue<float>("menu-dx", q.x - m_origin.x);
        Mod::get()->setSavedValue<float>("menu-dy", q.y - m_origin.y);
    }

    bool ccTouchBegan(CCTouch* t, CCEvent* e) override {
        if (!vanta::isOn("menu-lock")) {
            CCPoint local = m_mainLayer->convertToNodeSpace(t->getLocation());
            if (local.x >= 0.f && local.x <= m_size.width && local.y >= m_size.height - 34.f && local.y <= m_size.height) {
                m_drag = true;
                m_lastTouch = t->getLocation();
            }
        }
        return Popup::ccTouchBegan(t, e);
    }

    void ccTouchMoved(CCTouch* t, CCEvent* e) override {
        if (m_drag) {
            CCPoint now = t->getLocation();
            CCPoint pos = m_mainLayer->getPosition();
            m_mainLayer->setPosition(ccp(pos.x + (now.x - m_lastTouch.x), pos.y + (now.y - m_lastTouch.y)));
            m_lastTouch = now;
        }
        Popup::ccTouchMoved(t, e);
    }

    void ccTouchEnded(CCTouch* t, CCEvent* e) override {
        this->finishDrag();
        Popup::ccTouchEnded(t, e);
    }

    void ccTouchCancelled(CCTouch* t, CCEvent* e) override {
        this->finishDrag();
        Popup::ccTouchCancelled(t, e);
    }

public:
    static VantaPopup* create() {
        auto ret = new VantaPopup();
        if (ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};

// The Vanta button: drag it anywhere, it remembers where you left it.
class VantaFab : public CCLayer {
protected:
    ButtonSprite* m_spr = nullptr;
    CCPoint m_start = ccp(0.f, 0.f);
    bool m_down = false;
    bool m_moved = false;

    CCPoint clampPos(CCPoint p) {
        auto win = CCDirector::get()->getWinSize();
        p.x = std::max(25.f, std::min(win.width - 25.f, p.x));
        p.y = std::max(18.f, std::min(win.height - 18.f, p.y));
        return p;
    }

    void savePos() {
        auto win = CCDirector::get()->getWinSize();
        auto p = this->getPosition();
        Mod::get()->setSavedValue<float>("fab-fx", p.x / win.width);
        Mod::get()->setSavedValue<float>("fab-fy", p.y / win.height);
    }

    bool init() override {
        if (!CCLayer::init()) return false;

        m_spr = ButtonSprite::create("Vanta", 60, true, "goldFont.fnt", "GJ_button_01.png", 28.f, .7f);
        this->addChild(m_spr);

        auto win = CCDirector::get()->getWinSize();
        float fx = Mod::get()->getSavedValue<float>("fab-fx", 50.f / win.width);
        float fy = Mod::get()->getSavedValue<float>("fab-fy", 1.f - 25.f / win.height);
        this->setPosition(this->clampPos(ccp(fx * win.width, fy * win.height)));

        this->setTouchMode(kCCTouchesOneByOne);
        this->setTouchPriority(-129);
        this->setTouchEnabled(true);
        return true;
    }

    bool ccTouchBegan(CCTouch* t, CCEvent*) override {
        if (!this->isVisible()) return false;
        CCPoint local = m_spr->convertToNodeSpace(t->getLocation());
        CCSize cs = m_spr->getContentSize();
        if (local.x < 0.f || local.x > cs.width || local.y < 0.f || local.y > cs.height) return false;
        m_down = true;
        m_moved = false;
        m_start = t->getLocation();
        m_spr->setScale(0.9f);
        return true;
    }

    void ccTouchMoved(CCTouch* t, CCEvent*) override {
        if (!m_down) return;
        CCPoint now = t->getLocation();
        float dx = now.x - m_start.x;
        float dy = now.y - m_start.y;
        if (!m_moved && dx * dx + dy * dy > 100.f) {
            m_moved = true;
        }
        if (m_moved && !vanta::isOn("fab-lock")) {
            this->setPosition(this->clampPos(now));
        }
    }

    void ccTouchEnded(CCTouch*, CCEvent*) override {
        if (!m_down) return;
        m_down = false;
        m_spr->setScale(1.f);
        if (m_moved) {
            if (!vanta::isOn("fab-lock")) this->savePos();
        } else {
            if (auto p = VantaPopup::create()) p->show();
        }
    }

    void ccTouchCancelled(CCTouch*, CCEvent*) override {
        m_down = false;
        m_spr->setScale(1.f);
    }

public:
    static VantaFab* create() {
        auto ret = new VantaFab();
        if (ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};

inline void addVantaButton(CCNode* parent) {
    if (auto fab = VantaFab::create()) {
        parent->addChild(fab, 100);
    }
}
