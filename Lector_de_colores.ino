#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Replace 0x27 with your LCD address

#define outPin 8
#define s0 9
#define s1 10
#define s2 11
#define s3 12

boolean DEBUG = true;

// Variables
int red, grn, blu;
String color = "";
long startTiming = 0;
long elapsedTime = 0;

void setup() {
  Serial.begin(9600);

  Wire.begin();  // Initialize the I2C communication
  lcd.init();    // Initialize the LCD
  lcd.backlight(); // Turn on the backlight

  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(outPin, INPUT); //out from sensor becomes input to Arduino

  // Setting frequency scaling to 100%
  digitalWrite(s0, HIGH);
  digitalWrite(s1, HIGH);

  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("DETECTOR DE ");
  lcd.setCursor(1, 1);
  lcd.print("COLORES");
  delay(2000);
  lcd.clear();
  startTiming = millis();
}

void loop() {
  getColor();

  if (DEBUG) printData();
  elapsedTime = millis() - startTiming;
  if (elapsedTime > 1000) {
    showDataLCD();
    startTiming = millis();
  }
}

/* read RGB components */
void readRGB() {
  red = 0, grn = 0, blu = 0;

  int n = 10;
  for (int i = 0; i < n; ++i) {
    // read red component
    digitalWrite(s2, LOW);
    digitalWrite(s3, LOW);
    red = red + pulseIn(outPin, LOW);

    // read green component
    digitalWrite(s2, HIGH);
    digitalWrite(s3, HIGH);
    grn = grn + pulseIn(outPin, LOW);

    // let's read blue component
    digitalWrite(s2, LOW);
    digitalWrite(s3, HIGH);
    blu = blu + pulseIn(outPin, LOW);
  }
  red = red / n;
  grn = grn / n;
  blu = blu / n;
}

/***************************************************
 * Showing captured data at Serial Monitor
 ****************************************************/
void printData(void) {
  Serial.print("red= ");
  Serial.print(red);
  Serial.print("   green= ");
  Serial.print(grn);
  Serial.print("   blue= ");
  Serial.print(blu);
  Serial.print(" - ");
  Serial.print(color);
  Serial.println(" detected!");
}

/***************************************************
 * Showing captured data on LCD
 ****************************************************/
void showDataLCD(void) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("R");
  lcd.print(red);
  lcd.setCursor(6, 0);
  lcd.print("G");
  lcd.print(grn);
  lcd.setCursor(12, 0);
  lcd.print("B");
  lcd.print(blu);

  lcd.setCursor(0, 1);
  lcd.print("Color: ");
  lcd.print(color);
}

void getColor() {
  readRGB();

  if (red > 15 && red < 30 && grn > 20 && grn < 30 && blu > 20 && blu < 30 )color = "BLANCO";  // Ajusta los valores límite del color negro
  else if (red > 19 && red < 40 && grn > 65 && grn < 90 && blu > 45 && blu < 75) color = "ROJO";  // Ajusta los valores límite del color rojo  // Ajusta los valores límite del color azul
  else if (red > 25 && red < 85 && grn > 25 && grn < 60 && blu  > 30 && blu < 70) color = "VERDE";
  else if (red > 15 && red <  25 && grn > 25 && grn < 39 && blu > 30 && blu < 65) color = "AMARILLO";
  else if (red > 35 && red < 50 && grn > 55 && grn < 75 && blu > 35 && blu < 50) color = "MORADO";
  else if (red > 70 && red <95 && grn > 45 && grn < 90 && blu >20 && blu < 60) color = "AZUL";  // Ajusta los valores límite del color azul
  else color = "NO_COLOR";
}
