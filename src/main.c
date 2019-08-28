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

#include <string.h>

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



#define LEFT_FRONT 27
#define RIGHT_FRONT 26
#define LEFT_BACK 25
#define RIGHT_BACK 33
#define DELAY_ON 1000



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
    

    gpio_pad_select_gpio(LEFT_FRONT);
    gpio_set_direction(LEFT_FRONT, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(RIGHT_FRONT);
    gpio_set_direction(RIGHT_FRONT, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(LEFT_BACK);
    gpio_set_direction(LEFT_BACK, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(RIGHT_BACK);
    gpio_set_direction(RIGHT_BACK, GPIO_MODE_OUTPUT);

    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(UART_NUM_1, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        // Write data back to the UART
        uart_write_bytes(UART_NUM_1, (const char *) data, len);

        if (len > 0){
            int i = 0;

            printf("Chegou!\n");

            char* bt_data = 0;
            bt_data = (char*)malloc(len);
            memcpy(bt_data,data,len);
            bt_data[len-1] = '\0'; 
            printf("%s\n", bt_data);

            char bt_data_char = (char) bt_data[0];

            if (bt_data_char == '3'){
                printf("LEFT!\n");
                gpio_set_level(LEFT_BACK, 1);
                gpio_set_level(RIGHT_FRONT, 1);
                vTaskDelay(DELAY_ON / portTICK_PERIOD_MS);
                gpio_set_level(LEFT_BACK, 0);
                gpio_set_level(RIGHT_FRONT, 0);
            }
            else if (bt_data_char == '4'){
                printf("RIGHT!\n");
                gpio_set_level(RIGHT_BACK, 1);
                gpio_set_level(LEFT_FRONT, 1);
                vTaskDelay(DELAY_ON / portTICK_PERIOD_MS);
                gpio_set_level(RIGHT_BACK, 0);
                gpio_set_level(LEFT_FRONT, 0);
            }
            else if (bt_data_char == '1'){
                printf("STRAIGHT!\n");
                gpio_set_level(LEFT_FRONT, 1);
                gpio_set_level(RIGHT_FRONT, 1);
                vTaskDelay(DELAY_ON / portTICK_PERIOD_MS);
                gpio_set_level(LEFT_FRONT, 0);
                gpio_set_level(RIGHT_FRONT, 0);
            }
            else if (bt_data_char == '2'){
                printf("BACK!\n");
                gpio_set_level(LEFT_BACK, 1);
                gpio_set_level(RIGHT_BACK, 1);
                vTaskDelay(DELAY_ON / portTICK_PERIOD_MS);
                gpio_set_level(LEFT_BACK, 0);
                gpio_set_level(RIGHT_BACK, 0);
            }
            
            printf("-------------------\n");
        }
    }
}

void blink_task(void *pvParameter)
{
    #define BLINK_GPIO 2
    #define BLINK_DELAY 300

    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
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
    // xTaskCreate(&blink_task, "blink_task", 1024, NULL, 5, NULL);
    xTaskCreate(echo_task, "uart_echo_task", 1024, NULL, 10, NULL);
}
