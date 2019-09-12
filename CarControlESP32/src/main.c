// /* UART Echo Example

//    This example code is in the Public Domain (or CC0 licensed, at your option.)

//    Unless required by applicable law or agreed to in writing, this
//    software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
//    CONDITIONS OF ANY KIND, either express or implied.
// */
// #include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "driver/uart.h"

// #include "driver/gpio.h"

// #include <stdint.h>

// #include <string.h>

// #include "definitions.h"

// /**
//  * This is an example which echos any data it receives on UART1 back to the sender,
//  * with hardware flow control turned off. It does not use UART driver event queue.
//  *
//  * - Port: UART1
//  * - Receive (Rx) buffer: on
//  * - Transmit (Tx) buffer: off
//  * - Flow control: off
//  * - Event queue: off
//  * - Pin assignment: see defines below
//  */

// #define ECHO_TEST_TXD  (GPIO_NUM_4)
// #define ECHO_TEST_RXD  (GPIO_NUM_5)
// #define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
// #define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)

// #define BUF_SIZE (1024)

// #define BLINK_GPIO 2
// #define BLINK_DELAY 300


// #define GPIO_RASP_COLLISION GPIO_NUM_34
// #define GPIO_RASP_AUTOBRAKE GPIO_NUM_35

// #define GPIO_BUZZER GPIO_NUM_33

// uint8_t current_status = 0;

// static void adas_perception()
// {
//     // RASP
//     gpio_pad_select_gpio(GPIO_RASP_COLLISION);
//     gpio_set_direction(GPIO_RASP_COLLISION, GPIO_MODE_INPUT);
//     gpio_pad_select_gpio(GPIO_RASP_AUTOBRAKE);
//     gpio_set_direction(GPIO_RASP_AUTOBRAKE, GPIO_MODE_INPUT);

//     while (1) {
//         if (gpio_get_level(GPIO_RASP_AUTOBRAKE)){
//             printf("AUTOBRAKE!\n");
//             current_status = 2;
//         }
//         else if (gpio_get_level(GPIO_RASP_COLLISION)){
//             printf("COLLISION!\n");
//             current_status = 1;
//         }
//         else{
//             printf("SEGUE!\n");
//             current_status = 0;
//         }     
//     }
// }

// void buzzer_TASK()
// {
//     // BUZZER
//     gpio_pad_select_gpio(GPIO_BUZZER);
//     gpio_set_direction(GPIO_BUZZER, GPIO_MODE_OUTPUT);
//     gpio_set_level(GPIO_BUZZER, 0);
    
//     // while(1){
//     //     switch(current_status){
//     //         case 0:
//     //             gpio_set_level(GPIO_BUZZER, 0);
//     //         break;
//     //         case 1:
//     //             gpio_set_level(GPIO_BUZZER, 1);
//     //         break;
//     //         case 2:
//     //             gpio_set_level(GPIO_BUZZER, 1);
//     //         break;
//     //     }
//     // }
// }



// // static void echo_task()
// // {
// //     /* Configure parameters of an UART driver,
// //      * communication pins and install the driver */
// //     uart_config_t uart_config = {
// //         .baud_rate = 9600,
// //         .data_bits = UART_DATA_8_BITS,
// //         .parity    = UART_PARITY_DISABLE,
// //         .stop_bits = UART_STOP_BITS_1,
// //         .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
// //     };
// //     uart_param_config(UART_NUM_1, &uart_config);
// //     uart_set_pin(UART_NUM_1, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);
// //     uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);

// //     // Configure a temporary buffer for the incoming data
// //     uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
    
// //     while (1) {
// //         // Read data from the UART
// //         int len = uart_read_bytes(UART_NUM_1, data, BUF_SIZE, 20 / portTICK_RATE_MS);
// //         // Write data back to the UART
// //         uart_write_bytes(UART_NUM_1, (const char *) data, len);

// //         if (len > 0){
// //             printf("Chegou!\n");

// //             char* bt_data = 0;
// //             bt_data = (char*)malloc(len);
// //             memcpy(bt_data,data,len);
// //             bt_data[len-1] = '\0'; 
// //             printf("%s\n", bt_data);

// //             char bt_data_char = (char) bt_data[0];

// //             if (bt_data_char == '3'){
// //                 printf("LEFT!\n");
// //                 go_left();
// //                 vTaskDelay(DELAY_ON / portTICK_PERIOD_MS);
// //                 stop();
// //             }
// //             else if (bt_data_char == '4'){
// //                 printf("RIGHT!\n");
// //                 go_right();
// //                 vTaskDelay(DELAY_ON / portTICK_PERIOD_MS);
// //                 stop();
// //             }
// //             else if (bt_data_char == '1'){
// //                 printf("STRAIGHT!\n");
// //                 go_straight();
// //                 vTaskDelay(DELAY_ON / portTICK_PERIOD_MS);
// //                 stop();
// //             }
// //             else if (bt_data_char == '2'){
// //                 printf("BACK!\n");
// //                 go_back();
// //                 vTaskDelay(DELAY_ON / portTICK_PERIOD_MS);
// //                 stop();
// //             }
            
