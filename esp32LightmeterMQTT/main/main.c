#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/i2c.h"
#include "mqtt_client.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"

// Configurações I2C para o BH1750
#define I2C_MASTER_SCL_IO 22           // GPIO para SCL
#define I2C_MASTER_SDA_IO 21           // GPIO para SDA
#define I2C_MASTER_FREQ_HZ 100000      // Frequência I2C
#define I2C_MASTER_PORT I2C_NUM_0      // Porta I2C
#define BH1750_SENSOR_ADDR 0x23        // Endereço I2C do BH1750
#define BH1750_CMD_START 0x10          // Modo de medição contínua de alta resolução

// Configurações MQTT
#define MQTT_BROKER_URI "mqtt://test.mosquitto.org:1883"
#define MQTT_TOPIC "vshaft/sensor/light"

// Configurações Wi-Fi
#define WIFI_SSID "SUA REDE WIFI"
#define WIFI_PASSWORD "senha do WIFI"

// Tags de log
static const char *TAG_MQTT = "MQTT";
static const char *TAG_I2C = "I2C";

// Variável para conexão Wi-Fi
static EventGroupHandle_t wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0

// Função de callback do Wi-Fi
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

// Inicialização do Wi-Fi
static void wifi_init() {
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL, NULL);
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

// Inicialização do I2C
static esp_err_t i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_PORT, &conf));
    return i2c_driver_install(I2C_MASTER_PORT, conf.mode, 0, 0, 0);
}

// Função para leitura do BH1750
static uint16_t bh1750_read_light() {
    uint8_t data[2];
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BH1750_SENSOR_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, BH1750_CMD_START, true);
    i2c_master_stop(cmd);
    ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_MASTER_PORT, cmd, pdMS_TO_TICKS(1000)));
    i2c_cmd_link_delete(cmd);

    vTaskDelay(pdMS_TO_TICKS(180));

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BH1750_SENSOR_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, 2, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_MASTER_PORT, cmd, pdMS_TO_TICKS(1000)));
    i2c_cmd_link_delete(cmd);

    return (data[0] << 8) | data[1];
}

// Callback MQTT
static void mqtt_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_id == MQTT_EVENT_CONNECTED) {
        ESP_LOGI(TAG_MQTT, "Conectado ao broker MQTT");
    } else if (event_id == MQTT_EVENT_DISCONNECTED) {
        ESP_LOGI(TAG_MQTT, "Desconectado do broker MQTT");
    }
}

// Inicialização do cliente MQTT
static esp_mqtt_client_handle_t mqtt_init() {
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI,
        //.credentials.username = MQTT_USERNAME,
        //.credentials.authentication.password = MQTT_PASSWORD,
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
    return client;
}

// Função principal
void app_main() {
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init();
    i2c_master_init();
    esp_mqtt_client_handle_t mqtt_client = mqtt_init();

    // Esperar conexão Wi-Fi
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);

    // Loop principal
    while (true) {
        uint16_t light = bh1750_read_light();
        char message[50];
        sprintf(message, "Luminosidade: %u lx", light);
        ESP_LOGI(TAG_I2C, "%s", message);
        esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC, message, 0, 1, 0);
        vTaskDelay(pdMS_TO_TICKS(60000)); // 1 minuto
    }
}
