#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "freertos/queue.h"

// Tasks handles
static TaskHandle_t task1_handle = NULL;
static TaskHandle_t task2_handle = NULL;
static TaskHandle_t task3_handle = NULL;

// Queue handle

static QueueHandle_t fila = NULL;

// Tasks
void task1(void *parameter)
{
    int i = 0;
    while (1)
    {
        if (xQueueSend(fila, &i, 0) == errQUEUE_FULL)
        {
            printf("Fila cheia!\n");
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
        // uxTaskGetStackHighWaterMark(task1_handle);
    }
}

void task2(void *parameter)
{
    int i = 1;
    while (1)
    {
        if (xQueueSend(fila, &i, 0) == errQUEUE_FULL)
        {
            printf("Fila cheia!\n");
        }
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

void task3(void *parameter)
{
    int recebido;
    while (1)
    {
        if (xQueueReceive(fila, &recebido, portMAX_DELAY))
        {
            printf("%d\n", recebido);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    fila = xQueueCreate(10, sizeof(int));
    xTaskCreate(task1, "Task 1", 2048, NULL, 2, &task1_handle);
    xTaskCreate(task2, "Task 2", 2048, NULL, 2, &task2_handle);
    xTaskCreate(task3, "Task 3", 2048, NULL, 1, &task3_handle);
}
