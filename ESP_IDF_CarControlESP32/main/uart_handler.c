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
#include <math.h>
#include "uart_handler.h"
#include "encoder_isr.h"

uint8_t current_state = 0;

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
        .baud_rate = ECU_COM_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);
    uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);
}

void send_speed_to_ECU()
{
    // Converts float to string
    char str[100];
    // const char *tmpSign = (avg_speed < 0) ? "-" : "";
    float tmpVal = (avg_speed < 0) ? -avg_speed : avg_speed;

    int tmpInt1 = tmpVal;                  // Get the integer (678).
    float tmpFrac = tmpVal - tmpInt1;      // Get fraction (0.0123).
    int tmpInt2 = trunc(tmpFrac * 10000);  // Turn into integer (123).

    // Print as parts, note that you need 0-padding for fractional bit.
    sprintf(str, "%d.%04d\n", tmpInt1, tmpInt2);
    printf("\nAvg Speed na Serial: %d   |   %s\n", strlen(str), str);

    // Write data to UART.
    // const char* test_str = "This is a test string.\n";
    uart_write_bytes(UART_NUM_1, (const char*)str, strlen(str));
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
            current_state = data[0]-48;

            printf("Chegou! Len: %d\n", len);
            printf("data: %d %d\n", current_state, data[2]-48);
            printf("current_state: %d\n", current_state);
        }
    }
}