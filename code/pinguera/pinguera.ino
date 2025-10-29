#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "notifications.h"

/*--- config de wifi ---*/
const char* ssid = "xxxx";
const char* password = "xxxx";

/* --- config notificaciones de callmebot ---*/
String phoneNumber = "xxxx";
String apiKey = "xxxx";

/* --- config de pines --- */
const int deviceLedPins[6] = {D4, D0, D3, D5, D6, D7}; 
const int failureLedPin = D8;
int fail_count = 0;

// --- config del LCD ---
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// --- config de los dispositivos ---
const char* hosts[6] = {
  "google.com",   //proveedor
  "xxxx",
  "xxxx",
  "xxxx",
  "xxxx",
  "xxxx"  
};
const int NUM_DEVICES = 6;
bool alertActive = false;

void setup() {
  Serial.begin(9600);
  Serial.println("\n--- Sistema de Monitoreo de Red ---");

  Wire.begin(D2, D1); // D2 (SDA), D1 (SCL)
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Iniciando...");

  for (int i = 0; i < NUM_DEVICES; i++) {
    pinMode(deviceLedPins[i], OUTPUT);
    if (deviceLedPins[i] == D4 || deviceLedPins[i] == D3) {
      digitalWrite(deviceLedPins[i], HIGH);
    } else {
      digitalWrite(deviceLedPins[i], LOW);
    }
  }
  pinMode(failureLedPin, OUTPUT);
  digitalWrite(failureLedPin, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Conectando WiFi...");
  lcd.setCursor(0, 1);
  lcd.print("Conectando WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n wifi conectado ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Conectado!");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());

  send_wsp("La conexión fue exitosa",phoneNumber,apiKey);
  delay(2000); 
}

void loop() {
  bool anyDeviceFailed = false;
  String firstFailedHost = "";

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pingueando:");

  for (int i = 0; i < NUM_DEVICES; i++) {
    // --- limpiar y mostrar host ---
    lcd.setCursor(0, 1);
    lcd.print("                "); 
    lcd.setCursor(0, 1);
    lcd.print(hosts[i]);

    // espera para que se vea el host
    delay(100); 

    bool pingSuccess = Ping.ping(hosts[i]);

    if (pingSuccess) {
      float rtt = Ping.averageTime(); // rtt del host actual

      // --- mostrar RTT en el LCD ---
      lcd.setCursor(11, 0); 
      lcd.print(String((int)rtt)+ "mS  ");
      
      if (deviceLedPins[i] == D4 || deviceLedPins[i] == D3) {
        digitalWrite(deviceLedPins[i], LOW); // OK = activo bajo
      } else {
        digitalWrite(deviceLedPins[i], HIGH); // OK = activo alto
      }
    } else {
      // --- mostrar FALLO en el LCD ---
      lcd.setCursor(11, 1); 
      lcd.print("FALLO");
      
      if (deviceLedPins[i] == D4 || deviceLedPins[i] == D3) {
        digitalWrite(deviceLedPins[i], HIGH); // fallo = apagado (alto)
      } else {
        digitalWrite(deviceLedPins[i], LOW);  // fallo = apagado (bajo)
      }
      
      anyDeviceFailed = true;
      if (firstFailedHost == "") {
        firstFailedHost = hosts[i];
      }
    }
    delay(500); // pausa para leer resultado de cada host
  }

  // --- logica de alertas y LCD  ---
  if (anyDeviceFailed) {
    digitalWrite(failureLedPin, HIGH);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("FALLO DETECTADO:");
    lcd.setCursor(0, 1);
    lcd.print(firstFailedHost);

    if (!alertActive && fail_count >= 3) {
      String alertMessage = "🚨 Arriba!!! El equipo " + firstFailedHost + " no responde.";
      send_wsp(alertMessage,phoneNumber,apiKey);
      alertActive = true; 
    } else if (!alertActive) {
      fail_count++;
    }
    
  } else {
    if (alertActive) {
      send_wsp("👍 It's alive!!!! Todos los equipos responden.",phoneNumber, apiKey);
    }
    // --- pantalla final ---
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SISTEMA OK");
    lcd.setCursor(0, 1);
    lcd.print("Don't worry...");

    digitalWrite(failureLedPin, LOW);
    alertActive = false;
    fail_count = 0; 
  }

  // Espera 2 seg antes del nuevo ciclo de pingueo
  delay(2000);
}