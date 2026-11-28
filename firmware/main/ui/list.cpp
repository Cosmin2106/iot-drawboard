#include "ui/list.h"

#include <algorithm>
#include <utility>

#include "hw_peripherals.h"

UIList::UIList(portMUX_TYPE& mux, const bool topbar_on, std::string title) :
    Canvas(mux, topbar_on, std::move(title)) {
}

void UIList::addItem(std::string item) {
    items_.emplace_back(std::move(item));
}

void UIList::setCurrentPosition(const int pos) {
    position_ = std::clamp(pos, 0, static_cast<int>(items_.size()) - 1);
}

unsigned int UIList::getCurrentPosition() const {
    return position_;
}

void UIList::draw() {
    if (!isFocused()) {
        return;
    }
    beginDraw();

    const int visible_rows = (u8g2_GetDisplayHeight(&HWPeripherals::dd) - contentTop()) / ListItemHeight;
    if (items_.empty() || visible_rows <= 0) {
        endDraw();
        return;
    }

    first_visible_item_ = std::clamp(first_visible_item_, position_ - visible_rows + 1, position_);

    u8g2_SetClipWindow(
        &HWPeripherals::dd,
        0,
        contentTop(),
        u8g2_GetDisplayWidth(&HWPeripherals::dd),
        u8g2_GetDisplayHeight(&HWPeripherals::dd)
    );
    u8g2_SetFont(&HWPeripherals::dd, u8g2_font_6x13_tr);
    u8g2_SetFontPosBaseline(&HWPeripherals::dd);

    for (
        int item = first_visible_item_;
        item < std::min(first_visible_item_ + visible_rows, static_cast<int>(items_.size()));
        item++
    ) {
        const int y = contentTop() + (item - first_visible_item_) * ListItemHeight;
        if (item == position_) {
            u8g2_DrawBox(&HWPeripherals::dd, 0, y, u8g2_GetDisplayWidth(&HWPeripherals::dd), ListItemHeight);
        }
        u8g2_SetDrawColor(&HWPeripherals::dd, item != position_);
        u8g2_DrawStr(
            &HWPeripherals::dd,
            4,
            y + (ListItemHeight + u8g2_GetAscent(&HWPeripherals::dd) + u8g2_GetDescent(&HWPeripherals::dd)) / 2,
            items_[item].c_str()
        );
    }

    u8g2_SetDrawColor(&HWPeripherals::dd, 1);
    u8g2_SetMaxClipWindow(&HWPeripherals::dd);
    endDraw();
}
