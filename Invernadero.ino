#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <DHT.h>

LiquidCrystal_I2C lcd1(0x27, 16, 2);
LiquidCrystal_I2C lcd2(0x26, 16, 2);

SoftwareSerial bluetooth(10, 11);

int mediumPin = 7;
int highPin = 8;
int relayPin1 = 9; // Relay para el nivel de agua
int relayPin2 = 12; // Relay para el comando Bluetooth
int relayPin3 = 13; // Relay para la bomba de agua
int ledPins[] = {2, 3, 4, 5};
int numLeds = 4;
int soilMoisturePin = A2; // Pin analógico para el sensor de humedad del suelo

#define DHTPIN 6
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  bluetooth.begin(9600);

  pinMode(mediumPin, INPUT);
  pinMode(highPin, INPUT);
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(relayPin3, OUTPUT);
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  lcd1.init();
  lcd1.backlight();
  lcd1.clear();
  lcd1.setCursor(0, 0);
  lcd1.print("Sistema Iniciado");

  lcd2.init();
  lcd2.backlight();
  lcd2.clear();
  lcd2.setCursor(0, 0);
  lcd2.print("Humedad Suelo: ");
  
  dht.begin();

  Serial.println("Sistema listo para recibir comandos.");
}

void loop() {
  if (Serial.available()) {
    char receivedChar = Serial.read();
    Serial.print("Comando recibido por Serial: ");
    Serial.println(receivedChar);
    controlRelays(receivedChar);
  }

  if (bluetooth.available()) {
    char receivedChar = bluetooth.read();
    Serial.print("Comando recibido por Bluetooth: ");
    Serial.println(receivedChar);
    controlRelays(receivedChar);
  }

  if (digitalRead(highPin) == HIGH) {
    Serial.println("Nivel de agua: Alto");
    bluetooth.println("Nivel de agua: Alto");
    lcd1.clear();
    lcd1.setCursor(0, 0);
    lcd1.print("Nivel de Agua:");
    lcd1.setCursor(0, 1);
    lcd1.print("Alto");
    digitalWrite(relayPin1, LOW);

  } else if (digitalRead(mediumPin) == HIGH) {
    Serial.println("Nivel de agua: Medio");
    bluetooth.println("Nivel de agua: Medio");
    lcd1.clear();
    lcd1.setCursor(0, 0);
    lcd1.print("Nivel de Agua:");
    lcd1.setCursor(0, 1);
    lcd1.print("Medio");
    digitalWrite(relayPin1, HIGH);

    digitalWrite(ledPins[numLeds - 1], HIGH); 
  } else {
    Serial.println("Nivel de agua: Bajo");
    bluetooth.println("Nivel de agua: Bajo");
    lcd1.clear();
    lcd1.setCursor(0, 0);
    lcd1.print("Nivel de Agua:");
    lcd1.setCursor(0, 1);
    lcd1.print("Bajo");
    digitalWrite(relayPin1, HIGH);

  }

  float temperatureC = dht.readTemperature();
  if (isnan(temperatureC)) {
    Serial.println("Error al leer la temperatura!");
  } else {
    Serial.print("Temperatura: ");
    Serial.print(temperatureC);
    Serial.println("°C");
    lcd2.setCursor(0, 1);
    lcd2.print("Temp: ");
    lcd2.print(temperatureC);
    lcd2.write(0xDF); // Símbolo de grados Celsius
    lcd2.print("C");
  }

  int soilMoistureValue = analogRead(soilMoisturePin);
  int soilMoisturePercentage = map(soilMoistureValue, 0, 1023, 0, 100);
  Serial.print("Humedad del suelo: ");
  Serial.print(soilMoisturePercentage);
  Serial.println("%");
  
  lcd2.setCursor(0, 0);
  lcd2.print("Suelo: ");
  lcd2.print(soilMoisturePercentage);
  lcd2.print("%");

  // Si la humedad del suelo es menor a un cierto umbral, activar la bomba de agua
  if (soilMoisturePercentage < 30) {
    digitalWrite(relayPin3, HIGH); // Activar la bomba de agua
  } else {
    digitalWrite(relayPin3, LOW); // Desactivar la bomba de agua
  }

  delay(2000); 
}

void controlRelays(char command) {
  digitalWrite(relayPin1, LOW);
  digitalWrite(relayPin2, LOW);


  switch (command) {
    case 'a':
      digitalWrite(relayPin1, HIGH);
      Serial.println("Relé activado por comando A");
      bluetooth.println("Relé activado por comando A");
      lcd1.clear();
      lcd1.setCursor(0, 0);
      lcd1.print("Relé A activado");
      break;
    case 'b':
      digitalWrite(relayPin1, LOW);
      Serial.println("Relé desactivado por comando B");
      bluetooth.println("Relé desactivado por comando B");
      lcd1.clear();
      lcd1.setCursor(0, 0);
      lcd1.print("Relé B desactivado");
      break;
    default:
      Serial.println("Comando no reconocido");
      bluetooth.println("Comando no reconocido");
      lcd1.clear();
      lcd1.setCursor(0, 0);
      lcd1.print("Comando no reconocido");
      break;
  }
}

void controlRelay2(char command) {
  digitalWrite(relayPin2, LOW);

  switch (command) {
    case 'c':
      digitalWrite(relayPin2, HIGH);
      Serial.println("Relé 2 activado por comando E");
      bluetooth.println("Relé 2 activado por comando E");
      lcd1.clear();
      lcd1.setCursor(0, 0);
      lcd1.print("Relé 2 activado");
      break;
    case 'd':
      digitalWrite(relayPin2, LOW);
      Serial.println("Relé 2 desactivado por comando F");
      bluetooth.println("Relé 2 desactivado por comando F");
      lcd1.clear();
      lcd1.setCursor(0, 0);
      lcd1.print("Relé 2 desactivado");
      break;
    default:
      Serial.println("Comando no reconocido para el Relé 2");
      bluetooth.println("Comando no reconocido para el Relé 2");
      lcd1.clear();
      lcd1.setCursor(0, 0);
      lcd1.print("Comando no reconocido para el Relé 2");
      break;
  }
}
