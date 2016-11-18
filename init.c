/*
  wiring.c - Partial implementation of the Wiring API for the ATmega8.
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  $Id: wiring.c 254 2007-04-20 23:17:38Z mellis $
*/

#include <avr/io.h>
#include <avr/interrupt.h>

#include "init.h"

void init(void) {
  // set a2d reference to AVCC (5 volts)
  cbi(ADMUX, REFS1);
  sbi(ADMUX, REFS0);

  // set a2d prescale factor to 128
  // 16 MHz / 128 = 125 KHz, inside the desired 50-200 KHz range.
  // XXX: this will not work properly for other clock speeds, and
  // this code should use F_CPU to determine the prescale factor.
  sbi(ADCSRA, ADPS2);
  sbi(ADCSRA, ADPS1);
  sbi(ADCSRA, ADPS0);

  // enable a2d conversions
  sbi(ADCSRA, ADEN);

  // the bootloader connects pins 0 and 1 to the USART; disconnect them
  // here so they can be used as normal digital i/o; they will be
  // reconnected in Serial.begin()
#if defined(__AVR_ATmega328P__)
  UCSR0B = 0;
#else
  UCSRB = 0;
#endif

  // enable power save modes (making wfi/sleep effective)
  SMCR |= (1 << SE);

  sei();
}
