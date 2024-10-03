//Arquivo: mpu9250_wifi.ino
//Versão: 0.2.0
//Descrição: 
//Firmware para teste o sensor MPU9250 e wifi do ESP32-S3
// Caso haja conexão com a internet permanece acesso o ledWifi
// Caso haja novos dados no MPU9250 permanece acesso o ledMPU
// As informações também podem ser conferidas numa interface WEB
// simples acessível através do IP LOCAL atribuido a esta ESP32
//Autor: Malki-çedheq  Benjamim <https://github.com/malki-cedheq>
//Criado em: 04/09/2024
//Atualizado em: 02/10/2024

#include <Wire.h>
#include <MPU9250.h> // library by hideakitai
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h> // Necessário para requisição HTTP

// Definição dos pinos dos LEDs
const int ledWifi = 1;  // LED no GPIO 1 (indica conexão à internet)
const int ledMPU = 2;   // LED no GPIO 2 (indica update do MPU9250)

MPU9250 mpu; // Instanciação do objeto do sensor MPU9250

// Configurações personalizadas do MPU9250
MPU9250Setting settings;

// Configuração de WiFi
const char* ssid = "SSID";       // SSID da rede WiFi
const char* password = "Senha";  // Senha da rede WiFi

WebServer server(80); // Instância do servidor web na porta 80
String publicIP; // Variável para armazenar o IP público

/**
 * @brief Função para inicializar o sensor MPU9250 e a conexão WiFi.
 */
void setup() {
  Serial.begin(115200);
  Wire.begin(4, 3); // Configura GPIO 4 como SDA e GPIO 3 como SCL para o MPU-9250

  // Inicialização dos LEDS
  setupLEDs();

  // Inicialização do MPU9250
  setupMPU9250();

  // Inicialização do WiFi
  setupWiFi();

  // Configurações do servidor web
  server.on("/", handleRoot);  // Rota principal que exibe dados do sensor
  server.begin();              // Inicia o servidor web
  Serial.println("Servidor web iniciado.");
}

/**
 * @brief Função para configurar os LEDs
 */
void setupLEDs(){
  // Inicialização dos pinos dos LEDs
  pinMode(ledWifi, OUTPUT);   // LED de WiFi
  pinMode(ledMPU, OUTPUT);    // LED de MPU9250
  digitalWrite(ledWifi, LOW); // Inicialmente o LED de WiFi está desligado
  digitalWrite(ledMPU, LOW);  // Inicialmente o LED de MPU9250 está desligado
}

/**
 * @brief Função para configurar o MPU-9250
 */
void setupMPU9250(){
  // Configurações do sensor
  settings.accel_fs_sel = ACCEL_FS_SEL::A2G;       // Acelerômetro configurado para ±2G
  settings.gyro_fs_sel = GYRO_FS_SEL::G250DPS;     // Giroscópio configurado para ±250°/s
  settings.mag_output_bits = MAG_OUTPUT_BITS::M16BITS; // Magnetômetro configurado para 16 bits
  settings.fifo_sample_rate = FIFO_SAMPLE_RATE::SMPL_200HZ; // Taxa de amostragem configurada para 200Hz
  settings.gyro_dlpf_cfg = GYRO_DLPF_CFG::DLPF_41HZ; // Filtro digital do giroscópio configurado para 41Hz
  settings.accel_dlpf_cfg = ACCEL_DLPF_CFG::DLPF_45HZ; // Filtro digital do acelerômetro configurado para 45Hz

  // Inicializa o sensor
  if (!mpu.setup(0x68, settings)) {
    Serial.println("Falha ao inicializar o MPU9250!");
    while (1);
  }

  // Calibração dos sensores
  mpu.calibrateAccelGyro(); // Calibra o acelerômetro e o giroscópio
  mpu.calibrateMag();       // Calibra o magnetômetro
}

/**
 * @brief Função para configurar o WiFi e conectar à rede.
 */
void setupWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Conectando-se a ");
  Serial.println(ssid);

  // Aguarda até a conexão ser estabelecida
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConexão estabelecida!");
  Serial.print("Endereço IP local: ");
  Serial.println(WiFi.localIP());

  // Obtém o IP público após conexão
  getPublicIP();
}

