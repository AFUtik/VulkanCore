#pragma once 

#include <algorithm>
#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <variant>
#include <queue>
#include "../Fonts.hpp"

// GUI LOGIC HEADER //

enum MetricsFormat {
    Pixels
};

enum GUIContentType {
    TextBox
};

enum MouseEventType {
    Scroll,
    Button
};

struct Color {
    float r=1.0f, g=1.0f, b=1.0f, a=1.0f; // Default white //
};

struct MouseEvent {
    MouseEventType eventType;
    uint32_t mouse_x;
    uint32_t mouse_y;
    float scroll;
    bool mouseButton;
};

struct KeyboardEvent {
    uint32_t codepointPressed;
    uint8_t  keyPressed;
};

struct GUIContent {
    uint32_t id;
    uint32_t version = 0;

    virtual void key(KeyboardEvent& keyEvent);
    virtual void mouse(MouseEvent& mouseEvent);
};

struct TextBox : GUIContent {
    uint32_t pos_x = 0;
    uint32_t pos_y = 0;
    uint32_t boxWidth = 0;
    uint32_t boxHeight = 0;

    Text text;
    Color color {0.0f, 0.0f, 0.0f, 1.0f}; // Black //
};

struct GUIWindow {
    uint32_t version = 0;

    uint32_t posX;
    uint32_t posY;
    uint32_t windowWidth;
    uint32_t windowHeight;
    uint32_t scrollX;
    uint32_t scrollY;
    bool scrollEnable = false;
    Color windowColor;

    // Content padding //
    uint32_t contentWidth;
    uint32_t contentHeight;
    uint32_t left_padding = 0;
    uint32_t top_padding  = 0;
    Color contentColor;

    GUIContent* activeContent = nullptr;

    std::vector<std::unique_ptr<GUIContent>> contents;
    int uid_counter = 0;

    template<typename T, typename... Args>
    T* createElement(Args&&... args)
    {
        auto content = std::make_unique<T>(std::forward<Args>(args)...);
        content->id = uid_counter;
        uid_counter++;

        T* ptr = content.get();
        contents.push_back(std::move(content));
        return ptr;
    }
};

struct GUIContext {
    MetricsFormat metricsFormat;

    uint32_t screenWidth;
    uint32_t screenHeight;

    GUIWindow* activeWindow = nullptr;

    std::vector<std::shared_ptr<GUIWindow>> windows_;

    GUIContext(uint32_t width, uint32_t height);

    void key(KeyboardEvent& keyboardEvent);
    void mouse(MouseEvent& mouseEvent);

    void createWindow(std::shared_ptr<GUIWindow> window);
    inline std::vector<std::shared_ptr<GUIWindow>>& windows() {return windows_;}
};
