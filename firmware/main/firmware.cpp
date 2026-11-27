#include <cstdlib>

#include "hw_peripherals.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ui/list.h"

static portMUX_TYPE state_mux = portMUX_INITIALIZER_UNLOCKED;

static void update_inputs_task(void*) {
    while (true) {
        HWPeripherals::updateInputs();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

extern "C" void app_main() {
    HWPeripherals::init(state_mux);

    UIList l(HWPeripherals::instance(), state_mux, true, "MENU");
    l.focus(true);
    l.addItem("Draw Solo");
    l.addItem("Host Session");
    l.addItem("Join Session");
    l.addItem("Change Brightness");
    l.addItem("Advanced");

    if (pdPASS != xTaskCreate(update_inputs_task, "inputs", 4096, nullptr, 1, nullptr)) {
        std::abort();
    }

    while (true) {
        l.draw();
        vTaskDelay(pdMS_TO_TICKS(1000));
        l.scrollDown();
    }
}
