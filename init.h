#ifndef INIT_H
#define INIT_H

#include <avr/io.h>

// not really belong here...
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void init(void);
void (*register_timer_cb(void *cb))(void);

volatile unsigned int ms;

// wait for interrupt
#define wfi()							\
  do { __asm__ __volatile__(" sleep"); } while (0)

#endif
