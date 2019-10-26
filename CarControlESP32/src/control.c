#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"

#include "driver/gpio.h"
#include "driver/ledc.h"

#include "definitions.h"


#define GPIO_LEFT_FRONT GPIO_NUM_13
#define GPIO_RIGHT_FRONT GPIO_NUM_12
#define GPIO_LEFT_BACK GPIO_NUM_14
#define GPIO_RIGHT_BACK GPIO_NUM_27

#define DELAY_ON 2000

void init_motor(){
    // // MOTOR
    // gpio_pad_select_gpio(GPIO_LEFT_FRONT);
    // gpio_set_direction(GPIO_LEFT_FRONT, GPIO_MODE_OUTPUT);
    // gpio_pad_select_gpio(GPIO_RIGHT_FRONT);
    // gpio_set_direction(GPIO_RIGHT_FRONT, GPIO_MODE_OUTPUT);
    // gpio_pad_select_gpio(GPIO_LEFT_BACK);
    // gpio_set_direction(GPIO_LEFT_BACK, GPIO_MODE_OUTPUT);
    // gpio_pad_select_gpio(GPIO_RIGHT_BACK);
    // gpio_set_direction(GPIO_RIGHT_BACK, GPIO_MODE_OUTPUT);

    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_8_BIT, // resolution of PWM duty
        .freq_hz = 5000,                      // frequency of PWM signal
        .speed_mode = LEDC_HS_MODE,           // timer mode
        .timer_num = LEDC_HS_TIMER,            // timer index
    };
    // Set configuration of timer0 for high speed channels
    ledc_timer_config(&ledc_timer);

    // Prepare and set configuration of timer1 for low speed channels
    ledc_timer.speed_mode = LEDC_LS_MODE;
    ledc_timer.timer_num = LEDC_LS_TIMER;
    ledc_timer_config(&ledc_timer);

    /*
     * Prepare individual configuration
     * for each channel of LED Controller
     * by selecting:
     * 
     * - output duty cycle, set initially to 0
     * - GPIO number where LED is connected to
     * - speed mode, either high or low
     * - timer servicing selected channel
     *   Note: if different channels use one timer,
     *         then frequency and bit_num of these channels
     *         will be the same
     */
    ledc_channel_config_t ledc_channel[LEDC_TEST_CH_NUM] = {
        {
            .channel    = LEDC_HS_CH0_CHANNEL,
            .duty       = 0,
            .gpio_num   = LEDC_HS_CH0_GPIO,
            .speed_mode = LEDC_HS_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_HS_TIMER
        },
        {
            .channel    = LEDC_HS_CH1_CHANNEL,
            .duty       = 0,
            .gpio_num   = LEDC_HS_CH1_GPIO,
            .speed_mode = LEDC_HS_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_HS_TIMER
        },
        {
            .channel    = LEDC_LS_CH2_CHANNEL,
            .duty       = 0,
            .gpio_num   = LEDC_LS_CH2_GPIO,
            .speed_mode = LEDC_LS_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_LS_TIMER
        },
        {
            .channel    = LEDC_LS_CH3_CHANNEL,
            .duty       = 0,
            .gpio_num   = LEDC_LS_CH3_GPIO,
            .speed_mode = LEDC_LS_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_LS_TIMER
        },
    };

    // Set LED Controller with previously prepared configuration
    int ch;
    for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
        ledc_channel_config(&ledc_channel[ch]);
    }
}
// void stop(){
//     gpio_set_level(GPIO_LEFT_BACK, 0);
//     gpio_set_level(GPIO_LEFT_FRONT, 0);
//     gpio_set_level(GPIO_RIGHT_BACK, 0);
//     gpio_set_level(GPIO_RIGHT_FRONT, 0);
// }
// void go_straight(){
//     // Ordem diferente dos demais métodos, pois primeiro zera 
//     // ... a roda e depois ativa a direção, evitando curto
//     gpio_set_level(GPIO_LEFT_BACK, 0);
//     gpio_set_level(GPIO_LEFT_FRONT, 1);
//     gpio_set_level(GPIO_RIGHT_BACK, 0);
//     gpio_set_level(GPIO_RIGHT_FRONT, 1);
// }
// void go_left(){
//     // Ordem diferente dos demais métodos, pois primeiro zera 
//     // ... a roda e depois ativa a direção, evitando curto
//     gpio_set_level(GPIO_LEFT_FRONT, 0);
//     gpio_set_level(GPIO_LEFT_BACK, 1);
//     gpio_set_level(GPIO_RIGHT_BACK, 0);
//     gpio_set_level(GPIO_RIGHT_FRONT, 1);
// }
// void go_right(){
//     // Ordem diferente dos demais métodos, pois primeiro zera 
//     // ... a roda e depois ativa a direção, evitando curto
//     gpio_set_level(GPIO_LEFT_BACK, 0);
//     gpio_set_level(GPIO_LEFT_FRONT, 1);
//     gpio_set_level(GPIO_RIGHT_FRONT, 0);
//     gpio_set_level(GPIO_RIGHT_BACK, 1);
// }
// void go_back(){
//     // Ordem diferente dos demais métodos, pois primeiro zera 
//     // ... a roda e depois ativa a direção, evitando curto
//     gpio_set_level(GPIO_LEFT_FRONT, 0);
//     gpio_set_level(GPIO_LEFT_BACK, 1);
//     gpio_set_level(GPIO_RIGHT_FRONT, 0);
//     gpio_set_level(GPIO_RIGHT_BACK, 1);
// }


