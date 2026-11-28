#pragma once

#include <algorithm>
#include <vector>

#include "canvas.h"

class UIList : public Canvas {
    static constexpr int ListItemHeight = 15;

    int position_ = 0;
    int first_visible_item_ = 0;
    std::vector<std::string> items_;

public:
    UIList(portMUX_TYPE& mux, bool topbar_on, std::string title = "", std::vector<std::string> items = {});

    void setCurrentPosition(int pos);

    [[nodiscard]] unsigned int getCurrentPosition() const;

    [[nodiscard]] unsigned int size() const { return items_.size(); }

    void draw() override;
};
