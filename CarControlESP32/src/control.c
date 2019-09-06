#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"

#include "driver/gpio.h"

#define GPIO_LEFT_FRONT GPIO_NUM_13
#define GPIO_RIGHT_FRONT GPIO_NUM_12
#define GPIO_LEFT_BACK GPIO_NUM_14
#define GPIO_RIGHT_BACK GPIO_NUM_27

void init_motor_pins(){
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