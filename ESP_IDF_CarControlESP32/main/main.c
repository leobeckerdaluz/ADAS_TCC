
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

void alarms_task(void *pvParameter)
{
    gpio_pad_select_gpio(BUZZER_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BUZZER_GPIO, GPIO_MODE_OUTPUT);
    
    static uint8_t buzzer_status = 0;

    while(1){
        switch (current_state)
        {
            case 0:
                gpio_set_level(BUZZER_GPIO, 0);
            break;
            
            case 1:
                printf("\nBUZZER\nnBUZZER\n");
                printf("\nFUDEUUUUUUUUUUUUUUUUUUUU\nnBUZZER\n");
                
                buzzer_status = !buzzer_status;
                gpio_set_level(BUZZER_GPIO, buzzer_status);
                vTaskDelay(100 / portTICK_PERIOD_MS);
            break;

            case 2:
                gpio_set_level(BUZZER_GPIO, 1);
            break;
        
            default:
                gpio_set_level(BUZZER_GPIO, 0);
            break;
        }
        vTaskDelay(5 / portTICK_PERIOD_MS);   
    }
}

void app_main(void){
    printf("Começou!\n");

    configure_bluetooth_and_events();
    set_encoder_ISRs();
    init_uart1();
    mcpwm_example_gpio_initialize();

    xTaskCreate(&blink_task, "blink_task", 1024, NULL, 5, NULL);
    xTaskCreate(get_ECU_serial_parameters_TASK, "get_ECU_serial_parameters_TASK", 2048, NULL, 10, NULL);
    xTaskCreate(get_rms_task, "encoder_task", 4096, NULL, 10, NULL);
    xTaskCreate(alarms_task, "alarms_task", 2048, NULL, 10, NULL);
    
}