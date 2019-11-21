/* GPIO Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "uart_handler.h"
#include "encoder_isr.h"

xQueueHandle encoder_evt_queue = NULL;

uint32_t cont_left_encoder = 0;
uint32_t cont_right_encoder = 0;
uint32_t current_time;

static void IRAM_ATTR encoder_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    
    if (gpio_num == GPIO_INPUT_LEFT_ENCODER)
        cont_left_encoder++;
    else if (gpio_num == GPIO_INPUT_RIGHT_ENCODER)
        cont_right_encoder++;
}

void set_encoder_ISRs(void)
{
    gpio_config_t io_conf;
    
    //interrupt of rising edge
    io_conf.intr_type = GPIO_PIN_INTR_NEGEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = ((1ULL<<GPIO_INPUT_LEFT_ENCODER) | (1ULL<<GPIO_INPUT_RIGHT_ENCODER));
    //set as input mode    
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-down mode
    io_conf.pull_down_en = 1;
    gpio_config(&io_conf);

    //create a queue to handle gpio event from isr
    encoder_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_LEFT_ENCODER, encoder_isr_handler, (void*) GPIO_INPUT_LEFT_ENCODER);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_RIGHT_ENCODER, encoder_isr_handler, (void*) GPIO_INPUT_RIGHT_ENCODER);
}

void get_rms_task(void* arg)
{
    float left_rps = 0;
    float right_rps = 0;
    float left_speed = 0;
    float right_speed = 0;

    for(;;) {
        // Calcula o RPM
        // left_rps = ((float)DELAY_CALCULATE_RPM / PULSES_PER_REVOLUTION) * cont_left_encoder; 
        // left_rps = (float)(cont_left_encoder / PULSES_PER_REVOLUTION) * DELAY_CALCULATE_RPM; 
        left_rps = 1000.0 * cont_left_encoder / DELAY_CALCULATE_RPM / PULSES_PER_REVOLUTION;
        right_rps = 1000.0 * cont_right_encoder / DELAY_CALCULATE_RPM / PULSES_PER_REVOLUTION;
        left_speed = left_rps * WHEEL_CIRCUMFERENCE;
        right_speed = right_rps * WHEEL_CIRCUMFERENCE;
        printf("LEFT  ENCODER: %d \n", cont_left_encoder);
        printf("RIGHT ENCODER: %d \n", cont_right_encoder);
        printf("left  RPM: %f \n", left_rps);
        printf("right RPM: %f \n", right_rps);
        printf("left  SPEED: %f cm/s \n", left_speed);
        printf("right SPEED: %f cm/s \n", right_speed);
        printf("-----------------\n");

        send_speed_to_ECU(left_rps);
        
        cont_left_encoder = 0;
        cont_right_encoder = 0;

        vTaskDelay(DELAY_CALCULATE_RPM / portTICK_PERIOD_MS);
    }
}