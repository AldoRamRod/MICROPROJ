#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "ThingSpeak.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Definición de los pines I2C para la Raspberry Pi Pico W
#define I2C_SDA_0 0
#define I2C_SCL_0 1
#define I2C_SDA_1 2
#define I2C_SCL_1 3

// Inicialización del sensor BME280
Adafruit_BME280 bme;

// Credenciales WiFi
const char* ssid = ".:PC Puma FESC C4:.";
const char* password = "";

// ThingSpeak
unsigned long myChannelNumber = 2776639;
const char * myWriteAPIKey = "LARGDQ4A7ZX0256G";
const char* server = "api.thingspeak.com";

WiFiClient  client;

void setup() {
  Serial.begin(115200);
  Serial.println(F("Iniciando..."));

  // Conectar a la red WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");

  ThingSpeak.begin(client);

  // Inicialización del I2C para el BME280
  Wire1.setSDA(I2C_SDA_1);
  Wire1.setSCL(I2C_SCL_1);
  Wire1.begin();

  // Inicialización del sensor BME280 con la dirección 0x76
  if (!bme.begin(0x76, &Wire1)) {
    Serial.println(F("No se encontró el sensor BME280"));
    while (1);
  }

  Serial.println(F("Sensor BME280 iniciado correctamente"));

  // Inicialización del I2C para la pantalla OLED
  Wire.setSDA(I2C_SDA_0);
  Wire.setSCL(I2C_SCL_0);
  Wire.begin();
  
  // Inicialización de la pantalla OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("No se encontró la pantalla OLED"));
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Inicializando..."));
  display.display();
  delay(2000);
}

void loop() {
  // Lectura de los valores del sensor
  float temperature = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0F; // Convertir de Pa a hPa
  float humidity = bme.readHumidity();

  // Mostrar los valores en el Monitor Serial
  Serial.print(F("Temperatura: "));
  Serial.print(temperature);
  Serial.println(F(" °C"));

  Serial.print(F("Presión: "));
  Serial.print(pressure);
  Serial.println(F(" hPa"));

  Serial.print(F("Humedad: "));
  Serial.print(humidity);
  Serial.println(F(" %"));

  Serial.println(F("-------------------"));

  // Mostrar los valores en la pantalla OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print(F("Temp: "));
  display.print(temperature);
  display.println(F(" *C"));
  
  display.print(F("Pres: "));
  display.print(pressure);
  display.println(F(" hPa"));
  
  display.print(F("Hum: "));
  display.print(humidity);
  display.println(F(" %"));
  
  display.display();
  
  // Enviar los datos a ThingSpeak
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, pressure);
  ThingSpeak.setField(3, humidity);
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if(x == 200){
    Serial.println("Actualización exitosa.");
  }
  else{
    Serial.println("Error en la actualización.");
  }

  // Espera de 15 segundos antes de la siguiente lectura (mínimo permitido por ThingSpeak)
  delay(15000);
}