/**
 * @brief Função para obter o IP público do dispositivo usando um serviço web.
 */
void getPublicIP() {
  HTTPClient http;
  http.begin("http://api.ipify.org"); // Serviço simples para obter o IP público
  int httpCode = http.GET();

  if (httpCode > 0) { // Checa se a requisição foi bem-sucedida
    publicIP = http.getString(); // Armazena o IP público
    Serial.print("IP público: ");
    Serial.println(publicIP);
  } else {
    Serial.println("Falha ao obter o IP público.");
    publicIP = "Não disponível";
  }
  
  http.end();
}

/**
 * @brief Função que gera e exibe os dados do sensor MPU9250 via web.
 */
void handleRoot() {
  String html = "<html><body><h1>Dados do Sensor MPU9250</h1>";
  html += "<p>ax: " + String(mpu.getAccX(), 2) + " | ay: " + String(mpu.getAccY(), 2) + " | az: " + String(mpu.getAccZ(), 2) + "</p>";
  html += "<p>gx: " + String(mpu.getGyroX(), 2) + " | gy: " + String(mpu.getGyroY(), 2) + " | gz: " + String(mpu.getGyroZ(), 2) + "</p>";
  html += "<p>mx: " + String(mpu.getMagX(), 2) + " | my: " + String(mpu.getMagY(), 2) + " | mz: " + String(mpu.getMagZ(), 2) + "</p>";

  // Informações de conexão
  if (WiFi.status() == WL_CONNECTED) {
    // Acende o LED de WiFi quando conectado
    digitalWrite(ledWifi, HIGH);
    html += "<p><strong>Conectado a internet</strong></p>";
    html += "<p>IP publico: " + publicIP + "</p>";
  } else {
    // Acende o LED de WiFi quando conectado
    digitalWrite(ledWifi, LOW);    
    html += "<p><strong>Nao conectado a internet</strong></p>";
    html += "<p>IP publico: Nao disponivel</p>";
  }

  html += "</body></html>";
  server.send(200, "text/html", html);
}

/**
 * @brief Função que coleta e exibe dados do sensor no monitor serial.
 */
void plot(){
  if (mpu.update()) {

    // Acende o LED de MPU9250 enquanto os dados são atualizados
    digitalWrite(ledMPU, HIGH);
   
    // Coleta os dados de aceleração, giroscópio e magnetômetro
    float data[9] = {
      mpu.getAccX(), mpu.getAccY(), mpu.getAccZ(),
      mpu.getGyroX(), mpu.getGyroY(), mpu.getGyroZ(),
      mpu.getMagX(), mpu.getMagY(), mpu.getMagZ()
    };

    // Verifica se todos os dados são válidos antes de enviá-los para o monitor serial
    bool dados_validos = true;
    for (int i = 0; i < 9; i++) {
      if (isnan(data[i]) || isinf(data[i])) {
        dados_validos = false;
        break;
      }
    }

    // Se os dados forem válidos, exibe-os no monitor serial
    if (dados_validos) {
      Serial.print("ax:"); Serial.print(data[0], 2);
      Serial.print(", ay:"); Serial.print(data[1], 2);
      Serial.print(", az:"); Serial.print(data[2], 2);
      Serial.print(", gx:"); Serial.print(data[3], 2);
      Serial.print(", gy:"); Serial.print(data[4], 2);
      Serial.print(", gz:"); Serial.print(data[5], 2);
      Serial.print(", mx:"); Serial.print(data[6], 2);
      Serial.print(", my:"); Serial.print(data[7], 2);
      Serial.print(", mz:"); Serial.println(data[8], 2);
    }

    // Apaga o LED de MPU9250 após a leitura
    digitalWrite(ledMPU, LOW);
  }
}

/**
 * @brief Função principal que atualiza o sensor e serve a página web.
 */
void loop() {
  plot();           // Atualiza os dados do sensor
  server.handleClient();  // Lida com requisições HTTP
  delay(20);        // Ajuste a taxa de aquisição conforme necessário
}
