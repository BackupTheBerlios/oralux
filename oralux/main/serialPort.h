#ifndef SERIALPORT
#define SERIALPORT 1

enum serialPortIdentifier
  {
    TTYS0,
    TTYS1,
    MaxSerialPort,
  };

void serialPortInit();
int serialPortIsAvailable(int thePort);
void serialPortChoose( enum serialPortIdentifier * thePort);
enum serialPortIdentifier getEnumSerialPort(char* thePort);
char* serialPortGetString( enum serialPortIdentifier theValue);
enum sentence serialPortGetSentence( enum serialPortIdentifier theValue);

#endif
