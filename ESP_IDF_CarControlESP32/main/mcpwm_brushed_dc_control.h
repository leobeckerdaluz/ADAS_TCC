#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_attr.h"

#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"


// 25 26 27 14
// #define GPIO_PWM0A_OUT 15   //Set GPIO 15 as PWM0A
// #define GPIO_PWM0B_OUT 16   //Set GPIO 16 as PWM0B
#define MOTOR_LEFT_FWD_PIN 25   //Set GPIO 15 as PWM0A
#define MOTOR_LEFT_REV_PIN 26   //Set GPIO 16 as PWM0B
#define MOTOR_RIGHT_FWD_PIN 27   //Set GPIO 15 as PWM0A
#define MOTOR_RIGHT_REV_PIN 14   //Set GPIO 16 as PWM0B

void brushed_motor_stop(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num);
void brushed_motor_forward(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num , float duty_cycle);
void brushed_motor_backward(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num , float duty_cycle);
void mcpwm_example_gpio_initialize(void);
