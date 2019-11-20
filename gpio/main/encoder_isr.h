
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define GPIO_INPUT_LEFT_ENCODER     4
#define GPIO_INPUT_RIGHT_ENCODER    5
#define ESP_INTR_FLAG_DEFAULT       0

xQueueHandle encoder_evt_queue;

void set_encoder_ISRs();
void encoder_task(void* arg);