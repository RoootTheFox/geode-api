#include "ModListView.hpp"
#include "../info/ModInfoLayer.hpp"
#include <utils/WackyGeodeMacros>

ModCell::ModCell(const char* name, CCSize size) :
    TableViewCell(name, size.width, size.height) {}

void ModCell::draw() {
    reinterpret_cast<StatsCell*>(this)->StatsCell::draw();
}

void ModCell::loadFromMod(ModObject* Mod) {
    this->m_mod = Mod->m_mod;

    this->m_mainLayer->setVisible(true);
    this->m_backgroundLayer->setOpacity(255);
    
    auto menu = CCMenu::create();
    menu->setPosition(this->m_width - this->m_height, this->m_height / 2);
    this->m_mainLayer->addChild(menu);

    auto titleLabel = CCLabelBMFont::create(
        this->m_mod->getName().c_str(), "bigFont.fnt"
    );
    titleLabel->setAnchorPoint({ .0f, .5f });
    titleLabel->setScale(.5f);
    titleLabel->setPosition(this->m_height / 2, this->m_height / 2 + 7.f);
    this->m_mainLayer->addChild(titleLabel);
    
    auto creatorStr = "by " + this->m_mod->getDeveloper();
    auto creatorLabel = CCLabelBMFont::create(
        creatorStr.c_str(), "goldFont.fnt"
    );
    creatorLabel->setAnchorPoint({ .0f, .5f });
    creatorLabel->setScale(.43f);
    creatorLabel->setPosition(this->m_height / 2, this->m_height / 2 - 7.f);
    this->m_mainLayer->addChild(creatorLabel);

    auto viewSpr = ButtonSprite::create(
        "View", 0, 0, "bigFont.fnt", "GJ_button_01.png", 0, .8f
    );
    viewSpr->setScale(.65f);

    auto viewBtn = CCMenuItemSpriteExtra::create(
        viewSpr, this, menu_selector(ModCell::onInfo)
    );
    menu->addChild(viewBtn);

    if (this->m_mod->supportsDisabling()) {
        this->m_enableToggle = CCMenuItemToggler::createWithStandardSprites(
            this, menu_selector(ModCell::onEnable), .7f
        );
        this->m_enableToggle->setPosition(-50.f, 0.f);
        menu->addChild(this->m_enableToggle);
    }

    auto exMark = CCSprite::createWithSpriteFrameName("exMark_001.png");
    exMark->setScale(.5f);

    this->m_unresolvedExMark = CCMenuItemSpriteExtra::create(
        exMark, this, menu_selector(ModCell::onUnresolvedInfo)
    );
    this->m_unresolvedExMark->setPosition(-80.f, 0.f);
    menu->addChild(this->m_unresolvedExMark);

    this->updateState();
}

void ModCell::onInfo(CCObject*) {
    ModInfoLayer::create(this->m_mod)->show();
}

void ModCell::updateBGColor(int index) {
	if (index & 1) m_backgroundLayer->setColor(ccc3(0xc2, 0x72, 0x3e));
    else m_backgroundLayer->setColor(ccc3(0xa1, 0x58, 0x2c));
    m_backgroundLayer->setOpacity(0xff);
}

void ModCell::onEnable(CCObject* pSender) {
    if (!as<CCMenuItemToggler*>(pSender)->isToggled()) {
        auto res = this->m_mod->enable();
        if (!res) {
            FLAlertLayer::create(
                nullptr,
                "Error Enabling Mod",
                res.error(),
                "OK", nullptr
            )->show();
        }
    } else {
        auto res = this->m_mod->disable();
        if (!res) {
            FLAlertLayer::create(
                nullptr,
                "Error Disabling Mod",
                res.error(),
                "OK", nullptr
            )->show();
        }
    }
    this->m_list->updateAllStates(this);
}

void ModCell::onUnresolvedInfo(CCObject* pSender) {
    std::string info = "This mod has the following <cr>unresolved dependencies</c>: ";
    for (auto const& dep : this->m_mod->getUnresolvedDependencies()) {
        info += "<cg>" + dep.m_id + "</c> (<cy>" + dep.m_version.toString() + "</c>), ";
    }
    info.pop_back();
    info.pop_back();
    FLAlertLayer::create(
        nullptr,
        "Unresolved Dependencies",
        info,
        "OK", nullptr,
        400.f 
    )->show();
}

bool ModCell::init(ModListView* list) {
    this->m_list = list;
    return true;
}

void ModCell::updateState(bool invert) {
    bool unresolved = this->m_mod->hasUnresolvedDependencies();
    if (this->m_enableToggle) {
        this->m_enableToggle->toggle(this->m_mod->isEnabled() ^ invert);
        this->m_enableToggle->setEnabled(!unresolved);
        this->m_enableToggle->m_offButton->setOpacity(unresolved ? 100 : 255);
        this->m_enableToggle->m_offButton->setColor(unresolved ? cc3x(155) : cc3x(255));
        this->m_enableToggle->m_onButton->setOpacity(unresolved ? 100 : 255);
        this->m_enableToggle->m_onButton->setColor(unresolved ? cc3x(155) : cc3x(255));
    }

    this->m_unresolvedExMark->setVisible(unresolved);
}

ModCell* ModCell::create(ModListView* list, const char* key, CCSize size) {
    auto pRet = new ModCell(key, size);
    if (pRet && pRet->init(list)) {
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}


void ModListView::updateAllStates(ModCell* toggled) {
    CCARRAY_FOREACH_B_TYPE(this->m_tableView->m_cellArray, cell, ModCell) {
        cell->updateState(toggled == cell);
    }
}

void ModListView::setupList() {
    this->m_itemSeparation = 40.0f;

    if (!this->m_entries->count()) return;

    this->m_tableView->reloadData();

    if (this->m_entries->count() == 1)
        this->m_tableView->moveToTopWithOffset(this->m_itemSeparation);
    
    this->m_tableView->moveToTop();
}

TableViewCell* ModListView::getListCell(const char* key) {
    return ModCell::create(this, key, { this->m_width, this->m_itemSeparation });
}

void ModListView::loadCell(TableViewCell* cell, unsigned int index) {
    as<ModCell*>(cell)->loadFromMod(
        as<ModObject*>(this->m_entries->objectAtIndex(index))
    );
    as<ModCell*>(cell)->updateBGColor(index);
}

ModListView* ModListView::create(
    CCArray* mods
) {
    auto pRet = new ModListView;
    if (pRet) {
        if (pRet->init(mods, kBoomListType_Mod, 356.f, 220.f)) {
            pRet->autorelease();
            return pRet;
        }
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}
