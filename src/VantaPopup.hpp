#pragma once
#include "Features.hpp"

class VantaPopup : public geode::Popup {
protected:
    ScrollLayer* m_scroll = nullptr;
    CCLabelBMFont* m_catLabel = nullptr;
    std::string m_cat;

    bool init() {
        if (!Popup::init(380.f, 250.f)) return false;
        this->setTitle("VantaMenu");

        auto cats = vanta::categories();
        m_cat = cats.empty() ? "" : cats[0];

        auto tabs = CCMenu::create();
        tabs->setContentSize({m_size.width - 30.f, 28.f});
        tabs->setPosition({m_size.width / 2.f, m_size.height - 50.f});
        tabs->setLayout(RowLayout::create()->setGap(6.f));
        for (auto const& cat : cats) {
            auto spr = ButtonSprite::create(cat.c_str(), 80, true, "bigFont.fnt", "GJ_button_02.png", 26.f, .55f);
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
        m_scroll->m_contentLayer->updateLayout();
        m_mainLayer->addChild(m_scroll);
    }

    CCNode* makeRow(vanta::Feature const& f, float w) {
        auto row = CCNode::create();
        row->setContentSize({w, 38.f});

        auto bg = CCScale9Sprite::create("square02b_001.png");
        bg->setContentSize({w, 38.f});
        bg->setColor({18, 18, 28});
        bg->setOpacity(210);
        bg->setPosition({w / 2.f, 19.f});
        row->addChild(bg);

        auto name = CCLabelBMFont::create(f.name.c_str(), "bigFont.fnt");
        name->setScale(.45f);
        name->setAnchorPoint({0.f, .5f});
        name->setPosition({10.f, 26.f});
        row->addChild(name);

        auto desc = CCLabelBMFont::create(f.desc.c_str(), "chatFont.fnt");
        desc->setScale(.55f);
        desc->setColor({150, 150, 170});
        desc->setAnchorPoint({0.f, .5f});
        desc->setPosition({10.f, 11.f});
        row->addChild(desc);

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
        } else {
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

        row->addChild(menu);
        return row;
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

inline void addVantaButton(CCNode* parent) {
    auto menu = CCMenu::create();
    auto spr = ButtonSprite::create("Vanta", 60, true, "goldFont.fnt", "GJ_button_01.png", 28.f, .7f);
    auto btn = CCMenuItemExt::createSpriteExtra(spr, [](CCMenuItemSpriteExtra*) {
        if (auto p = VantaPopup::create()) p->show();
    });
    menu->addChild(btn);
    auto win = CCDirector::get()->getWinSize();
    menu->setPosition({50.f, win.height - 25.f});
    parent->addChild(menu, 100);
}
