#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/xtensa_api.h"
#include "driver/uart.h"

#include "driver/gpio.h"
#include "driver/ledc.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "esp_attr.h"   
#include "esp_err.h"

#include "definitions.h"

int8_t x_axis = 0, y_axis = 0;

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

void substr_to_int(char *data, int8_t *int_value, uint8_t substring_init_pos, uint8_t substring_end_pos){
    // Calcula o tamanho da substring com base nas posições inicial e final
    uint8_t substring_size = (substring_end_pos - substring_init_pos + 2);
    // Aloca o espaço para a substring
    char* bt_data = (char*)malloc(substring_size * sizeof(char));
    // Copia a substring para a variável
    memcpy(bt_data, data + substring_init_pos, substring_size);
    // Adiciona o \0 no fim da string
    bt_data[substring_size - 1] = '\0'; 
    
    // Converte a substring para inteiro
    *int_value = atoi(bt_data) + INT_SUBSTRING_OFFSET;

    // Libera o espaço alocado na memória
    free(bt_data);
}

void joystick_bluetooth_TASK()
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
        // // Write data back to the UART
        // uart_write_bytes(UART_NUM_1, (const char *) data, len);

        if (len > 0){
            printf("Chegou!\n");

            substr_to_int((char*)data, &x_axis, X_AXIS_START_POS, X_AXIS__END__POS);
            substr_to_int((char*)data, &y_axis, Y_AXIS_START_POS, Y_AXIS__END__POS);

            printf("x: %d\n", x_axis);
            printf("y: %d\n", y_axis);

            printf("------------------------\n");
        }
    }
}