void go_straight_pwm(uint8_t duty){
    ledc_set_duty(LEDC_MODE_LEFT_FRONT, LEDC_CHANNEL_LEFT_FRONT, duty);
    ledc_update_duty(LEDC_MODE_LEFT_FRONT, LEDC_CHANNEL_LEFT_FRONT);
    
    ledc_set_duty(LEDC_MODE_RIGHT_FRONT, LEDC_CHANNEL_RIGHT_FRONT, duty);
    ledc_update_duty(LEDC_MODE_RIGHT_FRONT, LEDC_CHANNEL_RIGHT_FRONT);
    
    ledc_set_duty(LEDC_MODE_LEFT_BACK, LEDC_CHANNEL_LEFT_BACK, 0);
    ledc_update_duty(LEDC_MODE_LEFT_BACK, LEDC_CHANNEL_LEFT_BACK);
    
    ledc_set_duty(LEDC_MODE_RIGHT_BACK, LEDC_CHANNEL_RIGHT_BACK, 0);
    ledc_update_duty(LEDC_MODE_RIGHT_BACK, LEDC_CHANNEL_RIGHT_BACK);
}
// void go_left_pwm(uint8_t duty){
//     ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, duty);
//     ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
// }
// void go_right_pwm(uint8_t duty){
//     ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, duty);
//     ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
// }
// void go_back_pwm(uint8_t duty){
//     ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3, duty);
//     ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3);
// }
void stop_pwm(){
    ledc_set_duty(LEDC_MODE_LEFT_FRONT, LEDC_CHANNEL_LEFT_FRONT, 0);
    ledc_update_duty(LEDC_MODE_LEFT_FRONT, LEDC_CHANNEL_LEFT_FRONT);
    
    ledc_set_duty(LEDC_MODE_RIGHT_FRONT, LEDC_CHANNEL_RIGHT_FRONT, 0);
    ledc_update_duty(LEDC_MODE_RIGHT_FRONT, LEDC_CHANNEL_RIGHT_FRONT);
    
    ledc_set_duty(LEDC_MODE_LEFT_BACK, LEDC_CHANNEL_LEFT_BACK, 0);
    ledc_update_duty(LEDC_MODE_LEFT_BACK, LEDC_CHANNEL_LEFT_BACK);
    
    ledc_set_duty(LEDC_MODE_RIGHT_BACK, LEDC_CHANNEL_RIGHT_BACK, 0);
    ledc_update_duty(LEDC_MODE_RIGHT_BACK, LEDC_CHANNEL_RIGHT_BACK);
}

