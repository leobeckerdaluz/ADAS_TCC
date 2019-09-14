void motor_control_TASK();
void joystick_bluetooth_TASK();
void ADAS_listener_TASK();
void buzzer_TASK();
void each_full_power();

extern uint8_t current_status;
extern int8_t x_axis, y_axis;

#define BLINK_GPIO 2
#define BLINK_DELAY 300

#define GPIO_RASP_COLLISION GPIO_NUM_34
#define GPIO_RASP_AUTOBRAKE GPIO_NUM_35
#define GPIO_BUZZER GPIO_NUM_33

// UART
#define ECHO_TEST_TXD  (GPIO_NUM_4)
#define ECHO_TEST_RXD  (GPIO_NUM_5)
#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)

#define BUF_SIZE (1024)

#define X_AXIS_START_POS 1
#define X_AXIS__END__POS 3
#define Y_AXIS_START_POS 4
#define Y_AXIS__END__POS 6
#define INT_SUBSTRING_OFFSET -200

// LEDC
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