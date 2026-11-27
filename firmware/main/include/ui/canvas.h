#pragma once

#include <string>

#include "hw_peripherals.h"

class Canvas {
    const HWPeripherals& peripherals_;
    portMUX_TYPE& mux_;
    unsigned int length_ = 0;
    unsigned int position_ = 0;
    bool topbar_on_;
    std::string title_;

    void beginDraw() const;
    void endDraw() const;

public:
    explicit Canvas(const HWPeripherals& peripherals, portMUX_TYPE& mux, bool topbar_on, std::string title = "");
    void draw() const;
};
