#ifndef SERIAL_H
#define SERIAL_H

void beginSerial(long);
void serialWrite(unsigned char);
int serialAvailable(void);
int serialRead(void);
void serialFlush(void);

void printByte(unsigned char c);
void printNewline(void);
void printString(const char *s);
void printInteger(long n);
void printHex(unsigned long n);
void printOctal(unsigned long n);
void printBinary(unsigned long n);

#endif
