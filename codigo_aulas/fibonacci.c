/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

// Tasks handles
static TaskHandle_t task1_handle = NULL;
static TaskHandle_t task2_handle = NULL;

// Tasks
void task1(void *parameter)
{

    int n1 = 0;
    printf("i = %d\n", n1);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    int n2 = 1;
    printf("i = %d\n", n2);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    int nextN = n1 + n2;

    while (1)
    {
        printf("i = %d\n", nextN);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        n1 = n2;
        n2 = nextN;
        nextN = n1 + n2;
    }
}
void task2(void *parameter)
{
    while (1)
    {
        vTaskDelay(3000 / portTICK_PERIOD_MS);
        vTaskSuspend(task1_handle);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        vTaskResume(task1_handle);
    }
}

void app_main(void)
{
    printf("Fibonacci!\n");
    xTaskCreate(task1, "Task 1", 2048, NULL, 1, &task1_handle);
    xTaskCreate(task2, "Task 2", 2048, NULL, 1, &task2_handle);
    // fflush(stdout);
}
