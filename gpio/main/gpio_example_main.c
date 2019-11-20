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

uint32_t clock_millis;

#define GPIO_INPUT_LEFT_ENCODER     4
#define GPIO_INPUT_RIGHT_ENCODER     5
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_LEFT_ENCODER) | (1ULL<<GPIO_INPUT_RIGHT_ENCODER))
#define ESP_INTR_FLAG_DEFAULT 0

static xQueueHandle encoder_evt_queue = NULL;
volatile uint32_t debounceTimeout = 0;
#define DEBOUNCETIME 50

static void IRAM_ATTR encoder_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    uint32_t current_time = xTaskGetTickCount() * portTICK_PERIOD_MS;

    if (current_time - debounceTimeout > DEBOUNCETIME){
        debounceTimeout = current_time;
        xQueueSendFromISR(encoder_evt_queue, &gpio_num, NULL);
    }
}

void set_encoder_ISRs(void)
{
    gpio_config_t io_conf;
    
    //interrupt of rising edge
    io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
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






static void encoder_task(void* arg)
{
    uint32_t io_num;
    for(;;) {
        if(xQueueReceive(encoder_evt_queue, &io_num, portMAX_DELAY)) {
            if (io_num == GPIO_INPUT_LEFT_ENCODER)
                printf("ENCODER LEFT!\n");
            else if (io_num == GPIO_INPUT_RIGHT_ENCODER)
                printf("ENCODER RIGHT!\n");
        }
    }
}

void app_main(void){
    set_encoder_ISRs();

    //start gpio task
    xTaskCreate(encoder_task, "encoder_task", 4096, NULL, 10, NULL);
}