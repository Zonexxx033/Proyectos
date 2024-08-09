#include <ESP8266WiFi.h>

void setup() {
  Serial.begin(115200);
  delay(1000);  // Espera para asegurarse de que la conexión serie esté lista
  WiFi.mode(WIFI_STA);
  Serial.println("Iniciando...");
  delay(1000);
  Serial.print("La dirección MAC del ESP8266 es: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
  // No es necesario poner nada en el loop
}
