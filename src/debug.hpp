#define DEBUG 1

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugDec(x,d) Serial.print(x,d)
#define debugln(x) Serial.println(x)
#define debuglnDec(x,d) Serial.println(x,d)
#define debugDelay(x) delay(x)
#define debugRead() Serial.read()
#define debugAvailable() Serial.available()
#define debugMessages() verboseMessages()
#else
#define debug(x)
#define debugDec(x,d)
#define debugln(x)
#define debuglnDec(x,d)
#define debugDelay(x)
#define debugRead() 's'
#define debugAvailable() 1
#define debugMessages()
#endif
