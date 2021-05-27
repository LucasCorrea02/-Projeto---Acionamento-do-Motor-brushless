#include <ESP8266WiFi.h> 
#include <PubSubClient.h>

int pot;
int percent;
String payload;

//WiFi
const char* SSID = "Lucas Correa";                // SSID / nome da rede WiFi que deseja se conectar
const char* PASSWORD = "Lucas_44191913";          // Senha da rede WiFi que deseja se conectar
WiFiClient wifiClient;                        
 
//MQTT Server
const char* BROKER_MQTT = "test.mosquitto.org";   //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883;                           // Porta do Broker MQTT

#define ID_MQTT  "POT01"                         //Informe um ID unico e seu. Caso sejam usados IDs repetidos a ultima conexão irá sobrepor a anterior. 
#define TOPIC_PUBLISH "potenciometro"            //Informe um Tópico único. Caso sejam usados tópicos em duplicidade, o último irá eliminar o anterior.
PubSubClient MQTT(wifiClient);                   // Instancia o Cliente MQTT passando o objeto espClient

void wifiConnect() {      
  Serial.print("Conectando-se na rede: ");
  Serial.print(SSID);
  Serial.println("  Aguarde!");

  WiFi.begin(SSID, PASSWORD);                   // Conecta na rede WI-FI
 
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Conectado com sucesso, na rede: ");
  Serial.print(SSID);  
  Serial.print("  IP obtido: ");
  Serial.println(WiFi.localIP()); 

  if (MQTT.connect(ID_MQTT)) {
    Serial.print("Conectado ao MQTT Broker:");
    Serial.print(BROKER_MQTT);
    Serial.println(ID_MQTT);
    Serial.println(TOPIC_PUBLISH);
    }
    else {
    Serial.println("Conexao c/ MQTT Broker falhou");
    Serial.println("Reset e Tente novamente");
    abort();
    } 
}

void mqttReConnect() {
  while (!MQTT.connected()) {
    Serial.print("Tentando Conexao c/ MQTT Broker...");
    if (MQTT.connect(ID_MQTT)) { 
      Serial.println("MQTT Broker Conectado");
      MQTT.subscribe(TOPIC_PUBLISH);
    } else {
      Serial.print("Falha na Conexao c/ MQTT Broker");
      Serial.print(MQTT.state());
      Serial.println("Tente novamente em 5 seg.");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  wifiConnect();
  delay(10);
}

void loop() {
  pot = analogRead(A0);
  //percent = (int) ((pot * 100) / 1010);
  percent = (int) (pot);
  payload = (String) percent;
  if (MQTT.connected()) {
    if (MQTT.publish(TOPIC_PUBLISH, (char*) payload.c_str())) {
      Serial.print("Publisher OK (");
      Serial.print(payload);
      Serial.println(")");
    } else {
      Serial.println("Publisher falhou");
    }
  } else {
    mqttReConnect();
  }
 
  delay(200);
}
