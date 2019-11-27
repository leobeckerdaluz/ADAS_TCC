#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_attr.h"

#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"


// 25 26 27 14
// #define GPIO_PWM0A_OUT 15   //Set GPIO 15 as PWM0A
// #define GPIO_PWM0B_OUT 16   //Set GPIO 16 as PWM0B
#define MOTOR_LEFT_FWD_PIN 27//33//25   //Set GPIO 15 as PWM0A
#define MOTOR_LEFT_REV_PIN 14//25//26   //Set GPIO 16 as PWM0B
#define MOTOR_RIGHT_FWD_PIN 12//26//27   //Set GPIO 15 as PWM0A
#define MOTOR_RIGHT_REV_PIN 13//27//14   //Set GPIO 16 as PWM0B

void mcpwm_example_gpio_initialize(void);
void mcpwm_motor_control(int8_t x_axis, int8_t y_axis);
void test_emergency_brakes(void);
void emergency_brake();