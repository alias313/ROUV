#include <Arduino.h>
#include "pins.hpp"
#include "debug.hpp"

#define VREF 5.0      // analog reference voltage(Volt) of the ADC
#define SCOUNT  30           // sum of sample point

unsigned int sampleRate = 20U;
unsigned int window = 25; // Number of samples taken

// MPX (Pressure meter)

float mpxVoltage = 0.0;

// TDS (TDS meter)
int analogBuffer[SCOUNT];    // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0, copyIndex = 0;
int tdsValue = 0;
float tdsAverageVoltage = 0, temperature = 25;

// TSW (Turbidity meter)
float tswVoltage = 0.0;
int ntuValue = 0;

int getMedianNum(int bArray[], int iFilterLen);

void setup()
{
    Serial.begin(9600);
    pinMode(MPXPin, INPUT);
    pinMode(TDSPin, INPUT);
    pinMode(TSWPin, INPUT);
    while (debugAvailable() <= 0 || debugRead() != 's') 
    {
        debugln("Send 's' to start: ");
        debugDelay(200); 
    }
    debugln("Lectura de: presion (manometro), turbidez (turbidimetro), tds (conductimetro) y velocidad");
    debugln("Conversion a medidas utiles: tension => profundidad, turbidez, total de solidos disueltos");
    debugln("=========================================================================================");
    debugln(" ");
    debugln("MPX: Tension\tTDS: Tension\t TDS \tTSW: Tension\t NTU ");
    debugln("------------\t------------\t-----\t------------\t-----");
}

void loop()
{
    static unsigned long analogSampleTimepoint = millis();
    if (millis() - analogSampleTimepoint > sampleRate) // every 20 milliseconds, read the analog value from the ADC
    {
        // TDS sensor
        analogSampleTimepoint = millis();
        analogBuffer[analogBufferIndex] = analogRead(TDSPin); // read the analog value and store into the buffer
        analogBufferIndex++;
        if (analogBufferIndex == SCOUNT)
            analogBufferIndex = 0;

        // Turbitity sensor
        tswVoltage += ((float)analogRead(TSWPin) / 1024) * 5; //mapping from 10-bit voltage reading to 0-5V range
    }
    static unsigned long printTimepoint = millis();
    if (millis() - printTimepoint > window*sampleRate)
    {
        // Apply median filtering algorithm to data read from TDSPin
        printTimepoint = millis();
        for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++)
            analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
        tdsAverageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 1024.0; 
        float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);                                                                                                               // temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
        float compensationVolatge = tdsAverageVoltage / compensationCoefficient;                                                                                                            // temperature compensation
        tdsValue = (133.42 * compensationVolatge * compensationVolatge * compensationVolatge - 255.86 * compensationVolatge * compensationVolatge + 857.39 * compensationVolatge) * 0.5; // convert voltage value to tds value

        // Simple averaging to data read from TSWPin
        tswVoltage /= window;
        if (tswVoltage < 2.5)
        {
            ntuValue = 3000;
        } else {
            ntuValue = -1120.4 * square(tswVoltage) + 5742.3 * tswVoltage - 4352.9;
        }
        tswVoltage = 0.0;

        // Serial debugging
        debug("MPX: ");
        debugDec(mpxVoltage, 2);
        debug("V\t");
        debug("TDS: ");
        debugDec(tdsAverageVoltage,2);
        debug("V\t");
        debug(tdsValue);
        debug("ppm\t");
        debug("TSW: ");
        debug(analogRead(TSWPin));
        debug("V\t");
        debugln(ntuValue);

        // Serial communication (frame sent to master)
        Serial.print(mpxVoltage);
        Serial.print(" ");
        Serial.print(tdsValue);
        Serial.print(" ");
        Serial.print(ntuValue);
        Serial.print(" ");
        Serial.print("+234rpm");
        Serial.print(" ");
        Serial.print("-1034rpm");
        Serial.print(" ");
        Serial.println("+0rpm");
    }
}

int getMedianNum(int bArray[], int iFilterLen) 
{
    int bTab[iFilterLen];
    for (byte i = 0; i < iFilterLen; i++)
        bTab[i] = bArray[i];
    int i, j, bTemp;
    for (j = 0; j < iFilterLen - 1; j++)
    {
        for (i = 0; i < iFilterLen - j - 1; i++)
        {
            if (bTab[i] > bTab[i + 1])
            {
                bTemp = bTab[i];
                bTab[i] = bTab[i + 1];
                bTab[i + 1] = bTemp;
            }
        }
    }
    if ((iFilterLen & 1) > 0)
        bTemp = bTab[(iFilterLen - 1) / 2];
    else
        bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
    return bTemp;
}
