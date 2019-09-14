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

uint8_t current_status = 0;

void ADAS_listener_TASK()
{
    // RASP
    gpio_pad_select_gpio(GPIO_RASP_COLLISION);
    gpio_set_direction(GPIO_RASP_COLLISION, GPIO_MODE_INPUT);
    gpio_pad_select_gpio(GPIO_RASP_AUTOBRAKE);
    gpio_set_direction(GPIO_RASP_AUTOBRAKE, GPIO_MODE_INPUT);

    while (1) {
        if (gpio_get_level(GPIO_RASP_AUTOBRAKE)){
            // printf("AUTOBRAKE!\n");
            current_status = 2;
        }
        else if (gpio_get_level(GPIO_RASP_COLLISION)){
            // printf("COLLISION!\n");
            current_status = 1;
        }
        else{
            // printf("SEGUE!\n");
            current_status = 0;
        }  
        vTaskDelay(10 / portTICK_PERIOD_MS);   
    }
}

void buzzer_TASK()
{
    // BUZZER
    gpio_pad_select_gpio(GPIO_BUZZER);
    gpio_set_direction(GPIO_BUZZER, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_BUZZER, 0);

    static uint8_t buzzer_status = 0;
    
    while(1){
        switch(current_status){
            case 0:
                gpio_set_level(GPIO_BUZZER, 0);
            break;
            case 1:
                if(buzzer_status)   gpio_set_level(GPIO_BUZZER, 0);
                else    gpio_set_level(GPIO_BUZZER, 1);
                vTaskDelay(100 / portTICK_PERIOD_MS);
                buzzer_status = !buzzer_status;
            break;
            case 2:
                gpio_set_level(GPIO_BUZZER, 1);
            break;
        }
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}