// //             printf("-------------------\n");
// //         }
// //     }
// // }

// void blink_task(void *pvParameter)
// {
//    // LED
//     gpio_pad_select_gpio(BLINK_GPIO);
//     gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    
//     while(1) {
//         /* Blink off (output low) */
//         gpio_set_level(BLINK_GPIO, 0);
//         vTaskDelay(BLINK_DELAY / portTICK_PERIOD_MS);
//         /* Blink on (output high) */
//         gpio_set_level(BLINK_GPIO, 1);
//         vTaskDelay(BLINK_DELAY / portTICK_PERIOD_MS);
//     }
// }

// void app_main()
// {
//     // xTaskCreate(&blink_task, "blink_task", 1024, NULL, 5, NULL);
//     // xTaskCreate(echo_task, "uart_echo_task", 1024, NULL, 10, NULL);
//     // xTaskCreate(each_full_power, "motor_test", 1024, NULL, 6, NULL);
//     xTaskCreate(adas_perception, "rasp_integration", 1024, NULL, 2, NULL);
//     // xTaskCreate(buzzer_TASK, "buzzer_control", 1024, NULL, 3, NULL);
//     xTaskCreate(motor_control_TASK, "motor_control", 1024, NULL, 1, NULL);
// }

/* LEDC (LED Controller) fade example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"

/*
 * About this example
 *
 * 1. Start with initializing LEDC module:
 *    a. Set the timer of LEDC first, this determines the frequency
 *       and resolution of PWM.
 *    b. Then set the LEDC channel you want to use,
 *       and bind with one of the timers.
 *
 * 2. You need first to install a default fade function,
 *    then you can use fade APIs.
 *
 * 3. You can also set a target duty directly without fading.
 *
 * 4. This example uses GPIO18/19/4/5 as LEDC output,
 *    and it will change the duty repeatedly.
 *
 * 5. GPIO18/19 are from high speed channel group.
 *    GPIO4/5 are from low speed channel group.
 *
 */
#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_HS_CH0_GPIO       (18)
#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0
#define LEDC_HS_CH1_GPIO       (19)
#define LEDC_HS_CH1_CHANNEL    LEDC_CHANNEL_1

#define LEDC_LS_TIMER          LEDC_TIMER_1
#define LEDC_LS_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_LS_CH2_GPIO       (4)
#define LEDC_LS_CH2_CHANNEL    LEDC_CHANNEL_2
#define LEDC_LS_CH3_GPIO       (5)
#define LEDC_LS_CH3_CHANNEL    LEDC_CHANNEL_3

#define LEDC_TEST_CH_NUM       (4)
#define LEDC_TEST_DUTY         (4000)
#define LEDC_TEST_FADE_TIME    (3000)
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/xtensa_api.h"
#include "driver/ledc.h"
#include "esp_attr.h"   
#include "esp_err.h"

void app_main()
// esp_err_t app_main() 
{
 
    ledc_timer_config_t ledc_timer = {
       .bit_num = LEDC_TIMER_8_BIT,
        .freq_hz = 50,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel0 = {
        .channel = LEDC_CHANNEL_0,
        .duty = 0,
        .gpio_num = LEDC_HS_CH0_GPIO,
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_sel = LEDC_TIMER_0
    };

    ledc_channel_config_t ledc_channel1 = {
        .channel = LEDC_CHANNEL_1,
        .duty = 0,
        .gpio_num = LEDC_HS_CH1_GPIO,
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_sel = LEDC_TIMER_0
    };
    
    ledc_channel_config(&ledc_channel0);
    ledc_channel_config(&ledc_channel1);

    int up_or_down = 1;
    int dutyduty = 0;  
    int resolution = 255;  
    while(1) {
        if(up_or_down)  dutyduty++;
        else    dutyduty--;
            
        if(dutyduty >= resolution)   up_or_down = 0;
        else if(dutyduty <= 0)  up_or_down = 1;

        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, dutyduty);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        vTaskDelay(10 / portTICK_PERIOD_MS);

        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, resolution-dutyduty);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
        vTaskDelay(10 / portTICK_PERIOD_MS);

        // ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 50);
        // ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        // vTaskDelay(1000 / portTICK_PERIOD_MS);

        // ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 100);
        // ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        // vTaskDelay(1000 / portTICK_PERIOD_MS);

        // ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 150);
        // ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        // vTaskDelay(1000 / portTICK_PERIOD_MS);

        // ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 200);
        // ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        // vTaskDelay(1000 / portTICK_PERIOD_MS);

        // ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 256);
        // ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        // vTaskDelay(1000 / portTICK_PERIOD_MS);

        // ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 270);
        // ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        // vTaskDelay(1000 / portTICK_PERIOD_MS);

        // ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
        // ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        // vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}