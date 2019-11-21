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

#include <string.h>
#include "uart_handler.h"

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

void init_uart1(void){
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
}

void send_speed_to_ECU(float rpm)
{
    // // Configure a temporary buffer for the incoming data
    // uint8_t *data = (uint8_t *) malloc(2);

    // int len = 2;

    // // Write data back to the UART
    // uart_write_bytes(UART_NUM_1, (const char *) data, len);

    // Write data to UART.
    char* test_str = "This is a test string.\n";
    uart_write_bytes(UART_NUM_1, (const char*)test_str, strlen(test_str));
}

void get_ECU_serial_parameters_TASK(void *arg)
{
    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(UART_NUM_1, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        // // Write data back to the UART
        // uart_write_bytes(UART_NUM_1, (const char *) data, len);
        
        if (len>0){
            printf("Chegou! Len: %d\n", len);
            printf("data: %d %d\n", data[0]-48, data[2]-48);
        }
    }
}