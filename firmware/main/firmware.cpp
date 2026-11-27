#include <cstdlib>

#include "hw_peripherals.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ui/canvas.h"

static portMUX_TYPE state_mux = portMUX_INITIALIZER_UNLOCKED;

static void update_inputs_task(void*) {
    while (true) {
        HWPeripherals::updateInputs();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

extern "C" void app_main() {
    HWPeripherals::init(state_mux);

    const Canvas c(HWPeripherals::instance(), state_mux, true, "MENU");

    if (pdPASS != xTaskCreate(update_inputs_task, "inputs", 4096, nullptr, 1, nullptr)) {
        std::abort();
    }

    while (true) {
        c.draw();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
