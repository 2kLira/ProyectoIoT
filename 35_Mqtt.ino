#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"
#include "MQTT.hpp"
#include "ESP8266_Utils.hpp"
#include "ESP8266_Utils_MQTT.hpp"

// Si ya tienes la declaración de espClient en MQTT.hpp, elimínala de aquí.
// WiFiClient espClient;  // Eliminar esta línea si ya está en MQTT.hpp

const int trigPin = D5;      // Pin de Trigger
const int echoPin = D6;      // Pin de Echo
const int ledRojo = D2;      // Pin del LED rojo
const int ledVerde = D3;     // Pin del LED verde

// Declaración del cliente MQTT, asumiendo que espClient ya está definido en MQTT.hpp
extern WiFiClient espClient; // Utiliza extern para hacer referencia al cliente definido en MQTT.hpp
PubSubClient client(espClient); // Inicializa el cliente MQTT

// Asegúrate de definir estas variables en config.h o aquí mismo
const char* mqttServer = "tu.servidor.mqtt"; // Reemplaza con tu servidor MQTT
const int mqttPort = 1883; // Reemplaza con el puerto de tu servidor MQTT

void setup() {
  Serial.begin(115200);
  SPIFFS.begin();
  
  // Configuración de pines
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledRojo, OUTPUT);
  pinMode(ledVerde, OUTPUT);

  ConnectWiFi_STA(false);
  client.setServer(mqttServer, mqttPort); // Configura el servidor y el puerto del MQTT
  InitMqtt();
}

void loop() {
  // MQTT loop handler
  HandleMqtt();

  // Limpia el pin de Trigger
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Genera un pulso de 10 microsegundos en el pin de Trigger
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Lee el tiempo del pulso en el pin Echo
  long duration = pulseIn(echoPin, HIGH);

  // Calcula la distancia en cm
  int distance = duration * 0.034 / 2;

  // Imprime la distancia en el Monitor Serial
  Serial.print("Distancia medida: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Lógica para encender y apagar LEDs según la distancia
  if (distance <= 3) {
    digitalWrite(ledRojo, HIGH);   // Enciende el LED rojo
    digitalWrite(ledVerde, LOW);   // Apaga el LED verde
  } else {
    digitalWrite(ledRojo, LOW);    // Apaga el LED rojo
    digitalWrite(ledVerde, HIGH);  // Enciende el LED verde
  }

  // Publica la distancia en el tópico MQTT `hello/world`
  char msg[50];
  snprintf(msg, 50, "Distancia medida: %d cm", distance);
  client.publish("hello/world", msg);

  delay(500);  // Espera medio segundo antes de la próxima lectura
}


