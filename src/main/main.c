
#include <stdio.h>
#include <stdint.h>
#include "nvs_flash.h"

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"

#include "protocol_examples_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <esp_system.h>

#include <sys/param.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_eth.h"

#include "esp_tls_crypto.h"
#include <esp_http_server.h>

const char TAG[] = "WEB-SERVER";

bool post_reception(httpd_req_t *req, char *buf, int size_buf);
void WS_Init(void);

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());
    WS_Init();
}

esp_err_t index_handler(httpd_req_t *req)
{
    httpd_resp_send_chunk(req, "eita", 16);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

esp_err_t set_temperature_handler(httpd_req_t *req)
{
    char *buff = NULL;
    uint32_t size_buff = req->content_len + 1;
    buff = (char *)calloc(size_buff, sizeof(char));

    post_reception(req, buff, size_buff);

    for (int w = 0; w < size_buff; w++)
    {
        printf("%c", buff[w]);
    }

    httpd_resp_send_chunk(req, "eita", 16);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

httpd_uri_t urls[] = {
    {.uri = "/",
     .method = HTTP_GET,
     .handler = index_handler,
     .user_ctx = NULL},
    {
        .uri = "/settemp",
        .method = HTTP_POST,
        .handler = set_temperature_handler,
        .user_ctx = NULL,
    },
    // {
    //     .uri = "/setph",
    //     .method = HTTP_POST,
    //     .handler = set_ph_handler,
    //     .user_ctx = NULL,
    // },
    // {
    //     .uri = "/setlvl",
    //     .method = HTTP_POST,
    //     .handler = set_lvl_handler,
    //     .user_ctx = NULL,
    // },
};

void WS_Init(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    config.max_uri_handlers = 25;
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);

    if (httpd_start(&server, &config) == ESP_OK)
    {
        ESP_LOGI(TAG, "Registering URI handlers");

        uint32_t arr_size = sizeof(urls) / sizeof(urls[0]);
        for (int w = 0; w < arr_size; w++)
        {
            httpd_register_uri_handler(server, &(urls[w]));
        }
        return; // server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return; // NULL;
}

void strShiftLeft(char *string, size_t shiftLen)
{
    memmove(string, string + shiftLen, strlen(string) + 1);
}

bool post_reception(httpd_req_t *req, char *buf, int size_buf)
{
    int ret = 0, remaining = req->content_len;
    ESP_LOGI(TAG, "Size buf: %i", size_buf);

    while (remaining > 0)
    {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf, remaining)) <= 0)
        {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT)
            {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return 0;
        }

        remaining -= ret;

        /* Log data received */
        ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
        ESP_LOGI(TAG, "%.*s", ret, buf);
        ESP_LOGI(TAG, "====================================");
    }

    if (size_buf > 2)
    {
        buf[ret] = '\0';
        char *ch = strstr(buf, "\r\n");

        while (ch != NULL)
        {
            strShiftLeft(ch, 1);
            *ch = '&';
            ch = strstr(buf, "\r\n");
        }
    }

    return 1;
}