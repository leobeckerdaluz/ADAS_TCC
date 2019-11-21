#define ECHO_TEST_TXD  (GPIO_NUM_4)
#define ECHO_TEST_RXD  (GPIO_NUM_5)
#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)

#define BUF_SIZE (1024)

void init_uart1();
void get_ECU_serial_parameters_TASK(void *arg);
void send_speed_to_ECU(float avg_speed);