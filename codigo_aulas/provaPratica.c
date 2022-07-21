#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_timer.h"

//  Tassany Onofre e Weslley Gomes Dantas

// Varivavéis globais
int pino_int_1 = 12;
int pino_int_2 = 13;
int pino_int_3 = 14;

// Task handles
static TaskHandle_t task1_handle = NULL;

// Queues handles
static QueueHandle_t gpio_isr_queue = NULL;
static QueueHandle_t esp_timer_queue = NULL;

// ISR handles
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    xQueueSendFromISR(gpio_isr_queue, arg, NULL);
}

// Tasks

static void periodic_timer_callback(void *arg)
{
    int i = 0;
    xQueueSendFromISR(esp_timer_queue, &i, NULL);
}

static void esp_timer_task(void *arg)
{
    bool boolean = true;
    int i = 0;
    while (1)
    {
        if (xQueueReceive(esp_timer_queue, &i, portMAX_DELAY))
        {
            if (boolean)
            {
                gpio_set_level(GPIO_NUM_2, 1);
                gpio_set_level(GPIO_NUM_4, 0);
                printf("GP2 1 GP4 0\n");
                boolean = false;
            }
            else
            {
                gpio_set_level(GPIO_NUM_2, 0);
                gpio_set_level(GPIO_NUM_4, 1);
                printf("GP2 0 GP4 1\n");
                boolean = true;
            }
        }
    }
}

static void interrupt_task(void *arg)
{
    int pino = 0;
    while (1)
    {
        if (xQueueReceive(gpio_isr_queue, &pino, portMAX_DELAY))
        {

            esp_timer_create_args_t periodic_timer_args = {
                .callback = &periodic_timer_callback,
                .name = "periodic"};
            esp_timer_handle_t periodic_timer;
            esp_timer_create(&periodic_timer_args, &periodic_timer);

            esp_timer_queue = xQueueCreate(10, sizeof(int));

            if (pino == 12)
            {
                esp_timer_start_periodic(periodic_timer, 250150);
                xTaskCreate(esp_timer_task, "esp timer task", 2048, NULL, 3, NULL);
                vTaskDelay(10000 / portTICK_PERIOD_MS);
                vTaskDelete(esp_timer_task);
            }
            if (pino == 13)
            {
                esp_timer_start_periodic(periodic_timer, 500300);
                xTaskCreate(esp_timer_task, "esp timer task", 2048, NULL, 3, NULL);
                vTaskDelay(10000 / portTICK_PERIOD_MS);
                vTaskDelete(esp_timer_task);
            }
            if (pino == 14)
            {
                esp_timer_start_periodic(periodic_timer, 990600);
                xTaskCreate(esp_timer_task, "esp timer task", 2048, NULL, 3, NULL);
                vTaskDelay(10000 / portTICK_PERIOD_MS);
                vTaskDelete(esp_timer_task);
            }
        }
    }
}

// Pulso a cada 15 segundos no GPIO5
void task1(void *parameter)
{
    while (1)
    {
        gpio_set_level(GPIO_NUM_5, 1);
    vTaskDelay(50 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_NUM_5, 0);
        vTaskDelay(14950 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{

    gpio_isr_queue = xQueueCreate(10, sizeof(int));

    gpio_config_t io_conf;

    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << 12) | (1ULL << 13) | (1ULL << 14);
    io_conf.pull_down_en = 1;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << 4) | (1ULL << 2) | (1ULL << 5);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    gpio_set_level(GPIO_NUM_2, 0);
    gpio_set_level(GPIO_NUM_4, 0);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_NUM_12, gpio_isr_handler, &pino_int_1);
    gpio_isr_handler_add(GPIO_NUM_13, gpio_isr_handler, &pino_int_2);
    gpio_isr_handler_add(GPIO_NUM_14, gpio_isr_handler, &pino_int_3);

    xTaskCreate(interrupt_task, "Interrupt Task", 2048, NULL, 4, NULL);
    xTaskCreate(task1, "Pulsos", 2048, NULL, 2, &task1_handle);
}
