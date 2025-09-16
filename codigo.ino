/*
  Projeto IoT – Monitoramento de Temperatura e Umidade com ESP32 + DHT22 + MQTT + FIWARE
  Participantes:
  - Rafael Augusto Carmona RM: 563758
  - Eduardo Tolentino RM: 562169
  - Enzo Hort Ramos RM:561872
*/

#include <WiFi.h>           // Biblioteca para conexão Wi-Fi
#include <PubSubClient.h>   // Biblioteca para comunicação MQTT
#include "DHT.h"            // Biblioteca para o sensor DHT22

// Definições do sensor DHT
#define DHTPIN 15       // Pino conectado ao DHT22 no Wokwi (GPIO15)
#define DHTTYPE DHT22   // Definindo o modelo do sensor

// Inicializa o objeto do sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// Credenciais de rede Wi-Fi (padrão do Wokwi)
const char* ssid = "Wokwi-GUEST";   // Nome da rede Wi-Fi
const char* password = "";          // Sem senha

// Endereço do broker MQTT (IP da VM rodando o IoT Agent)
const char* mqtt_server = "20.116.216.196";

// Criação dos clientes Wi-Fi e MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Função para conectar ao Wi-Fi
void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);  // Inicia a conexão Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {  // Aguarda até conectar
    delay(500);
  }
}

// Função para reconectar ao broker MQTT caso a conexão caia
void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Client")) {   // Nome do cliente no broker
      // Conectado ao broker MQTT
    } else {
      delay(5000);  // Aguarda antes de tentar novamente
    }
  }
}

// Função de inicialização
void setup() {
  Serial.begin(115200);   // Inicia comunicação serial
  dht.begin();            // Inicializa o sensor DHT22
  setup_wifi();           // Conecta ao Wi-Fi
  client.setServer(mqtt_server, 1883); // Configura o servidor MQTT
}

// Loop principal
void loop() {
  if (!client.connected()) {  // Se desconectar do broker, tenta reconectar
    reconnect();
  }
  client.loop();  // Mantém a conexão MQTT ativa

  // Leitura dos valores do sensor
  float h = dht.readHumidity();     // Umidade
  float t = dht.readTemperature();  // Temperatura

  // Verifica se houve erro na leitura
  if (isnan(h) || isnan(t)) {
    Serial.println("Falha ao ler o DHT22!");
    return;
  }

  // Monta a mensagem no formato Ultralight 2.0
  // Exemplo: "t|25.3|h|60.2"
  String payload = "t|" + String(t, 1) + "|h|" + String(h, 1);

  // Publica os dados no tópico MQTT específico do IoT Agent
  client.publish("/TEF/weather001/attrs", payload.c_str());

  // Mostra no monitor serial o que foi publicado
  Serial.print("Publicado: ");
  Serial.println(payload);

  delay(5000); // Aguarda 5 segundos antes de publicar novamente
}
