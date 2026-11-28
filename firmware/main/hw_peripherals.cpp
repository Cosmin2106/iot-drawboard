#include "hw_peripherals.h"

#include <algorithm>
#include <cstdlib>

#include "esp_err.h"
#include "hal_extensions.h"
#include "driver/gpio.h"

adc_oneshot_unit_handle_t HWPeripherals::adc_{};
adc_channel_t HWPeripherals::channel_ct1_{};
adc_channel_t HWPeripherals::channel_ct2_{};
max17048_handle_t HWPeripherals::bat_gauge_{};

bool HWPeripherals::trigger_left_pressed{};
bool HWPeripherals::trigger_right_pressed{};
int HWPeripherals::pot_left_perc{};
int HWPeripherals::pot_right_perc{};
float HWPeripherals::bat_perc_raw{};
float HWPeripherals::bat_v{};
u8g2_t HWPeripherals::dd{};

void HWPeripherals::init(portMUX_TYPE& mux) {
    if (state_mux_ != nullptr) {
        std::abort();
    }
    state_mux_ = &mux;

    ESP_ERROR_CHECK(gpio_set_direction(TR1_PIN, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_set_direction(TR2_PIN, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_level(LED_PIN, 0));

    adc_unit_t unit_ct1, unit_ct2;

    ESP_ERROR_CHECK(adc_oneshot_io_to_channel(CT1_PIN, &unit_ct1, &channel_ct1_));
    ESP_ERROR_CHECK(adc_oneshot_io_to_channel(CT2_PIN, &unit_ct2, &channel_ct2_));
    // ESP32C6 has a single ADC unit, so the above unit variables should always be the same
    ESP_ERROR_CHECK(unit_ct1 == unit_ct2 ? ESP_OK : ESP_ERR_NOT_SUPPORTED);

    const adc_oneshot_unit_init_cfg_t adc_config = {
        .unit_id = unit_ct1,
        .clk_src = ADC_DIGI_CLK_SRC_DEFAULT,
        .ulp_mode = ADC_ULP_MODE_DISABLE
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_config, &adc_));

    constexpr adc_oneshot_chan_cfg_t channel_config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_, channel_ct1_, &channel_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_, channel_ct2_, &channel_config));

    init_i2c();

    bat_gauge_ = max17048_create(i2c_bus, MAX17048_I2C_ADDR_DEFAULT);
    ESP_ERROR_CHECK(bat_gauge_ ? ESP_OK : ESP_FAIL);

    u8g2_Setup_ssd1315_i2c_128x64_noname_f(&dd, U8G2_R0, hal_u8g2_cb, hal_u8g2_delay);
    u8g2_InitDisplay(&dd);
    u8g2_SetPowerSave(&dd, 0);
    u8g2_SetContrast(&dd, 200);
}

HWPeripherals& HWPeripherals::instance() {
    static HWPeripherals inputs_instance{};
    return inputs_instance;
}

void HWPeripherals::updateInputs() {
    const bool left_pressed = gpio_get_level(TR1_PIN) == 0;
    const bool right_pressed = gpio_get_level(TR2_PIN) == 0;

    int raw_ct1, raw_ct2;
    adc_oneshot_read(adc_, channel_ct1_, &raw_ct1);
    adc_oneshot_read(adc_, channel_ct2_, &raw_ct2);
    const int left_percentage = std::clamp((raw_ct1 - CT_RAW_0_PCT) * 100 / (CT_RAW_100_PCT - CT_RAW_0_PCT), 0, 100);
    const int right_percentage = std::clamp((raw_ct2 - CT_RAW_0_PCT) * 100 / (CT_RAW_100_PCT - CT_RAW_0_PCT), 0, 100);

    float percentage;
    float voltage;
    const bool battery_read_ok = max17048_get_cell_percent(bat_gauge_, &percentage) == ESP_OK &&
        max17048_get_cell_voltage(bat_gauge_, &voltage) == ESP_OK;

    portENTER_CRITICAL(state_mux_);
    trigger_left_pressed = left_pressed;
    trigger_right_pressed = right_pressed;
    pot_left_perc = left_percentage;
    pot_right_perc = right_percentage;
    bat_perc_raw = battery_read_ok ? percentage : -1.0f;
    bat_v = battery_read_ok ? voltage : -1.0f;
    portEXIT_CRITICAL(state_mux_);
}
