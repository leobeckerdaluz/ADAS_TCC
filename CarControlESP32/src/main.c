/* UART Echo Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"

#include "driver/gpio.h"

#include <stdint.h>

#include <string.h>

#include "definitions.h"

/**
 * This is an example which echos any data it receives on UART1 back to the sender,
 * with hardware flow control turned off. It does not use UART driver event queue.
 *
 * - Port: UART1
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: off
 * - Pin assignment: see defines below
 */

#define ECHO_TEST_TXD  (GPIO_NUM_4)
#define ECHO_TEST_RXD  (GPIO_NUM_5)
#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)

#define BUF_SIZE (1024)

#define BLINK_GPIO 2
#define BLINK_DELAY 300
#define DELAY_ON 1000

#define GPIO_RASP_COLLISION GPIO_NUM_35
#define GPIO_RASP_AUTOBRAKE GPIO_NUM_34

static void ultrassonico_test()
{
    while (1) {
        if (gpio_get_level(GPIO_RASP_COLLISION)){
            printf("COLLISION!\n");
            
            go_left();
        }
        else if (gpio_get_level(GPIO_RASP_AUTOBRAKE)){
            printf("AUTOBRAKE!\n");

            stop();
        }
        else{
            printf("SEGUE!\n");

            go_straight();
        }     
    }
}


static void each_full_power()
{
    while (1) {
        printf("Começando loop!\n");
        for (int i=0; i<4; i++)
        {
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

static void echo_task()
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);
    uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);

    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
    
    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(UART_NUM_1, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        // Write data back to the UART
        uart_write_bytes(UART_NUM_1, (const char *) data, len);

        if (len > 0){
            printf("Chegou!\n");

            char* bt_data = 0;
            bt_data = (char*)malloc(len);
            memcpy(bt_data,data,len);
            bt_data[len-1] = '\0'; 
            printf("%s\n", bt_data);

            char bt_data_char = (char) bt_data[0];

            if (bt_data_char == '3'){
                printf("LEFT!\n");
                go_left();
                vTaskDelay(DELAY_ON / portTICK_PERIOD_MS);
                stop();
            }
            else if (bt_data_char == '4'){
                printf("RIGHT!\n");
                go_right();
                vTaskDelay(DELAY_ON / portTICK_PERIOD_MS);
                stop();
            }
            else if (bt_data_char == '1'){
                printf("STRAIGHT!\n");
                go_straight();
                vTaskDelay(DELAY_ON / portTICK_PERIOD_MS);
                stop();
            }
            else if (bt_data_char == '2'){
                printf("BACK!\n");
                go_back();
                vTaskDelay(DELAY_ON / portTICK_PERIOD_MS);
                stop();
            }
            
            printf("-------------------\n");
        }
    }
}

void blink_task(void *pvParameter)
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    
    while(1) {
        /* Blink off (output low) */
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(BLINK_DELAY / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(BLINK_DELAY / portTICK_PERIOD_MS);
    }
}

void app_main()
{
    // LED
    gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    // RASP
    gpio_pad_select_gpio(GPIO_RASP_COLLISION);
    gpio_set_direction(GPIO_RASP_COLLISION, GPIO_MODE_INPUT);
    gpio_pad_select_gpio(GPIO_RASP_AUTOBRAKE);
    gpio_set_direction(GPIO_RASP_AUTOBRAKE, GPIO_MODE_INPUT);

    init_motor_pins();

    // xTaskCreate(&blink_task, "blink_task", 1024, NULL, 5, NULL);
    // xTaskCreate(echo_task, "uart_echo_task", 1024, NULL, 10, NULL);
    xTaskCreate(each_full_power, "motor_test", 1024, NULL, 6, NULL);
    // xTaskCreate(ultrassonico_test, "rasp_integration", 1024, NULL, 6, NULL);
}
