#pragma once

#include <string>

#include "freertos/FreeRTOS.h"

class Canvas {
    portMUX_TYPE& mux_;
    bool focused_ = false;
    bool topbar_on_;
    std::string title_;

protected:
    static constexpr int TopbarHeight = 18;

    [[nodiscard]] bool isFocused() const { return focused_; }
    [[nodiscard]] int contentTop() const { return topbar_on_ ? TopbarHeight + 1 : 0; }
    void beginDraw() const;
    void endDraw() const;

public:
    Canvas(portMUX_TYPE& mux, bool topbar_on, std::string title = "");

    virtual ~Canvas() = default;

    void focus(bool focused);

    virtual void draw();
};
