#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

// Tasks handles
static TaskHandle_t task1_handle = NULL;

// Tasks
void task1(void *parameter)
{
    int recebidos[10] = {1, 3, 2, 3, 1, 4, 5, 4, 6, 6};
    int enviados[10];
    int repetidos = 0;
    int naoRepetidos = 0;
    for (int i = 0; i < 10; i++)
    {
        int count = 0;
        for (int j = 0; j < i; j++)
        {
            if (recebidos[i] == enviados[j])
            {
                count++;
            }
        }
        if (count > 0)
        {
            repetidos++;
            continue;
        }
        else
        {
            enviados[naoRepetidos] = recebidos[i];
            naoRepetidos++;
        }
    }

    for (int i = 10 - repetidos; i < 10; i++)
    {
        enviados[i] = -1;
    }

    for (int i = 0; i < 10; i++)
    {
        printf("enviados[%d] = %d\n", i, enviados[i]);
    }
    vTaskDelete(task1_handle);
}

void app_main(void)
{

    xTaskCreate(task1, "Task 1", 2048, NULL, 1, &task1_handle);
    // fflush(stdout);
}
