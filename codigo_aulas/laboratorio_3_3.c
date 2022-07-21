#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_timer.h"

static QueueHandle_t esp_timer_queue = NULL;

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
                boolean = false;
            }
            else
            {
                gpio_set_level(GPIO_NUM_2, 0);
                boolean = true;
            }
        }
    }
}

void app_main(void)
{
    esp_timer_create_args_t periodic_timer_args = {
        .callback = &periodic_timer_callback,
        .name = "periodic"};
    esp_timer_handle_t periodic_timer;
    esp_timer_create(&periodic_timer_args, &periodic_timer);
    esp_timer_start_periodic(periodic_timer, 250000);

    esp_timer_queue = xQueueCreate(10, sizeof(int));

    gpio_config_t io_conf;

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = 1ULL << 2;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;

    gpio_config(&io_conf);

    xTaskCreate(esp_timer_task, "esp timer task", 2048, NULL, 1, NULL);
}
