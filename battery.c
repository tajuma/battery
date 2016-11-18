#include <avr/io.h>
#include "config.h"
#include "serial.h"

#define I2C_VERYSLOWMODE 1
#define I2C_TIMEOUT 2000
#define I2C_NOINTERRUPT 1

#include "SoftI2CMaster/SoftI2CMaster.h"

#define VOLTAGE 0x09
#define TEMPERATURE 0x08
#define CURRENT 0x0a
#define CAPACITY 0x10
#define TIME_TO_FULL 0x13
#define CHARGE 0x0d
#define STATUS 0x16
#define CYCLE_COUNT 0x17
#define MANUFACTURE_DATE 0x1b
#define MANUFACTURER_NAME 0x20
#define DEVICE_NAME 0x21
#define DEVICE_CHEMISTRY 0x22

static byte deviceAddress = 11;
static byte buf[64];

static int fetchString(byte func) {
  i2c_start(deviceAddress<<1 | I2C_WRITE);
  i2c_write(func);
  i2c_rep_start(deviceAddress<<1 | I2C_READ);

  byte i;
  byte size;
  size = i2c_read(false);
  if (size > sizeof(buf))
    size = sizeof(buf);
  for (i = 0; i < size-1; i++) {
    byte b1 = i2c_read(false);
    buf[i] = b1;
  }
  byte b1 = i2c_read(true);
  buf[i] = b1;
  buf[i+1] = 0;

  i2c_stop();

  return size;
}  

static int fetchWord(byte func) {
  i2c_start(deviceAddress<<1 | I2C_WRITE);
  i2c_write(func);
  i2c_rep_start(deviceAddress<<1 | I2C_READ);

  byte b1 = i2c_read(false);
  byte b2 = i2c_read(true);
  i2c_stop();

  return (int)b1|((( int)b2)<<8);
}

#if 0
static void scan(void) {
  byte i = 0;

  for (i= 0; i < 127; i++) {
      printString("Address ");
      printInteger(i);
      printString(" ");

      bool ack = i2c_start(i<<1 | I2C_WRITE);  
      i2c_stop();
      if (ack) {
	printString("OK\r\n");
	break;
      }
      else
	printString("NO\r\n");
    }
}
#endif

void battery_test(void) {
  //  scan();

  int v = fetchWord(VOLTAGE);
  printString("Voltage: ");
  printInteger(v);
  printNewline();

  // printString("Temp: ");
  // unsigned int tempk = fetchWord(TEMPERATURE);
  // float tempc = ((float)tempk)/10.0-273.15;
  //  printFloat(tempc);

  printString("Current (mA): ");
  int ma = fetchWord(CURRENT);
  printInteger(ma);
  printNewline();

  printString("Capacity (mAh): ");
  int mah = fetchWord(CAPACITY);
  printInteger(mah);
  printNewline();

  int ch = fetchWord(CHARGE);
  printString("Charge PCT: ");
  printInteger(ch);
  printNewline();

  printString("Minutes remaining for full charge: ");
  printInteger(fetchWord(TIME_TO_FULL));
  printNewline();

  printString("Status: ");
  printHex(fetchWord(STATUS));
  printNewline();

  printString("Cycle count: ");
  printHex(fetchWord(CYCLE_COUNT));
  printNewline();

  printString("Manufacture date: ");
  printHex(fetchWord(MANUFACTURE_DATE));
  printNewline();

  printString("Manufacturer name: ");
  fetchString(MANUFACTURER_NAME);
  printString((const char *)buf);
  printNewline();

  printString("Device name: ");
  fetchString(DEVICE_NAME);
  printString((const char *)buf);
  printNewline();

  printString("Device chemistry: ");
  fetchString(DEVICE_CHEMISTRY);
  printString((const char *)buf);
  printNewline();

  return;
}
