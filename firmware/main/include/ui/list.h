#pragma once

#include <vector>

#include "canvas.h"

class UIList : public Canvas {
    static constexpr int ListItemHeight = 15;

    int position_ = 0;
    int first_visible_item_ = 0;
    std::vector<std::string> items_{};

public:
    UIList(const HWPeripherals& peripherals, portMUX_TYPE& mux, bool topbar_on, std::string title = "");
    void addItem(std::string item);
    void scrollUp();
    void scrollDown();
    [[nodiscard]] unsigned int getCurrentPosition() const;
    void draw() override;
};
