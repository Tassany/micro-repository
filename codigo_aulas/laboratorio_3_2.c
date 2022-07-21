#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_system.h"

// Variables
int pino_int_1 = 12;
int pino_int_2 = 4;

// Tasks handles
static TaskHandle_t task1_handle = NULL;
static TaskHandle_t task2_handle = NULL;

// Queues handles
static QueueHandle_t gpio_isr_queue = NULL;
static QueueHandle_t esp_timer_queue = NULL;

// ISR handles
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    xQueueSendFromISR(gpio_isr_queue, arg, NULL);
}

// Tasks
static void interrupt_task(void *arg)
{
    int pino = 0;
    while (1)
    {
        if (xQueueReceive(gpio_isr_queue, &pino, portMAX_DELAY))
            printf("Interrupção no pino %d!\n", pino);
    }
}

void task1(void *arg)
{
    while (1)
    {
        gpio_set_level(GPIO_NUM_12, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_NUM_12, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void task2(void *arg)
{
    while (1)
    {
        gpio_set_level(GPIO_NUM_4, 1);
        vTaskDelay(250 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_NUM_4, 0);
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{

    gpio_config_t io_conf = {};
    gpio_isr_queue = xQueueCreate(10, sizeof(int));

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << 12) | (1ULL << 4);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << 13) | (1ULL << 5);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_NUM_13, gpio_isr_handler, &pino_int_1);
    gpio_isr_handler_add(GPIO_NUM_5, gpio_isr_handler, &pino_int_2);

    xTaskCreate(task1, "Task 1", 2048, NULL, 2, &task1_handle);
    xTaskCreate(task2, "Task 2", 2048, NULL, 2, &task2_handle);
    xTaskCreate(interrupt_task, "interrupt task", 2048, NULL, 1, NULL);
}
