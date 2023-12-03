#include <Arduino.h>

#define Turbidy_sensor A0   
int TurbidySensorValue = 0;  
float Tension = 0.0;  
void setup() {
    Serial.begin(9600); // Velocidad de comunicación
    Serial.println("Prueba de lectura del sensor de turbidez");
    Serial.println("========================================");
    Serial.println(" ");
    Serial.println("Lectura analogica\tTension\tTurbidez (NTU)");
    Serial.println("-----------------\t-------\t--------------");
}
void loop() {
    TurbidySensorValue = analogRead(Turbidy_sensor);	  // Lectura del pin analógico 0
    Tension = TurbidySensorValue * (5.0 / 1024.0);		  // Mapeo de la lectura analógica
    float NTU = map(TurbidySensorValue, 0, 1023, 0, 100); // Mapeo de la lectura analógica a NTU
    // Envio de valores y textos al terminal serie
    Serial.print(TurbidySensorValue);
    Serial.print("\t\t\t");
    Serial.print(Tension);
    Serial.print(" V\t");
    Serial.print(NTU);
    Serial.println(" NTU");
    delay(3000);
}
