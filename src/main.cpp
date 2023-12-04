#include <Arduino.h>
#include "pins.hpp"
#include "main.hpp"
#include "debug.hpp"

void turningDirection(char inputLetter, char motor);
int getMedianNum(int bArray[], int iFilterLen);
void verboseMessages();

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
    debugln("MPX: Tension\tTDS: Tension / TDS   \tTSW: Tension / NTU   ");
    debugln("------------\t---------------------\t---------------------");
}

void loop()
{   
    if (Serial.read() == 'c') // c for captain mode
    {
        switch (Serial.read())
        {
        case 'q':
            turn = Serial.read();
            turningDirection(turn, 'l');
            analogWrite(PWML, 25);
            break;
        case 'w':
            turn = Serial.read();
            turningDirection(turn, 'l');
            analogWrite(PWML, 50);
            break;
        case 'e':
            turn = Serial.read();
            turningDirection(turn, 'l');
            analogWrite(PWML, 75);
            break;
        case 'r':
            turn = Serial.read();
            turningDirection(turn, 'l');
            analogWrite(PWML, 100);
            break;

        case 'f':
            turn = Serial.read();
            turningDirection(turn, 'c');
            analogWrite(PWMC, 25);
            break;
        case 't':
            turn = Serial.read();
            turningDirection(turn, 'c');
            analogWrite(PWMC, 50);
            break;
        case 'y':
            turn = Serial.read();
            turningDirection(turn, 'c');
            analogWrite(PWMC, 75);
            break;
        case 'h':
            turn = Serial.read();
            turningDirection(turn, 'c');
            analogWrite(PWMC, 100);
            break;


        case 'u':
            turn = Serial.read();
            turningDirection(turn, 'r');
            analogWrite(PWMR, 25);
            break;
        case 'i':
            turn = Serial.read();
            turningDirection(turn, 'r');
            analogWrite(PWMR, 50);
            break;
        case 'o':
            turn = Serial.read();
            turningDirection(turn, 'r');
            analogWrite(PWMR, 75);
            break;
        case 'p':
            turn = Serial.read();
            turningDirection(turn, 'r');
            analogWrite(PWMR, 100);
            break;

        default:
            break;
        }
    }
    static unsigned long analogSampleTimepoint = millis();
    if (millis() - analogSampleTimepoint > sampleRate) // every 20 milliseconds, read the analog value from the ADC
    {   
        // TDS sensor
        analogSampleTimepoint = millis();
        analogBuffer[analogBufferIndex] = analogRead(TDSPin); // read the analog value and store into the buffer
        analogBufferIndex++;
        if (analogBufferIndex == SCOUNT)
            analogBufferIndex = 0;

        // LM35 temperature sensor
        lmVoltage += ((float)analogRead(LMPin) / 1024) * 5000; // mV because every 10 mV is 1 degree Celsius

        // MPX pressure ==> depth sensor
        mpxVoltage += ((float)analogRead(MPXPin) / 1024) * 5;       

        // Turbitity sensor
        tswVoltage += ((float)analogRead(TSWPin) / 1024) * 5; //mapping from 10-bit voltage reading to 0-5V range
    }
    static unsigned long printTimepoint = millis();
    if (millis() - printTimepoint > window*sampleRate)
    {
        // Simple averaging for depth and temperature values
        mpxVoltage /= window;
        lmVoltage /= window;

        depth = mpxVoltage * depthCoefficient - depthOffset;
        tempValue = lmVoltage / 10;

        mpxVoltage = 0.0;
        lmVoltage = 0.0;

        // Apply median filtering algorithm to data read from TDSPin
        printTimepoint = millis();
        for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++)
            analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
        tdsAverageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 1024.0; 
        float compensationCoefficient = 1.0 + 0.02 * (tempValue - 25.0);                                                                                                               // temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
        float compensationVoltage = tdsAverageVoltage / compensationCoefficient;                                                                                                            // temperature compensation
        tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5; // convert voltage value to tds value

        // https://fgcoca.github.io/Sensores-actuadores-y-shield-tipo-Arduino/turbidez/
        tswVoltage /= window;
        if (tswVoltage < 2.5)
        {
            ntuValue = 3000;
        } else {
            ntuValue = -1120.4 * square(tswVoltage) + 5742.3 * tswVoltage - 4352.9;
        }
        tswVoltage = 0.0;

        // Serial debugging
        debugMessages();

        // Serial communication (frame sent to master)
        debug("\tData in array: ");
        Serial.print(tempValue);    // 4 bytes
        Serial.print(" ");          // 1 byte
        Serial.print(depth);        // 4 bytes
        Serial.print(" ");          // 1 byte
        Serial.print(tdsValue);     // 2 bytes
        Serial.print(" ");          // 1 byte
        Serial.print(ntuValue);     // 2 bytes
        Serial.print(" ");          // 1 byte
        Serial.print("+234rpm");    // 2 bytes
        Serial.print(" ");          // 1 byte
        Serial.print("-1034rpm");   // 2 bytes
        Serial.print(" ");          // 1 byte
        Serial.println("+0rpm");    // 2 bytes
    }
}

void turningDirection(char inputLetter, char motor)
{
    switch (motor)
    {
    case 'l':
        if (inputLetter == CLOCKWISE_LETTER) digitalWrite(DIRL, CLOCKWISE);
        else if (inputLetter == ANTICLOCKWISE_LETTER) digitalWrite(DIRL, ANTICLOCKWISE);
        break;
    case 'c':
        if (inputLetter == CLOCKWISE_LETTER) digitalWrite(DIRC, CLOCKWISE);
        else if (inputLetter == ANTICLOCKWISE_LETTER) digitalWrite(DIRC, ANTICLOCKWISE);
        break;
    case 'r':
        if (inputLetter == CLOCKWISE_LETTER) digitalWrite(DIRR, CLOCKWISE);
        else if (inputLetter ==  ANTICLOKWISE_LETTER) digitalWrite(DIRR, ANTICLOCKWISE);
    default:
        break;
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

void verboseMessages() {
        debug("MPX: ");
        debugDec(mpxVoltage, 2);        // 4 Bytes
        debug("V ");
        // debug(sizeof(mpxVoltage));
        // debug("Bytes");
        debug("\tTDS: ");
        debugDec(tdsAverageVoltage,2);  // 4 Bytes
        debug("V / ");
        // debug(sizeof(tdsAverageVoltage));
        // debug("Bytes / ");
        debug(tdsValue);
        debug(" ppm ");
        // debug(sizeof(tdsValue));        // 2 Bytes
        // debug("Bytes");
        debug("\tTSW: ");
        debug((float)analogRead(TSWPin) / 1024 * 5);      // 2 Bytes
        debug("V / ");
        // debug(sizeof((float)analogRead(TSWPin) / 1024 * 5));
        // debug("Bytes / ");
        debug(ntuValue);                // 2 Bytes
        debug(" NTU ");
        // debug(sizeof(ntuValue));
        // debugln("Bytes");
}
