#include "GUIContext.hpp"

GUIContext::GUIContext(Rect windowRect) : screen(windowRect)
{

}

void GUIContext::createWindow(std::shared_ptr<GUIWindow> window) {
    window->id = windows_.size();
    window->zOrder = windowOrder.size();

    windows_.push_back(window);
    windowOrder.push_back(window.get());
}

void GUIContext::closeWindow(std::shared_ptr<GUIWindow> window) {
    windows_.erase(windows_.begin()+window->id);
    windowOrder.erase(windowOrder.begin()+window->zOrder);
    reorderWindows();
}

void GUIContext::keyEvent(KeyEvent& keyEvent) {

}

void GUIContext::mouseMoveEvent(MouseMoveEvent& mouseMoveEvent) {
    
}

void GUIContext::mouseClickEvent(MouseClickEvent& mouseClickEvent) {
    
}

void GUIContext::reorderWindows() {
    for(int i = 0; i < windowOrder.size(); i++) windowOrder[i]->zOrder=i;
}

void GUIContext::reorderWindow(GUIWindow* window) {
    windowOrder.erase(windowOrder.begin()+window->zOrder);
    windowOrder.push_back(window);
    reorderWindows();
}

GUIWindow* GUIContext::getWindowByCursor(Vec2 pos) {
    
    GUIWindow* curWindow = nullptr;
    int maxZ = -1;

    for(auto& window : windows_) {
        if(window->zOrder > maxZ) {
            curWindow = window.get();
            maxZ = curWindow->zOrder;
        }
    }
    reorderWindow(curWindow);
    return curWindow;
}

void GUIEventListener::listen() {
    // MOUSE CLICK EVENT //
    bool mouseButtonPressed = false;
    if(Events::jclicked(GLFW_MOUSE_BUTTON_LEFT)) {
        mouseClickEvent.button = MouseButtons::Left;
        mouseClickEvent.mouseX = Events::x;
        mouseClickEvent.mouseY = Events::y;
        mouseButtonPressed = true;
    }
    else if(Events::jclicked(GLFW_MOUSE_BUTTON_RIGHT)) {
        mouseClickEvent.button = MouseButtons::Right;
        mouseClickEvent.mouseX = Events::x;
        mouseClickEvent.mouseY = Events::y;
        mouseButtonPressed = true;
    }
    else if(Events::jclicked(GLFW_MOUSE_BUTTON_MIDDLE)) {
        mouseClickEvent.button = MouseButtons::Middle;
        mouseClickEvent.mouseX = Events::x;
        mouseClickEvent.mouseY = Events::y;
        mouseButtonPressed = true;
    }
    if(mouseButtonPressed) context->mouseClickEvent(mouseClickEvent);

    // MOUSE MOVE EVENT //
    int button = -1;
    if(Events::clicked(GLFW_MOUSE_BUTTON_LEFT)) {
        mouseMoveEvent.button = MouseButtons::Left;
    }
    else if(Events::clicked(GLFW_MOUSE_BUTTON_RIGHT)) {
        mouseMoveEvent.button = MouseButtons::Right;
    }
    else if(Events::clicked(GLFW_MOUSE_BUTTON_MIDDLE)) {
        mouseMoveEvent.button = MouseButtons::Middle;
    }
    mouseMoveEvent.mouseX = Events::x;
    mouseMoveEvent.mouseY = Events::y;
    mouseMoveEvent.deltaX = Events::deltaX;
    mouseMoveEvent.deltaY = Events::deltaY;
    context->mouseMoveEvent(mouseMoveEvent);

    // KEY EVENT //
}