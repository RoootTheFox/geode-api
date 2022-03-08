#include <Geode.hpp>
#include <GeodeAPI.hpp>
#include "APIInternal.hpp"

USE_GEODE_NAMESPACE();

GEODE_API bool GEODE_CALL geode_load(Mod* mod) {
	Interface::get()->init(mod);

    /*mod->with<GeodeAPI>()->addKeybindAction(TriggerableAction {
        "Keybind Test",
        "test_keybind",
        KB_GLOBAL_CATEGORY,
        [](CCNode* node, bool down) -> bool {
            if (down) {
                FLAlertLayer::create("Hey", "Keybinds work", "OK")->show();
            }
            return false;
        }
    }, {{ KEY_T, Keybind::Modifiers::Control | Keybind::Modifiers::Alt }});*/


    NotificationCenter::get()->registerObserver(Interface::mod(), "dragdrop.geode", [](auto const& data) {
        auto path = data.template object<ghc::filesystem::path>();
        auto to_file = Loader::get()->getGeodeDirectory() / geodeModDirectory / path.filename();

        if (to_file == path) {
            FLAlertLayer::create("Oops!", "<cr>" + path.stem().u8string() + "</c> is already installed!", "OK")->show();
        } else if (ghc::filesystem::copy_file(path, to_file, ghc::filesystem::copy_options::overwrite_existing)) {
            ghc::filesystem::remove(path);
            FLAlertLayer::create("Success!", "<cg>" + path.stem().u8string() + "</c> successfully installed!", "OK")->show();
        } else {
            FLAlertLayer::create("Oops!", "<cr>" + path.stem().u8string() + "</c> couldn't be installed!", "OK")->show();
        }
    });

    return true;
}

GEODE_API bool GEODE_CALL geode_load_data(const char* path) {
    Interface::mod()->log() << __FUNCTION__;
    auto settingsPath = ghc::filesystem::path(path) / "api.json";
    if (ghc::filesystem::exists(settingsPath)) {
        try {
            auto data = file_utils::readString(settingsPath);
            if (!data) return false;
            if (!APIInternal::get()->load(nlohmann::json::parse(data.value())))
                return false;
        } catch(...) {
            return false;
        }
    }
    return true;
} 

GEODE_API bool GEODE_CALL geode_save_data(const char* path) {
    auto settingsPath = ghc::filesystem::path(path) / "api.json";
    auto json = nlohmann::json::object();
    if (!APIInternal::get()->save(json))
        return false;
    return file_utils::writeString(settingsPath, json.dump(4)).is_value();
} 
