#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/xtensa_api.h"
#include "driver/uart.h"

#include "driver/gpio.h"
#include "driver/ledc.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "esp_attr.h"   
#include "esp_err.h"

#include "definitions.h"

void blink_task(void *pvParameter)
{
   // LED
    gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    
    while(1) {
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(BLINK_DELAY / portTICK_PERIOD_MS);

        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(BLINK_DELAY / portTICK_PERIOD_MS);
    }
}

void app_main()
{
    xTaskCreate(&blink_task, "blink_task", 1024, NULL, 4, NULL);
    xTaskCreate(&joystick_bluetooth_TASK, "uart_bt_task", 2048, NULL, 10, NULL);
    xTaskCreate(&ADAS_listener_TASK, "rasp_integration", 1024, NULL, 2, NULL);
    xTaskCreate(&buzzer_TASK, "buzzer_control", 1024, NULL, 3, NULL);
    xTaskCreate(&motor_control_TASK, "motor_control", 2048, NULL, 1, NULL);
}