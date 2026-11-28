#include <cstdlib>

#include "hw_peripherals.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ui/list.h"
#include "ui/ui.h"

static portMUX_TYPE state_mux = portMUX_INITIALIZER_UNLOCKED;

static void init_ui() {
    UI::init(state_mux);

    static UIList menu_list(state_mux, true, "MENU", {
        "Draw Solo",
        "Host Session",
        "Join Session",
        "Download Drawings",
        "Change Brightness",
        "Debug Info"
    });
    UI::addScreen(menu_list, true, {0, nullptr, nullptr, nullptr});
    UI::addScreen(menu_list, true, {1, nullptr, nullptr, nullptr});
    UI::addScreen(menu_list, true, {2, nullptr, nullptr, nullptr});
    UI::addScreen(menu_list, true, {3, nullptr, nullptr, nullptr});
    UI::addScreen(menu_list, true, {4, nullptr, nullptr, nullptr});
    UI::addScreen(menu_list, true, {5, nullptr, nullptr, nullptr});
}

static void update_inputs_task(void*) {
    while (true) {
        HWPeripherals::updateInputs();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

extern "C" void app_main() {
    HWPeripherals::init(state_mux);
    init_ui();

    if (pdPASS != xTaskCreate(update_inputs_task, "inputs", 4096, nullptr, 1, nullptr)) {
        std::abort();
    }

    while (true) {
        UI::draw();
        vTaskDelay(1);
    }
}
