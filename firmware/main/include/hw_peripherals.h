#pragma once

#include "max17048.h"
#include "u8g2.h"
#include "esp_adc/adc_oneshot.h"
#include "hal/adc_types.h"
#include "portmacro.h"

#define CT_RAW_0_PCT    3300
#define CT_RAW_100_PCT  50

class HWPeripherals {
    inline static portMUX_TYPE* state_mux_ = nullptr;

    static adc_oneshot_unit_handle_t adc_;
    static adc_channel_t channel_ct1_;
    static adc_channel_t channel_ct2_;
    static max17048_handle_t bat_gauge_;

public:
    static bool trigger_left_pressed;
    static bool trigger_right_pressed;
    static int pot_left_perc;
    static int pot_right_perc;
    static float bat_perc_raw;
    static float bat_v;
    static u8g2_t dd;

    HWPeripherals() = default;

    HWPeripherals(const HWPeripherals&) = delete;

    HWPeripherals(HWPeripherals&&) = delete;

    HWPeripherals& operator=(const HWPeripherals&) = delete;

    HWPeripherals& operator=(HWPeripherals&&) = delete;

    static void init(portMUX_TYPE& mux);

    static HWPeripherals& instance();

    static void updateInputs();
};
