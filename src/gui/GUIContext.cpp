#include "GUIContext.hpp"

GUIContext::GUIContext(uint32_t width, uint32_t height) : screenWidth(width), screenHeight(height)
{

}

void GUIContext::createWindow(std::shared_ptr<GUIWindow> window) {
    windows_.push_back(window);
}