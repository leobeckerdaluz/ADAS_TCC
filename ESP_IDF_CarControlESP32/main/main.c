
#include <stdio.h>

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>

#include "bt_handler.h"
#include "adas_car.h"
#include "uart_handler.h"
#include "mcpwm_brushed_dc_control.h"
#include "encoder_isr.h"

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

void get_speeds(void *arg)
{
}

/**
 * @brief Configure MCPWM module for brushed dc motor
 */
void mcpwm_example_brushed_motor_control(void *arg)
{
    mcpwm_example_gpio_initialize();

    while (1) {
        float duty = (float)abs(y_axis);

        if (y_axis > 0){
            brushed_motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_0, duty);
            brushed_motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_1, duty);
            printf("Indo pra frente por %f\n", duty);
        }
        else if (y_axis < 0){
            brushed_motor_backward(MCPWM_UNIT_0, MCPWM_TIMER_0, duty);
            brushed_motor_backward(MCPWM_UNIT_0, MCPWM_TIMER_1, duty);
            printf("Indo pra trás por %f\n", duty);
        }
    }
}


void app_main(void){
    printf("Começou!\n");

    init_bluetooth();
    set_encoder_ISRs();
    init_uart1();

    xTaskCreate(&blink_task, "blink_task", 1024, NULL, 5, NULL);
    xTaskCreate(get_ECU_serial_parameters_TASK, "get_ECU_serial_parameters_TASK", 1024, NULL, 10, NULL);
    // xTaskCreate(mcpwm_example_brushed_motor_control, "mcpwm_examlpe_brushed_motor_control", 4096, NULL, 5, NULL);
    xTaskCreate(get_rms_task, "encoder_task", 4096, NULL, 10, NULL);
    
}