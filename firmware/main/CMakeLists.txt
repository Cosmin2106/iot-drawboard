set(SOURCES
        firmware.cpp
        hal_extensions.c
        hw_peripherals.cpp
        ui/ui.cpp
        ui/canvas.cpp
)

idf_component_register(
        SRCS ${SOURCES}
        INCLUDE_DIRS "include"
        REQUIRES esp_adc esp_driver_gpio esp_driver_i2c u8g2 freertos
)
