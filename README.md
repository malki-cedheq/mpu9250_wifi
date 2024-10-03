# Firmware MPU9250 + WiFi para ESP32-S3

> Autor: Malki-çedheq Benjamim <https://github.com/malki-cedheq>

## Descrição
Este projeto consiste em um firmware para o ESP32-S3 que testa o sensor MPU9250 e monitora a conexão WiFi. O sistema acende LEDs no ESP32-S3 para indicar o status de conexão à internet e atualização dos dados do MPU9250, além de exibir as informações do sensor em uma interface web simples acessível pelo IP local da placa.

- **LED no GPIO 1**: Indica se o ESP32-S3 está conectado à internet.
- **LED no GPIO 2**: Indica que há novos dados atualizados no sensor MPU9250.
- A interface web pode ser acessada via o **IP local** do ESP32 e exibe os dados de aceleração, giroscópio e magnetômetro.

## Funcionalidades
- **MPU9250**: O sensor MPU9250 coleta dados de aceleração, giroscópio e magnetômetro.
- **WiFi**: O ESP32-S3 se conecta à internet e exibe o IP público e local via uma página web.
- **Indicadores LED**:
  - **LED WiFi (GPIO 1)**: Acende quando o ESP32-S3 está conectado à internet.
  - **LED MPU9250 (GPIO 2)**: Acende quando o sensor MPU9250 atualiza os dados.

## Requisitos
- **Placa**: ESP32-S3
- **Sensor**: MPU9250
- **Bibliotecas**:
  - [MPU9250](https://github.com/hideakitai/MPU9250) (por Hideaki Tai)
  - WiFi
  - WebServer
  - HTTPClient

## Pinos
- **MPU9250**:
  - SDA: GPIO 4
  - SCL: GPIO 3
- **LEDs**:
  - WiFi: GPIO 1
  - MPU9250: GPIO 2

## Como usar

### 1. Configuração do WiFi
Altere as credenciais WiFi no código:
```cpp
const char* ssid = "SeuSSID";
const char* password = "SuaSenha";
```

### 2. Conectar o MPU9250
Conecte o sensor MPU9250 nos pinos SDA (GPIO 4) e SCL (GPIO 3) do ESP32-S3.

### 3. Carregar o firmware
Compile e carregue o firmware no ESP32-S3 usando a Arduino IDE ou outro ambiente de sua preferência.

### 4. Acessar a interface web
Após a conexão do ESP32-S3 à rede WiFi, o endereço IP local será exibido no monitor serial. Acesse esse IP no seu navegador para visualizar os dados do sensor.

### 5. Comportamento dos LEDs
O LED WiFi (GPIO 1) ficará aceso enquanto o ESP32-S3 estiver conectado à internet.
O LED MPU9250 (GPIO 2) acende brevemente enquanto o sensor coleta novos dados.

# Licença
Este projeto é de código aberto e segue a licença MIT.
