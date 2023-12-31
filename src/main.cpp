#include <Arduino.h>
#include "main.hpp"
#include "debug.hpp"

const uint8_t MPXPin = A0;   // MPX2100AP (pressure in kPa) sensor
const uint8_t TDSPin = A3;   // TDS (Total Dissolved Solids in ppm) sensor
const uint8_t TSWPin = A2;   // TSW-20M Turbidity (Nephelometer in NTU) sensor
const uint8_t LMPin = A1;   // LM35 temperature sensor 

/* Left motor */
const uint8_t DIRL = 2;
const uint8_t PWML = 3;

/* Center motor */
const uint8_t DIRC = 12;
const uint8_t PWMC = 10;

/* Right motor */
const uint8_t DIRR = 13;
const uint8_t PWMR = 11;

void turnMotor(char motor, char turnDirection, int dutyCycle);
int getMedianNum(int bArray[], int iFilterLen);
void verboseMessages();

void setup()
{
    Serial.begin(9600);

    pinMode(MPXPin, INPUT);
    pinMode(TDSPin, INPUT);
    pinMode(TSWPin, INPUT);

    pinMode(DIRL, LOW);
    pinMode(PWML, LOW);
    pinMode(DIRC, LOW);
    pinMode(PWMC, LOW);
    pinMode(DIRR, LOW);
    pinMode(PWMR, LOW);

    // while (debugAvailable() <= 0 || debugRead() != 's') 
    // {
    //     debugln("Send 's' to start: ");
    //     debugDelay(200); 
    // }
    debugln("Lectura de: presion (manometro), turbidez (turbidimetro), tds (conductimetro) y velocidad");
    debugln("Conversion a medidas utiles: tension => profundidad, turbidez, total de solidos disueltos");
    debugln("=========================================================================================");
    debugln(" ");
    debugln("MPX: Tension\tTDS: Tension / TDS   \tTSW: Tension / NTU   ");
    debugln("------------\t---------------------\t---------------------");
}

void loop()
{   
    while (Serial.available() > 0)
    {
        char inputByte = Serial.read();
        debug("Serial is available with byte '");
        debug(inputByte);
        debugln("' read");
        
        if (inputByte != '\n')
        {
            command[command_pos] = inputByte;
            debug("Command_pos: ");
            debugln(command_pos);
            command_pos++;
        } else {
            command[0] = 'c'; // First character indicates whether captain mode is on/off, 'c' = on
            command_pos = 1;
            debugln("newline");
            debug("Characters in serial buffer left: ");
            debugln(Serial.available());
            break;
        }
        debug("Characters in serial buffer left: ");
        debugln(Serial.available());
    }

    if (command[0] == 'c')
    {
        commandKey = command[1];
        turn = command[2];
        switch (commandKey)
        {
        case 'a':
            digitalWrite(DIRL, LOW);
            analogWrite(PWML, STOP);
            break;
        case 'q':
            turnMotor('l', turn, LOW_SPEED);
            break;
        case 'w':
            turnMotor('l', turn, HALF_SPEED);
            break;
        case 'e':
            turnMotor('l', turn, HIGH_SPEED);
            break;
        case 'r':
            turnMotor('l', turn, FULL_SPEED);
            break;

        case 's':
            digitalWrite(DIRC, LOW);
            analogWrite(PWMC, STOP);
            break;
        case 'f':
            turnMotor('c', turn, LOW_SPEED);
            break;
        case 't':
            turnMotor('c', turn, HALF_SPEED);
            break;
        case 'y':
            turnMotor('c', turn, HIGH_SPEED);
            break;
        case 'h':
            turnMotor('c', turn, FULL_SPEED);
            break;

        case 'd':
            digitalWrite(DIRR, LOW);
            analogWrite(PWMR, STOP);
            break;
        case 'u':
            turnMotor('r', turn, LOW_SPEED);
            break;
        case 'i':
            turnMotor('r', turn, HALF_SPEED);
            break;
        case 'o':
            turnMotor('r', turn, HIGH_SPEED);
            break;
        case 'p':
            turnMotor('r', turn, FULL_SPEED);
            break;
        case '\r':
            debugln("\r");
            break;

        default:
            debug("Command key '");
            debug(commandKey);
            debugln("' not recognized.");
            break;
        }

        command[0] = 'n'; // captain mode off
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
        // debugMessages();

        // Serial communication (frame sent to master)
        // debug("\tData in array: ");
        Serial.print(tempValue, 1);    // 4 bytes
        Serial.print(" ");          // 1 byte
        Serial.print(depth, 1);        // 4 bytes
        Serial.print(" ");          // 1 byte
        Serial.print(tdsValue);     // 2 bytes
        Serial.print(" ");          // 1 byte
        Serial.println(ntuValue);     // 2 bytes
    }
}

void turnMotor(char motor, char turnDirection, int dutyCycle)
{
    switch (motor)
    {
    case 'l':
        if (turnDirection == CLOCKWISE_LETTER) digitalWrite(DIRL, CLOCKWISE);
        else if (turnDirection == ANTICLOCKWISE_LETTER) digitalWrite(DIRL, ANTICLOCKWISE);
        analogWrite(PWML, dutyCycle);
        break;
    case 'c':
        if (turnDirection == CLOCKWISE_LETTER) digitalWrite(DIRC, CLOCKWISE);
        else if (turnDirection == ANTICLOCKWISE_LETTER) digitalWrite(DIRC, ANTICLOCKWISE);
        analogWrite(PWMC, dutyCycle);
        break;
    case 'r':
        if (turnDirection == CLOCKWISE_LETTER) digitalWrite(DIRR, CLOCKWISE);
        else if (turnDirection ==  ANTICLOCKWISE_LETTER) digitalWrite(DIRR, ANTICLOCKWISE);
        analogWrite(PWMR, dutyCycle);
    default:
        debug("Turn key '");
        debug(turnDirection);
        debugln("' not recognized.");
        break;
    }
    debug("Command ");
    debug(motor);
    debug(turnDirection);
    debugln(dutyCycle);
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
