// -------------------------------------------
// - CODIGO FUNCIONAL CONEXION ETHERNET O WIFI 
// - CON MQTT
// -------------------------------------------

// Topico para transmitir: Ethernet1/prueba
// Topico para transmitir: Ethernet1/actuadores

#include <SPI.h>
#include <EthernetENC.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <Wire.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Dirección MAC única para el módulo Ethernet
IPAddress ip(192, 168, 1, 240);     // Dirección IP asignada al dispositivo por ETHERNET
IPAddress myDns(192, 168, 1, 1);   

IPAddress wifiIP(192, 168, 1, 20); // Dirección IP asignada al dispositivo por WIFI
IPAddress wifiDNS(192, 168, 1, 1);
WiFiClient wifiClient;
EthernetClient ethClient;
PubSubClient mqttClient(ethClient);

const char* ssid = "ZTE-d007a4";                                                                    //  "ZTE-d007a4"
const char* password = "ec8a4cd0";
const char* mqttServer = "192.168.1.45";   // Dirección IP o nombre de dominio del servidor MQTT ETHERNET
//const char* mqttServerW = "192.168.1.5";      // IP WI-FI 
const int mqttPort = 9002; // Puerto del servidor MQTT


const char* mqttTopic = "Ethernet1/prueba"; // Tópico al que se enviarán los mensajes
const unsigned long interval = 5000; // Intervalo de tiempo entre cada mensaje (en milisegundos)

unsigned long previousMillis = 0;

void setup() {
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  Serial.begin(9600);
  while (!Serial) {
    ; // Espera a que la conexión serial se establezca
  }
  
  Ethernet.init(5); // Inicializa el módulo Ethernet con el pin CS (Chip Select) 10
  Ethernet.begin(mac, ip, myDns); // Configura la conexión Ethernet con la dirección MAC y la dirección IP
  delay(1000);
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Error al configurar Ethernet. Sin hardware disponible.");
    connectToWiFi();
  }
  else {
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet desconectado.");
      connectToWiFi();
    }
    else {
      Serial.println("Conexión Ethernet establecida.");
      Serial.print("Dirección IP Ethernet: ");
      Serial.println(Ethernet.localIP());
      connectToMQTT();
    }
  }

     if (mqttClient.connect("arduino-client")) {
       Serial.println("Conectado al servidor MQTT");
     }
     if (mqttClient.connect("arduino-client")) {
          Serial.println("Conectado al servidor MQTT");
        }
}

void connectToWiFi() {
//  WiFi.config(wifiIP, wifiDNS);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  Serial.println("Conexión WiFi establecida.");
  Serial.print("Dirección IP WiFi: ");
  Serial.println(WiFi.localIP());
  
  // Configurar cliente MQTT con WiFiClient
  mqttClient.setClient(wifiClient);
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(callback);
    while (!mqttClient.connected()) {
    Serial.println("Conectando al servidor MQTT...");

    if (mqttClient.connect("ESP32-Client")) {
      Serial.println("Conexión al servidor MQTT establecida");

      // Suscribirse a un tópico si es necesario
      mqttClient.subscribe("Ethernet1/actuadores");
    } else {
      Serial.print("Error al conectar al servidor MQTT. Estado: ");
      Serial.print(mqttClient.state());
      Serial.println(" Reintentando en 5 segundos...");
      delay(5000);
    }
  }
}

void connectToMQTT() {
  mqttClient.setClient(ethClient);
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(callback);
  while (!mqttClient.connected()) {
    Serial.println("Conectando al servidor MQTT...");

    if (mqttClient.connect("ESP32-Client")) {
      Serial.println("Conexión al servidor MQTT establecida");

      // Suscribirse a un tópico si es necesario
      mqttClient.subscribe("Ethernet1/actuadores");
    } else {
      Serial.print("Error al conectar al servidor MQTT. Estado: ");
      Serial.print(mqttClient.state());
      Serial.println(" Reintentando en 5 segundos...");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (String(topic) == "Ethernet1/actuadores") {
   // last_Message = "";
   // last_Message = messageTemp;
    if(messageTemp == "ON1"){
      Serial.println("on");
      digitalWrite(2, HIGH);
    }
    if(messageTemp == "OFF1"){
        Serial.println("off");
        digitalWrite(2, LOW);
    }

  }
}

void loop() {
  mqttClient.loop();                               //Habilita las funciones de MQTT

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (mqttClient.connected()) {
      String message = "Hola desde Arduino";
      mqttClient.publish(mqttTopic, message.c_str());
      Serial.println("Mensaje enviado al tópico");
    }
  }
}
