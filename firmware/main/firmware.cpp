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
    static UIList test1_list(state_mux, true, "TEST 1", {
                                 "Test Item 1",
                             });
    static UIList test2_list(state_mux, true, "TEST 2", {
                                 "Test Item 2",
                             });

    UI::addScreen(menu_list, true, {
                      {0, nullptr, &test1_list, nullptr},
                      {1, nullptr, &test2_list, nullptr},
                      {2, nullptr, nullptr, nullptr},
                      {3, nullptr, nullptr, nullptr},
                      {4, nullptr, nullptr, nullptr},
                      {5, nullptr, nullptr, nullptr}
                  });
    UI::addScreen(test1_list, false, {{0, &menu_list, nullptr, nullptr}});
    UI::addScreen(test2_list, false, {{0, &menu_list, nullptr, nullptr}});
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
