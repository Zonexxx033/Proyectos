#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Reemplazar con los datos de tu red wifi
const char* ssid = "CLARO1_D1CC60";
const char* password = "06156hPEpo";

// Token de Telegram BOT se obtiene desde Botfather en Telegram
#define token_Bot "7419680945:AAHnOxQTtRNngnSdDbBTXJDH69uXUsmU8uY"
// El ID se obtiene de (IDBot) en Telegram, no olvidar hacer click en iniciar
#define ID_Chat "5460959088"

WiFiClientSecure client;
UniversalTelegramBot bot(token_Bot, client);

const int sensorPin = 24; // Pin donde está conectada la entrada del sensor
const int luz = 13; // Pin para luz de 110 o 220 V (con módulo relay)

int conteo = 0;
String mensaje = "";
String conteoString = "";

void setup() {
  Serial.begin(115200);
  pinMode(sensorPin, INPUT_PULLUP);
  pinMode(luz, OUTPUT);

  connectToWiFi();

  // Prueba de conexión con Telegram
  Serial.println("Intentando enviar mensaje de inicio a Telegram...");
  if (bot.sendMessage(ID_Chat, "Sistema preparado!!!", "")) {
    Serial.println("Mensaje de inicio enviado a Telegram.");
  } else {
    Serial.println("Error al enviar el mensaje de inicio a Telegram.");
  }

  // Prueba de conexión adicional
  Serial.println("Intentando enviar mensaje de prueba a Telegram...");
  if (bot.sendMessage(ID_Chat, "Prueba de conexión", "")) {
    Serial.println("Mensaje de prueba enviado a Telegram.");
  } else {
    Serial.println("Error al enviar el mensaje de prueba a Telegram.");
  }
}

void connectToWiFi() {
  Serial.print("Conectando a la red wifi... ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  unsigned long startAttemptTime = millis();

  // Espera a la conexión WiFi durante un máximo de 10 segundos
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("No se pudo conectar a la red WiFi.");
    while (true);
  }

  Serial.println("");
  Serial.println("Conectado a la red wifi!!!");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  client.setInsecure();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conexión WiFi perdida. Intentando reconectar...");
    connectToWiFi();
  }

  int sensorValue = digitalRead(sensorPin);

  if (sensorValue == HIGH) { // Se detecta positivo
    digitalWrite(luz, HIGH);
    Serial.println("Luz Encendida!");

    conteo++;
    conteoString = String(conteo);
    mensaje = "Estado positivo detectado: " + conteoString;

    // Enviar mensaje a Telegram
    Serial.println("Intentando enviar mensaje de detección a Telegram...");
    if (bot.sendMessage(ID_Chat, mensaje, "")) {
      Serial.println("Mensaje enviado a Telegram: " + mensaje);
    } else {
      Serial.println("Error al enviar el mensaje a Telegram.");
      // Intentar reenviar el mensaje
      delay(2000);
      if (bot.sendMessage(ID_Chat, mensaje, "")) {
        Serial.println("Mensaje reenviado a Telegram: " + mensaje);
      } else {
        Serial.println("Error al reenviar el mensaje a Telegram.");
      }
    }
  } else { // Se detecta negativo
    digitalWrite(luz, LOW);
    Serial.println("Luz apagada!");

    // Enviar mensaje a Telegram si se detecta estado negativo
    mensaje = "Estado negativo detectado: " + String(conteo);
    Serial.println("Intentando enviar mensaje de detección a Telegram...");
    if (bot.sendMessage(ID_Chat, mensaje, "")) {
      Serial.println("Mensaje enviado a Telegram: " + mensaje);
    } else {
      Serial.println("Error al enviar el mensaje a Telegram.");
      // Intentar reenviar el mensaje
      delay(2000);
      if (bot.sendMessage(ID_Chat, mensaje, "")) {
        Serial.println("Mensaje reenviado a Telegram: " + mensaje);
      } else {
        Serial.println("Error al reenviar el mensaje a Telegram.");
      }
    }
  }
  
  delay(1000); // Espera antes de la siguiente detección
}
