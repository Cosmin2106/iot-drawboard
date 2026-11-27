#include "ui/canvas.h"

#include <algorithm>
#include <utility>

Canvas::Canvas(const HWPeripherals& peripherals, portMUX_TYPE& mux, const bool topbar_on, std::string title) :
    peripherals_(peripherals), mux_(mux), topbar_on_(topbar_on), title_(std::move(title)) {
}

void Canvas::beginDraw() const {
    u8g2_SetFontMode(&HWPeripherals::dd, 1);
    u8g2_ClearBuffer(&HWPeripherals::dd);
    u8g2_SetDrawColor(&HWPeripherals::dd, 1);

    if (topbar_on_) {
        portENTER_CRITICAL(mux_);
        const float bat_perc = HWPeripherals::bat_perc;
        portEXIT_CRITICAL(mux_);

        u8g2_SetFont(&HWPeripherals::dd, u8g2_font_9x15B_tr);
        u8g2_DrawStr(
            &HWPeripherals::dd,
            u8g2_GetDisplayWidth(&HWPeripherals::dd) - u8g2_GetStrWidth(&HWPeripherals::dd, title_.c_str()) - 10,
            14,
            title_.c_str()
        );
        u8g2_SetFont(&HWPeripherals::dd, u8g2_font_waffle_t_all);
        u8g2_DrawGlyph(&HWPeripherals::dd, 4, 13, 0xe242 + std::clamp(static_cast<int>(bat_perc), 1, 99) / 10);
        u8g2_SetFont(&HWPeripherals::dd, u8g2_font_6x10_tr);
        u8g2_DrawStr(
            &HWPeripherals::dd,
            20,
            13,
            (std::to_string(std::min(static_cast<int>(bat_perc), 100)) + "%").c_str()
        );
        u8g2_DrawHLine(&HWPeripherals::dd, 0, 18, u8g2_GetDisplayWidth(&HWPeripherals::dd));
    }
}

void Canvas::endDraw() const {
    u8g2_SendBuffer(&HWPeripherals::dd);
}

void Canvas::draw() const {
    beginDraw();
    endDraw();
}
