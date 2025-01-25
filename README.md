# ESP32 BH1750 MQTT Publisher

## 📝 Descrição do Projeto (Português)
Este projeto demonstra o uso do ESP32 para leitura de um sensor de luz BH1750 via interface I2C e publicação periódica dos dados em um broker MQTT. É ideal para aplicações IoT, permitindo monitorar níveis de luminosidade em tempo real.

### 🚀 Funcionalidades
- Leitura do sensor BH1750 a cada minuto.
- Publicação dos valores de luminosidade no tópico MQTT `vshaft/sensor/light`.
- Conexão Wi-Fi para envio dos dados ao broker MQTT.

### 📦 Requisitos de Hardware
- **ESP32** (Qualquer modelo compatível com o ESP-IDF).
- **Sensor de Luz BH1750** (Conexão I2C).
- Resistores de pull-up de 10kΩ para linhas SDA e SCL.
- Cabos para conexão (jumpers).

### 💻 Requisitos de Software
- **ESP-IDF** v5.4.0 ou superior configurado.
- Broker MQTT (ex.: test.mosquitto.org ou broker próprio).
- Cliente MQTT (opcional para testes, como `mosquitto`).

### 🛠️ Como Usar
1. **Clone o Repositório**
   ```bash
   git clone https://github.com/seu-usuario/esp32-bh1750-mqtt.git
   cd esp32-bh1750-mqtt
2. **Configure o Ambiente ESP-IDF Certifique-se de que o ESP-IDF está instalado e configurado corretamente:**
    ```bash
    idf.py --version
3. **Personalize as Configurações**

- Abra o arquivo main.c e edite:
    - Credenciais Wi-Fi:
    ```c
    #define WIFI_SSID "SUA REDE WIFI"
    #define WIFI_PASSWORD "SENHA WIFI"
    ```
- Broker MQTT (opcional):
    ```c
    #define MQTT_BROKER_URI "mqtt://test.mosquitto.org:1883"
    ```
- Compile e Faça Flash do Código

    ```bash
    idf.py build
    idf.py flash -p /dev/ttyUSB0 monitor
- Monitore o Broker MQTT Verifique as mensagens publicadas:
    ```bash
    mosquitto_sub -h test.mosquitto.org -t vshaft/sensor/light

📜 Funcionamento do Projeto
1. O ESP32 se conecta ao Wi-Fi configurado.
2. Realiza leituras do sensor BH1750 a cada minuto.
3. Publica as leituras no broker MQTT no formato Luminosidade: X lx.
4. Os logs podem ser monitorados no terminal via idf.py monitor.

## 📝 Project Description (English)
This project demonstrates using the ESP32 to read a BH1750 light sensor over the I2C interface and periodically publish the data to an MQTT broker. It’s perfect for IoT applications, enabling real-time light level monitoring.

## 🚀 Features
- BH1750 sensor readings every minute.
- Publishes luminosity values to the MQTT topic vshaft/sensor/light.
- Wi-Fi connection to send data to the MQTT broker.
## 📦 Hardware Requirements
- **ESP32** (Any model compatible with ESP-IDF).
- BH1750 **Light Sensor** (I2C connection).
- 10kΩ pull-up resistors for SDA and SCL lines.
- Connection cables (jumpers).
## 💻 Software Requirements
- **ESP-IDF** v5.4.0 or higher configured.
- MQTT broker (e.g., test.mosquitto.org or custom broker).
- MQTT client (optional for testing, such as mosquitto).
## 🛠️ How to Use
1. **Clone the Repository**

    ```bash
    git clone https://github.com/your-username/esp32-bh1750-mqtt.git
    cd esp32-bh1750-mqtt

2. **Configure ESP-IDF Environment Ensure ESP-IDF is installed and correctly configured:**

    ```bash
    idf.py --version

3. **Customize the Configurations**

- Open the main.c file and edit:
    - - Wi-Fi Credentials:
        ```c
        #define WIFI_SSID "YOUR WIFI NETWORK"
        #define WIFI_PASSWORD "YOUR WIFI PASSWORD"
-  MQTT Broker (optional):
    ```c
    #define MQTT_BROKER_URI "mqtt://test.mosquitto.org:1883"

- Build and Flash the Code

    ```bash
    idf.py build
    idf.py flash -p /dev/ttyUSB0 monitor

- Monitor the MQTT Broker Check the published messages:

    ```bash
    mosquitto_sub -h test.mosquitto.org -t vshaft/sensor/light

📜 Project Workflow
1. The ESP32 connects to the configured Wi-Fi network.
2. Reads the BH1750 sensor every minute.
3. Publishes readings to the MQTT broker in the format Luminosidade: X lx.
4.Logs can be monitored in the terminal via idf.py monitor.