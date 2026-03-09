#pragma once 

#include <algorithm>
#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <variant>
#include <queue>
#include "../Fonts.hpp"
#include "../window/Events.hpp"

// GUI LOGIC HEADER //

enum MetricsFormat {
    Pixels
};

enum GUIContentType {
    TextBox
};

enum MouseButtons {
    Left, Right, Middle
};

struct Vec2 {
    uint32_t x;
    uint32_t y;
};

struct Rect {
    uint32_t width;
    uint32_t height;
};

struct Color {
    float r=1.0f, g=1.0f, b=1.0f, a=1.0f; // Default white //

    static constexpr Color hex(const std::string& hex_string) {
        unsigned int r=0, g=0, b=0;
        if(hex_string[0] == '#') {
            std::string rs = hex_string.substr(1,2);
            std::string gs = hex_string.substr(3,2);
            std::string bs = hex_string.substr(5,2);
            r = std::stoul(rs, nullptr, 16);
            g = std::stoul(gs, nullptr, 16);
            b = std::stoul(bs, nullptr, 16);
        }
        return Color{ r / 255.0f, g / 255.0f, b / 255.0f, 1.0f };
    }
};

struct MouseMoveEvent {
    uint32_t mouseX = 0;
    uint32_t mouseY = 0;
    float deltaX = 0.0f;
    float deltaY = 0.0f;
    float scrollX = 0.0f;
    float scrollY = 0.0f;
    int button = 0;
};

struct MouseClickEvent {
    uint32_t mouseX = 0;
    uint32_t mouseY = 0;
    int button = 0;
};

struct KeyEvent {
    uint32_t codepointPressed;
    uint8_t  keyPressed;
};

struct GUIContent {
    uint32_t id;
    uint32_t version = 0;
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
    uint32_t id = 0;
    uint32_t version = 0;
    uint32_t zOrder = 0;
    bool visible = true;

    Vec2 pos;
    Vec2 scroll;
    Vec2 padding;
    Rect window;
    Rect footer;
    Rect content;

    bool scrollEnable = false;
    Color footerColor = Color::hex("#d8d8d8");
    Color windowColor = Color::hex("#ffffff");
    Color contentColor;

    // Content padding //
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

class GUIContext {
public:
    MetricsFormat metricsFormat;
    Rect screen;
    
    GUIContext(Rect windowRect);

    void keyEvent(KeyEvent& keyboardEvent);
    void mouseMoveEvent(MouseMoveEvent& mouseEvent);
    void mouseClickEvent(MouseClickEvent& mouseClickEvent);

    void createWindow(std::shared_ptr<GUIWindow> window);
    void closeWindow (std::shared_ptr<GUIWindow> window);
    inline std::vector<std::shared_ptr<GUIWindow>>& windows() {return windows_;}
private:
    void reorderWindows();
    void reorderWindow(GUIWindow* window);
    GUIWindow* getWindowByCursor(Vec2 pos); 

    std::vector<std::shared_ptr<GUIWindow>> windows_;
    std::vector<GUIWindow*> windowOrder;
};

struct GUIEventListener {
    GUIContext* context;

    MouseMoveEvent mouseMoveEvent;
    MouseClickEvent mouseClickEvent;
    KeyEvent keyEvent;

    GUIEventListener(GUIContext* context) : context(context) {};

    void listen();
};