#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define pinData 2  //GP2 ESP-01

//WiFi
const char* SSID = "Lucas Correa";              // SSID  nome da rede WiFi que deseja se conectar
const char* PASSWORD = "Lucas_44191913";        // Senha da rede WiFi que deseja se conectar
WiFiClient wifiClient;   

//MQTT Server
const char* BROKER_MQTT = "test.mosquitto.org"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883;                         // Porta do Broker MQTT

#define ID_MQTT "POT02"                        //Informe um ID unico e seu. Caso sejam usados IDs repetidos a ultima conexão irá sobrepor a anterior. 
#define TOPIC_SUBSCRIBE "potenciometro"        //Informe um Tópico único. Caso sejam usados tópicos em duplicidade, o último irá eliminar o anterior.
PubSubClient MQTT(wifiClient);                 // Instancia o Cliente MQTT passando o objeto espClient

//Declaração das Funções
void mantemConexoes();                        //Garante que as conexoes com WiFi e MQTT Broker se mantenham ativas
void conectaWiFi();                           //Faz conexão com WiFi
void conectaMQTT();                           //Faz conexão com Broker MQTT
void recebePacote(char* topic, byte* payload, unsigned int length);

void setup() {
  pinMode(pinData, OUTPUT);         
  Serial.begin(115200);
  conectaWiFi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);  
  MQTT.setCallback(recebePacote); 
}

void loop() {
  mantemConexoes();
  MQTT.loop();
}

void mantemConexoes() {
    if (!MQTT.connected()) {
       conectaMQTT(); 
    }
    
    conectaWiFi();                          //se não há conexão com o WiFI, a conexão é refeita
}

void conectaWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
     return;
  }
        
  Serial.print("Conectando-se na rede: ");
  Serial.print(SSID);
  Serial.println("  Aguarde!");

  WiFi.begin(SSID, PASSWORD);             // Conecta na rede WI-FI  
  while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Conectado com sucesso, na rede: ");
  Serial.print(SSID);  
  Serial.print("  IP obtido: ");
  Serial.println(WiFi.localIP()); 
}

void conectaMQTT() { 
    while (!MQTT.connected()) {
        Serial.print("Conectando ao MQTT Broker: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conectado ao MQTT Broker com sucesso!");
            MQTT.subscribe(TOPIC_SUBSCRIBE);
        } 
        else {
            Serial.println("Nao foi possivel conectar ao MQTT Broker");
            Serial.println("Nova tentatica de conexao em 10 seg.");
            delay(10000);
        }
    }
}

void recebePacote(char* topic, byte* payload, unsigned int length) 
{
    Serial.print("Mensagem recebida [");
    Serial.print(TOPIC_SUBSCRIBE);
    Serial.print("] velocidade:");
    String data;
    String msg;

    //obtem a string do payload recebido
    for (int i = 0; i < length; i++) {
    data += (char)payload[i];
    char c = (char)payload[i];
    msg += c;
    }

    int velocidade = (data.toInt());
    constrain(velocidade, 0, 100);
    Serial.println((int) velocidade);  
    analogWrite(pinData, velocidade);  
}