void set_all_duty(){
    if ( abs(y_axis)<10 || abs(x_axis)<10 )
        stop_pwm();
    else {
        printf("Andando!\n");

        uint8_t duty_x = (abs(x_axis)*2) + 40;
        uint8_t duty_y = (abs(y_axis)*2) + 40;

        float gain = y_axis;
        uint8_t left  = -10;
        uint8_t right =  10;

        if (x_axis>0 && y_axis>0){
            ledc_set_duty(LEDC_MODE_LEFT_FRONT, LEDC_CHANNEL_LEFT_FRONT, 0);
            ledc_update_duty(LEDC_MODE_LEFT_FRONT, LEDC_CHANNEL_LEFT_FRONT);
            
            ledc_set_duty(LEDC_MODE_RIGHT_FRONT, LEDC_CHANNEL_RIGHT_FRONT, 0);
            ledc_update_duty(LEDC_MODE_RIGHT_FRONT, LEDC_CHANNEL_RIGHT_FRONT);
            
            ledc_set_duty(LEDC_MODE_LEFT_BACK, LEDC_CHANNEL_LEFT_BACK, 0);
            ledc_update_duty(LEDC_MODE_LEFT_BACK, LEDC_CHANNEL_LEFT_BACK);
            
            ledc_set_duty(LEDC_MODE_RIGHT_BACK, LEDC_CHANNEL_RIGHT_BACK, 0);
            ledc_update_duty(LEDC_MODE_RIGHT_BACK, LEDC_CHANNEL_RIGHT_BACK);
        }
        else if (x_axis>0 && y_axis<0){
            ledc_set_duty(LEDC_MODE_LEFT_FRONT, LEDC_CHANNEL_LEFT_FRONT, 0);
            ledc_update_duty(LEDC_MODE_LEFT_FRONT, LEDC_CHANNEL_LEFT_FRONT);
            
            ledc_set_duty(LEDC_MODE_RIGHT_FRONT, LEDC_CHANNEL_RIGHT_FRONT, 0);
            ledc_update_duty(LEDC_MODE_RIGHT_FRONT, LEDC_CHANNEL_RIGHT_FRONT);
            
            ledc_set_duty(LEDC_MODE_LEFT_BACK, LEDC_CHANNEL_LEFT_BACK, 0);
            ledc_update_duty(LEDC_MODE_LEFT_BACK, LEDC_CHANNEL_LEFT_BACK);
            
            ledc_set_duty(LEDC_MODE_RIGHT_BACK, LEDC_CHANNEL_RIGHT_BACK, 0);
            ledc_update_duty(LEDC_MODE_RIGHT_BACK, LEDC_CHANNEL_RIGHT_BACK);
        }
        else if (x_axis<0 && y_axis>0){
            ledc_set_duty(LEDC_MODE_LEFT_FRONT, LEDC_CHANNEL_LEFT_FRONT, 0);
            ledc_update_duty(LEDC_MODE_LEFT_FRONT, LEDC_CHANNEL_LEFT_FRONT);
            
            ledc_set_duty(LEDC_MODE_RIGHT_FRONT, LEDC_CHANNEL_RIGHT_FRONT, 0);
            ledc_update_duty(LEDC_MODE_RIGHT_FRONT, LEDC_CHANNEL_RIGHT_FRONT);
            
            ledc_set_duty(LEDC_MODE_LEFT_BACK, LEDC_CHANNEL_LEFT_BACK, 0);
            ledc_update_duty(LEDC_MODE_LEFT_BACK, LEDC_CHANNEL_LEFT_BACK);
            
            ledc_set_duty(LEDC_MODE_RIGHT_BACK, LEDC_CHANNEL_RIGHT_BACK, 0);
            ledc_update_duty(LEDC_MODE_RIGHT_BACK, LEDC_CHANNEL_RIGHT_BACK);
        }
        else if (x_axis<0 && y_axis<0){
            ledc_set_duty(LEDC_MODE_LEFT_FRONT, LEDC_CHANNEL_LEFT_FRONT, 0);
            ledc_update_duty(LEDC_MODE_LEFT_FRONT, LEDC_CHANNEL_LEFT_FRONT);
            
            ledc_set_duty(LEDC_MODE_RIGHT_FRONT, LEDC_CHANNEL_RIGHT_FRONT, 0);
            ledc_update_duty(LEDC_MODE_RIGHT_FRONT, LEDC_CHANNEL_RIGHT_FRONT);
            
            ledc_set_duty(LEDC_MODE_LEFT_BACK, LEDC_CHANNEL_LEFT_BACK, 0);
            ledc_update_duty(LEDC_MODE_LEFT_BACK, LEDC_CHANNEL_LEFT_BACK);
            
            ledc_set_duty(LEDC_MODE_RIGHT_BACK, LEDC_CHANNEL_RIGHT_BACK, 0);
            ledc_update_duty(LEDC_MODE_RIGHT_BACK, LEDC_CHANNEL_RIGHT_BACK);
        }
        
        // if (x_axis>0){
        //     ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty_x);
        //     ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        //     ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 0);
        //     ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
        // }
        // else{
        //     ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
        //     ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        //     ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, duty_x);
        //     ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
        // }
        // if (y_axis>0){
        //     ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, duty_y);
        //     ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
        //     ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3, 0);
        //     ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3);
        // }
        // else{
        //     ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 0);
        //     ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
        //     ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3, duty_y);
        //     ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_3);
        // }
        

        // -----------------------------------------------
        // Mapeia os valores de 0 a 100 pra máximo de Duty
        // -----------------------------------------------
    }
}

void motor_control_TASK()
{
    init_motor();
    stop_pwm();

    while(1) {
        switch(current_status){
            case 2:
                stop_pwm();
            break;
            case 1:
                // printf("ligando 1 com o duty de: %d\n", y_axis);
                set_all_duty();
            break;
            case 0: 
                // printf("ligando 0 com o duty de: %d\n", y_axis);
                set_all_duty();
            break;
            
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void each_full_power()
{
    init_motor();

    // uint32_t on_time = DELAY_ON;
    // uint32_t off_time = DELAY_ON;
    // uint32_t multiple = 2;
    uint32_t on_on = 50;
    printf("Começando loop!\n");

    while (1) {
        printf("GO STRAIGHT!\n");
        go_straight_pwm(255);
        vTaskDelay(DELAY_ON / portTICK_PERIOD_MS);
        stop_pwm();

        printf("GO LEFT!\n");
        // go_left_pwm(255);
        vTaskDelay(DELAY_ON / portTICK_PERIOD_MS);
        stop_pwm();

        printf("GO BACK!\n");
        // go_back_pwm(255);
        vTaskDelay(DELAY_ON / portTICK_PERIOD_MS);
        stop_pwm();

        printf("GO RIGHT!\n");
        // go_right_pwm(255);
        vTaskDelay(DELAY_ON / portTICK_PERIOD_MS);
        stop_pwm();
    }
    printf("Acabou o loop!\n");
}