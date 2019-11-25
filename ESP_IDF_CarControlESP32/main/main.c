
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
                printf("\nBUZZER OFF\n");
                vTaskDelay(5 / portTICK_PERIOD_MS);   
            break;
            
            case 1:
                printf("\nFUDEUUUUUUUUUUUUUUUUUUUU\nnBUZZER\n");
                
                buzzer_status = !buzzer_status;
                gpio_set_level(BUZZER_GPIO, buzzer_status);
                vTaskDelay(100 / portTICK_PERIOD_MS);
            break;

            case 2:
                printf("\nBUZZER ON\n");
                gpio_set_level(BUZZER_GPIO, 1);
                vTaskDelay(5 / portTICK_PERIOD_MS);   
            break;
        
            default:
                gpio_set_level(BUZZER_GPIO, 0);
                vTaskDelay(5 / portTICK_PERIOD_MS);   
            break;
        }
    }
}

void app_main(void){
    printf("Começou!\n");

    gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    mcpwm_example_gpio_initialize();
    configure_bluetooth_and_events();   // When data comes, motor is handled
    set_encoder_ISRs();
    init_uart1();

    // xTaskCreate(&blink_task, "blink_task", 1024, NULL, 5, NULL);
    xTaskCreate(get_ECU_serial_parameters_TASK, "get_ECU_serial_parameters_TASK", 2048, NULL, 10, NULL);
    // xTaskCreate(get_rms_task, "encoder_task", 4096, NULL, 10, NULL);
    xTaskCreate(alarms_task, "alarms_task", 2048, NULL, 10, NULL);
    
}