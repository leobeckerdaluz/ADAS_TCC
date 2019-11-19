
#include <stdio.h>

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>

#include "bt_handler.h"
#include "adas_car.h"
#include "from_adas_rasp_uart.h"
#include "mcpwm_brushed_dc_control.h"

void blink_task(void *pvParameter)
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while(1) {
        /* Blink off (output low) */
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(BLINK_DELAY / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(BLINK_DELAY / portTICK_PERIOD_MS);
    }
}


/**
 * @brief Configure MCPWM module for brushed dc motor
 */
void mcpwm_example_brushed_motor_control(void *arg)
{
    mcpwm_example_gpio_initialize();

    while (1) {
        brushed_motor_backward(MCPWM_UNIT_0, MCPWM_TIMER_0, 50.0);
        brushed_motor_backward(MCPWM_UNIT_0, MCPWM_TIMER_1, 50.0);

        vTaskDelay(2000 / portTICK_RATE_MS);

        brushed_motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_0, 50.0);
        brushed_motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_1, 50.0);

        vTaskDelay(2000 / portTICK_RATE_MS);
    }
}


void app_main(void){
    printf("Começou!\n");

    init_bluetooth();

    xTaskCreate(&blink_task, "blink_task", 1024, NULL, 5, NULL);
    xTaskCreate(echo_task, "uart_echo_task", 1024, NULL, 10, NULL);

    printf("Testing brushed motor...\n");
    xTaskCreate(mcpwm_example_brushed_motor_control, "mcpwm_examlpe_brushed_motor_control", 4096, NULL, 5, NULL);
}