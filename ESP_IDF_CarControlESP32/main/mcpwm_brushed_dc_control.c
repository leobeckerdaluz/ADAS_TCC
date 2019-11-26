/* brushed dc motor control example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/*
 * This example will show you how to use MCPWM module to control brushed dc motor.
 * This code is tested with L298 motor driver.
 * User may need to make changes according to the motor driver they use.
*/

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_attr.h"

#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"

#include "bt_handler.h"

#include "adas_car.h"
#include "uart_handler.h"
#include "mcpwm_brushed_dc_control.h"



void mcpwm_example_gpio_initialize(void)
{
    printf("initializing mcpwm gpio...\n");
    
    //1. mcpwm gpio initialization
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, MOTOR_LEFT_FWD_PIN);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, MOTOR_LEFT_REV_PIN);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1A, MOTOR_RIGHT_FWD_PIN);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1B, MOTOR_RIGHT_REV_PIN);

    //2. initial mcpwm configuration
    printf("Configuring Initial Parameters of mcpwm...\n");
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 1000;    //frequency = 500Hz,
    pwm_config.cmpr_a = 0;    //duty cycle of PWMxA = 0
    pwm_config.cmpr_b = 0;    //duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    //Configure PWM0A & PWM0B with above settings
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config);    //Configure PWM1A & PWM1B with above settings
}

/**
 * @brief motor moves in forward direction, with duty cycle = duty %
 */
void brushed_motor_forward(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num , float duty_cycle)
{
    mcpwm_set_signal_low(mcpwm_num, timer_num, MCPWM_OPR_B);
    mcpwm_set_duty(mcpwm_num, timer_num, MCPWM_OPR_A, duty_cycle);
    mcpwm_set_duty_type(mcpwm_num, timer_num, MCPWM_OPR_A, MCPWM_DUTY_MODE_0); //call this each time, if operator was previously in low/high state
}

/**
 * @brief motor moves in backward direction, with duty cycle = duty %
 */
void brushed_motor_backward(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num , float duty_cycle)
{
    mcpwm_set_signal_low(mcpwm_num, timer_num, MCPWM_OPR_A);
    mcpwm_set_duty(mcpwm_num, timer_num, MCPWM_OPR_B, duty_cycle);
    mcpwm_set_duty_type(mcpwm_num, timer_num, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);  //call this each time, if operator was previously in low/high state
}

/**
 * @brief motor stop
 */
void brushed_motor_stop(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    mcpwm_set_signal_low(mcpwm_num, timer_num, MCPWM_OPR_A);
    mcpwm_set_signal_low(mcpwm_num, timer_num, MCPWM_OPR_B);
}

void emergency_brake()
{
    brushed_motor_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
    brushed_motor_stop(MCPWM_UNIT_0, MCPWM_TIMER_1);
    printf("FREIO DE EMERGENCIA ATIVADO!\n");
    
    brushed_motor_backward(MCPWM_UNIT_0, MCPWM_TIMER_0, 50);
    brushed_motor_backward(MCPWM_UNIT_0, MCPWM_TIMER_1, 50);
    vTaskDelay(150 / portTICK_PERIOD_MS);
    
    brushed_motor_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
    brushed_motor_stop(MCPWM_UNIT_0, MCPWM_TIMER_1);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    printf("FREIO LIBERADO!\n");

    gpio_set_level(BUZZER_GPIO, 0);
    printf("BUZZER DESLIGADO!\n");
}

void test_emergency_brakes(void){
    uint8_t teste = 0;

    up:

    printf("Acelerador no máximo!\n");
    brushed_motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_0, 100);
    brushed_motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_1, 100);
    
    printf("3...\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("2...\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("1...\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    if (teste == 0){
        teste = 1;
        goto there;
    }
    else{
        teste = 0;
        brushed_motor_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
        brushed_motor_stop(MCPWM_UNIT_0, MCPWM_TIMER_1);
        
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        goto up;
    }

    there:
    emergency_brake();

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    goto up;
}

/**
 * @brief Configure MCPWM module for brushed dc motor
 */
void mcpwm_motor_control(int8_t x_axis, int8_t y_axis)
{
    const int limit = 60;
    static uint8_t emergency_brake_applied = 0;
    
    // Converte os valores de duty
    float duty_y =  (float)abs(y_axis);
    float duty_x =  (float)abs(x_axis);
    
    // FUTURO - interpolar os dados pra saber quanto de duty em cada roda
    
    // Se não está no modo de auto brake, 
    if (current_state == 2)
    {
        if (y_axis < -limit){
            brushed_motor_backward(MCPWM_UNIT_0, MCPWM_TIMER_0, duty_y);
            brushed_motor_backward(MCPWM_UNIT_0, MCPWM_TIMER_1, duty_y);
            printf("Indo pra trás por %f\n", duty_y);
        }
        else
        {
            if (emergency_brake_applied == 0){
                emergency_brake();
                emergency_brake_applied = 1;
            }
            else{
                brushed_motor_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
                brushed_motor_stop(MCPWM_UNIT_0, MCPWM_TIMER_1);
                printf("PAROU\n");
            }
        }
    }
    else
    {
        emergency_brake_applied = 0;

        if (x_axis > limit){
            brushed_motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_0, duty_x);
            brushed_motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_1, duty_x/3);
            printf("Indo pra direita por %f\n", duty_x);
        }
        else if (x_axis < -limit)
        {
            brushed_motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_0, duty_x/3);
            brushed_motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_1, duty_x);
            printf("Indo pra esquerda por %f\n", duty_x);
        }
        else if (y_axis > limit)
        {
            brushed_motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_0, duty_y);
            brushed_motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_1, duty_y);
            printf("Indo pra frente por %f\n", duty_y);
        }
        else if (y_axis < -limit)
        {
            brushed_motor_backward(MCPWM_UNIT_0, MCPWM_TIMER_0, duty_y);
            brushed_motor_backward(MCPWM_UNIT_0, MCPWM_TIMER_1, duty_y);
            printf("Indo pra trás por %f\n", duty_y);
        }
        else
        {
            brushed_motor_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
            brushed_motor_stop(MCPWM_UNIT_0, MCPWM_TIMER_1);
            printf("PAROU\n");
        }
    }
}