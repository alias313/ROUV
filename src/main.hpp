#define VREF 5.0      // analog reference voltage(Volt) of the ADC
#define SCOUNT  30           // sum of sample point

#define STOP 0
#define LOW_SPEED 64   // ceiling(25% of 255)
#define HALF_SPEED 128 // ceiling(50% of 255)
#define HIGH_SPEED 196 // ceiling(75% of 255)
#define FULL_SPEED 250 // ceiling(98% of 255)

#define CLOCKWISE_LETTER 'k'
#define ANTICLOCKWISE_LETTER 'a'
#define CLOCKWISE 1
#define ANTICLOCKWISE 0

char command;   // command to chose the motor & speed
char turn;      // 'k' turns clockwise, 'a' turns anticlockwise

unsigned int sampleRate = 20U;
unsigned int window = 25;   // Number of samples taken

// LM35 (Temperature sensor)
float lmVoltage = 0.0;
float tempValue = 0.0;

// MPX (Pressure meter)
float mpxVoltage = 0.0;
float depthCoefficient =  1000 / (163.46 * 2.0);
int depthOffset = 10;
float depth = 0.0;

// TDS (TDS meter)
int analogBuffer[SCOUNT];    // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0, copyIndex = 0;
int tdsValue = 0;
float tdsAverageVoltage = 0;

// TSW (Turbidity meter)
float tswVoltage = 0.0;
int ntuValue = 0;
