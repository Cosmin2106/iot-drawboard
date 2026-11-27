#include <cstdlib>

#include "hw_peripherals.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static portMUX_TYPE state_mux = portMUX_INITIALIZER_UNLOCKED;

static void update_inputs_task(void*) {
    while (true) {
        HWPeripherals::updateInputs();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

extern "C" void app_main() {
    HWPeripherals::init(state_mux);

    if (pdPASS != xTaskCreate(update_inputs_task, "inputs", 4096, nullptr, 1, nullptr)) {
        std::abort();
    }

    while (true) {
        portENTER_CRITICAL(&state_mux);
        const int left_pot_percentage = HWPeripherals::pot_left_perc;
        const int right_pot_percentage = HWPeripherals::pot_right_perc;
        const float battery_percentage = HWPeripherals::bat_perc;
        const float battery_voltage = HWPeripherals::bat_v;
        const bool left_pressed = HWPeripherals::trigger_left_pressed;
        const bool right_pressed = HWPeripherals::trigger_right_pressed;
        portEXIT_CRITICAL(&state_mux);

        char buffer1[16], buffer2[16], buffer3[32], buffer4[16], buffer5[16];
        sprintf(buffer1, "CT1: %d%%", left_pot_percentage);
        sprintf(buffer2, "CT2: %d%%", right_pot_percentage);
        sprintf(buffer3, "BAT: %.1f%%, %.2fV", battery_percentage, battery_voltage);
        sprintf(buffer4, "BTN1: %s", left_pressed ? "ON" : "off");
        sprintf(buffer5, "BTN2: %s", right_pressed ? "ON" : "off");

        u8g2_ClearBuffer(&HWPeripherals::dd);
        u8g2_SetFont(&HWPeripherals::dd, u8g2_font_helvB08_tr);
        u8g2_DrawStr(&HWPeripherals::dd, 0, 10, buffer1);
        u8g2_DrawStr(&HWPeripherals::dd, 0, 20, buffer2);
        u8g2_DrawStr(&HWPeripherals::dd, 0, 30, buffer3);
        u8g2_DrawStr(&HWPeripherals::dd, 0, 40, buffer4);
        u8g2_DrawStr(&HWPeripherals::dd, 0, 50, buffer5);
        u8g2_SendBuffer(&HWPeripherals::dd);

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
