
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define GPIO_INPUT_LEFT_ENCODER     34
#define GPIO_INPUT_RIGHT_ENCODER    35
#define ESP_INTR_FLAG_DEFAULT       0

#define DELAY_CALCULATE_RPM 50
#define PULSES_PER_REVOLUTION 40.0
#define WHEEL_CIRCUMFERENCE 21.36283 //2*PI*3.4

xQueueHandle encoder_evt_queue;
extern float avg_speed;

void set_encoder_ISRs();
void encoder_queue_isr_handler_task(void* arg);
void get_rms_task(void* arg);