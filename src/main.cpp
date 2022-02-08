#include <Geode.hpp>
#include <GeodeAPI>

USE_GEODE_NAMESPACE();

GEODE_API bool GEODE_CALL geode_load(Mod* mod) {
	Interface::get()->init(mod);
    
    #ifdef GEODE_IS_WINDOWS
    mod->with<GeodeAPI>()->addKeybindAction(TriggerableAction {
        "Keybind Test",
        "test_keybind",
        KB_GLOBAL_CATEGORY,
        [](CCNode* node, bool down) -> bool {
            if (down) {
                FLAlertLayer::create("Hey", "Keybinds work", "OK")->show();
            }
            return false;
        }
    }, {{ KEY_T, Keybind::Modifiers::Control | Keybind::Modifiers::Alt }});
    #endif

    return true;
}
