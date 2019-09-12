#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"

#include "driver/gpio.h"

#include "definitions.h"

#define GPIO_LEFT_FRONT GPIO_NUM_13
#define GPIO_RIGHT_FRONT GPIO_NUM_12
#define GPIO_LEFT_BACK GPIO_NUM_14
#define GPIO_RIGHT_BACK GPIO_NUM_27

#define DELAY_ON 3000

void init_motor(){
    // MOTOR
    gpio_pad_select_gpio(GPIO_LEFT_FRONT);
    gpio_set_direction(GPIO_LEFT_FRONT, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(GPIO_RIGHT_FRONT);
    gpio_set_direction(GPIO_RIGHT_FRONT, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(GPIO_LEFT_BACK);
    gpio_set_direction(GPIO_LEFT_BACK, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(GPIO_RIGHT_BACK);
    gpio_set_direction(GPIO_RIGHT_BACK, GPIO_MODE_OUTPUT);
}
void stop(){
    gpio_set_level(GPIO_LEFT_BACK, 0);
    gpio_set_level(GPIO_LEFT_FRONT, 0);
    gpio_set_level(GPIO_RIGHT_BACK, 0);
    gpio_set_level(GPIO_RIGHT_FRONT, 0);
}
void go_straight(){
    // Ordem diferente dos demais métodos, pois primeiro zera 
    // ... a roda e depois ativa a direção, evitando curto
    gpio_set_level(GPIO_LEFT_BACK, 0);
    gpio_set_level(GPIO_LEFT_FRONT, 1);
    gpio_set_level(GPIO_RIGHT_BACK, 0);
    gpio_set_level(GPIO_RIGHT_FRONT, 1);
}
void go_left(){
    // Ordem diferente dos demais métodos, pois primeiro zera 
    // ... a roda e depois ativa a direção, evitando curto
    gpio_set_level(GPIO_LEFT_FRONT, 0);
    gpio_set_level(GPIO_LEFT_BACK, 1);
    gpio_set_level(GPIO_RIGHT_BACK, 0);
    gpio_set_level(GPIO_RIGHT_FRONT, 1);
}
void go_right(){
    // Ordem diferente dos demais métodos, pois primeiro zera 
    // ... a roda e depois ativa a direção, evitando curto
    gpio_set_level(GPIO_LEFT_BACK, 0);
    gpio_set_level(GPIO_LEFT_FRONT, 1);
    gpio_set_level(GPIO_RIGHT_FRONT, 0);
    gpio_set_level(GPIO_RIGHT_BACK, 1);
}
void go_back(){
    // Ordem diferente dos demais métodos, pois primeiro zera 
    // ... a roda e depois ativa a direção, evitando curto
    gpio_set_level(GPIO_LEFT_FRONT, 0);
    gpio_set_level(GPIO_LEFT_BACK, 1);
    gpio_set_level(GPIO_RIGHT_FRONT, 0);
    gpio_set_level(GPIO_RIGHT_BACK, 1);
}

void motor_control_TASK()
{
    init_motor();

    while(1){
        switch(current_status){
            case 0:
                go_straight();
            break;
            case 1:
                go_left();
            break;
            case 2:
                stop();
            break;
        }
    }
}

void each_full_power()
{
    init_motor();

    // uint32_t on_time = DELAY_ON;
    // uint32_t off_time = DELAY_ON;
    // uint32_t multiple = 2;
    uint32_t on_on = 50;
    while (1) {
        printf("Começando loop!\n");
        for (int i=0; i<4; i++)
        {

        //     for (uint32_t j=0; j<on_on; j++)
        //     {
        //         // on_time = j*multiple;
        //         // off_time = (DELAY_ON-j)*multiple;
        //         // go_straight();
        //         // vTaskDelay(j / portTICK_PERIOD_MS);
                
        //         // vTaskDelay(100 / portTICK_PERIOD_MS);
                
        //         // stop();
        //         // vTaskDelay(10000-j / portTICK_PERIOD_MS);

        //         // vTaskDelay(100 / portTICK_PERIOD_MS);
        //         go_straight();
        //         vTaskDelay((j) / portTICK_PERIOD_MS);
                
        //         stop();
        //         vTaskDelay((on_on-j) / portTICK_PERIOD_MS);
        //     }

            printf("GO STRAIGHT!\n");
            go_straight();
            vTaskDelay(DELAY_ON / portTICK_PERIOD_MS);

            printf("GO LEFT!\n");
            go_left();
            vTaskDelay(DELAY_ON / portTICK_PERIOD_MS);

            printf("GO BACK!\n");
            go_back();
            vTaskDelay(DELAY_ON / portTICK_PERIOD_MS);

            printf("GO RIGHT!\n");
            go_right();
            vTaskDelay(DELAY_ON / portTICK_PERIOD_MS);
        }
        printf("Acabou o loop!\n");
    }
}