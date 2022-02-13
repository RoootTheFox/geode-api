#include <GeodeAPI.hpp>
#include <settings/SettingNodeManager.hpp>

USE_GEODE_NAMESPACE();

#ifdef GEODE_IS_WINDOWS

bool GeodeAPI::addKeybindAction(
    KeybindAction     const& action,
    KeybindList       const& defaults,
    keybind_action_id const& insertAfter
) {
    return KeybindManager::get()->addKeybindAction(
        this, action, defaults, insertAfter
    );
}

bool GeodeAPI::removeKeybindAction(keybind_action_id const& id) {
    return KeybindManager::get()->removeKeybindAction(this, id);
}

bool GeodeAPI::addDragDropHandler(
    std::string handler_id,
    std::function<bool(Path)> handler
) {
    return DragDropManager::get()->addDropHandler(this, handler_id, handler);
}

bool GeodeAPI::addDragDropHandler(
    std::string handler_id,
    std::function<bool(Path)> handler,
    std::string extension
) {
    return DragDropManager::get()->addDropHandler(this, handler_id, handler, extension);
}

bool GeodeAPI::removeKeybindAction(std::string handler_id) {
    return DragDropManager::get()->removeDropHandler(handler_id);
}

#endif

Result<> GeodeAPI::setCustomSettingNode(
    std::string const& key,
    CustomSettingNodeGenerator generator
) {
    if (SettingNodeManager::get()->registerCustomNode(this, key, generator)) {
        return Ok<>();
    } else {
        return Err<>("Unknown error");
    }
}
