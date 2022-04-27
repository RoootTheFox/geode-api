#include "hook.hpp"
#include <dispatch/ExtMouseManager.hpp>

class $modify(CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(enumKeyCodes key, bool down) {
        ShortcutManager::get()->dispatchEvent(key, down);
        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down);
    }
};

class $modify(CCMouseDispatcher) {
    bool dispatchScrollMSG(float x, float y) {
        ExtMouseManager::get()->dispatchScrollEvent(
            y, x, ExtMouseManager::getMousePosition()
        );
        return CCMouseDispatcher::dispatchScrollMSG(x, y);
    }
};

class $modify(CCScheduler) {
    void update(float dt) {
        ExtMouseManager::get()->dispatchMoveEvent(
            ExtMouseManager::getMousePosition()
        );
        ShortcutManager::get()->update(dt);
        return CCScheduler::update(dt);
    }
};

#ifdef GEODE_IS_WINDOWS

class $modify(CCEGLView) {
    void pollEvents() {
        // MSG msg;
		// while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        //     TranslateMessage(&msg);
        //     if (msg.message == WM_DROPFILES) {
        //         UINT buffsize = MAX_PATH;
        //         char* buf = new char[MAX_PATH];
        //         HDROP hDropInfo = (HDROP) msg.wParam;
        //         DragQueryFile(hDropInfo, 0, buf, buffsize);

        //         ghc::filesystem::path p(buf);

        //         std::string fileExtension = p.extension().u8string();
        //         if (fileExtension.at(0) == '.') {
        //             fileExtension = fileExtension.substr(1);
        //         }

        //         NotificationCenter::get()->broadcast(Notification(
        //             "dragdrop",
        //             p,
        //             Mod::get()
        //         ));

        //         NotificationCenter::get()->broadcast(Notification(
        //             std::string("dragdrop.") + fileExtension,
        //             p,
        //             Mod::get()            
        //         ));

        //         delete[] buf;
        //     }
        //     DispatchMessage(&msg);
        // }
        CCEGLView::pollEvents();
    }

    void toggleFullScreen(bool fullscreen) {
        return CCEGLView::toggleFullScreen(fullscreen);
    }

    void onGLFWError(int code, const char* description) {
        return CCEGLView::onGLFWError(code, description);
    }
    
    void onGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        return CCEGLView::onGLFWKeyCallback(window, key, scancode, action, mods);
    }
    
	void updateWindow(int width, int height) {
        return CCEGLView::updateWindow(width, height);
    }
    
    void onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int mods) {
        std::cout << "clickety cluck clack\n";
        if (ExtMouseManager::get()->dispatchClickEvent(
            static_cast<MouseEvent>(button), action,
            ExtMouseManager::getMousePosition()
        )) return;
        return CCEGLView::onGLFWMouseCallBack(window, button, action, mods);
    }
};

#endif
