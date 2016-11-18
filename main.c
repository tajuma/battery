#include "config.h"
#include "init.h"
#include "io.h"
#include "serial.h"
#include "battery.h"

static void setup(void) {
  beginSerial(115200);

  setOutput(led);
  setPin(led, 0);
}

int main(void) {
  /* init board functions */
  init();
  
  /* init program functions */
  setup();

  printString("Battery test\r\n");

  battery_test();

  return 0;
}
