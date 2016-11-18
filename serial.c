/*
  wiring_serial.c - serial functions.
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

  $Id: wiring.c 248 2007-02-03 15:36:30Z mellis $
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "serial.h"

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define RX_BUFFER_SIZE 128
static unsigned char rx_buffer[RX_BUFFER_SIZE];
static uint8_t rx_buffer_head;
static uint8_t rx_buffer_tail;

static void printIntegerInBase(unsigned long n, uint8_t base);

void beginSerial(long baud) {
#if defined(__AVR_ATmega328P__)
  UBRR0H = ((F_CPU / 16 + baud / 2) / baud - 1) >> 8;
  UBRR0L = ((F_CPU / 16 + baud / 2) / baud - 1);
  
  // enable rx and tx
  sbi(UCSR0B, RXEN0);
  sbi(UCSR0B, TXEN0);
  
  // enable interrupt on complete reception of a byte
  sbi(UCSR0B, RXCIE0);
#else
  UBRRH = ((F_CPU / 16 + baud / 2) / baud - 1) >> 8;
  UBRRL = ((F_CPU / 16 + baud / 2) / baud - 1);
  
  // enable rx and tx
  sbi(UCSRB, RXEN);
  sbi(UCSRB, TXEN);
  
  // enable interrupt on complete reception of a byte
  sbi(UCSRB, RXCIE);
#endif
  // defaults to 8-bit, no parity, 1 stop bit
}

void serialWrite(unsigned char c) {
#if defined(__AVR_ATmega328P__)
  while (!(UCSR0A & (1 << UDRE0)))
    ;

  UDR0 = c;
#else
  while (!(UCSRA & (1 << UDRE)))
    ;

  UDR = c;
#endif
}

int serialAvailable() {
  // 1844
  // boolean version
  // return rx_buffer_head != rx_buffer_tail;

  // 1848
  // RX_BUFFER_SIZE must be <= 128
  uint8_t n = rx_buffer_head;
  n += rx_buffer_head < rx_buffer_tail ? RX_BUFFER_SIZE : 0;
  return n - rx_buffer_tail;
 
  // 1860
  // return rx_buffer_head > rx_buffer_tail ?
  //   rx_buffer_head - rx_buffer_tail :
  //   (rx_buffer_head + RX_BUFFER_SIZE) - rx_buffer_tail;

  // 1926
  // return (RX_BUFFER_SIZE + rx_buffer_head - rx_buffer_tail) % RX_BUFFER_SIZE;
}

int serialRead() {
  // if the head isn't ahead of the tail, we don't have any characters
  if (rx_buffer_head == rx_buffer_tail) {
    return -1;
  } else {
    unsigned char c = rx_buffer[rx_buffer_tail];
    rx_buffer_tail = (rx_buffer_tail + 1) % RX_BUFFER_SIZE;
    return c;
  }
}

void serialFlush() {
  // don't reverse this or there may be problems if the RX interrupt
  // occurs after reading the value of rx_buffer_head but before writing
  // the value to rx_buffer_tail; the previous value of rx_buffer_head
  // may be written to rx_buffer_tail, making it appear as if the buffer
  // were full, not empty.
  rx_buffer_head = rx_buffer_tail;
}

#if defined(__AVR_ATmega328P__)
ISR(USART_RX_vect)
#else
ISR(USART_RX_vect)
#endif
{
#if defined(__AVR_ATmega328P__)
  unsigned char c = UDR0;
#else
  unsigned char c = UDR;
#endif

  uint8_t i = (rx_buffer_head + 1) % RX_BUFFER_SIZE;
  
  // if we should be storing the received character into the location
  // just before the tail (meaning that the head would advance to the
  // current location of the tail), we're about to overflow the buffer
  // and so we don't write the character or advance the head.
  if (i != rx_buffer_tail) {
    rx_buffer[rx_buffer_head] = c;
    rx_buffer_head = i;
  }
}

void printMode(int mode) {
  // do nothing, we only support serial printing, not lcd.
}

void printByte(unsigned char c) {
  serialWrite(c);
}

void printNewline() {
  printByte('\r');
  printByte('\n');
}

void printString(const char *s) {
  while (*s)
    printByte(*s++);
}

static void printIntegerInBase(unsigned long n, uint8_t base) { 
  unsigned char buf[8 * sizeof(long)]; // Assumes 8-bit chars. 
  uint8_t i = 0;
  
  if (n == 0) {
    printByte('0');
    return;
  } 

  while (n > 0) {
    buf[i++] = n % base;
    n /= base;
  }

  for (; i > 0; i--)
    printByte(buf[i - 1] < 10 ?
	      '0' + buf[i - 1] :
	      'A' + buf[i - 1] - 10);
}

void printInteger(long n) {
  if (n < 0) {
    printByte('-');
    n = -n;
  }
  
  printIntegerInBase(n, 10);
}

void printHex(unsigned long n) {
  printIntegerInBase(n, 16);
}

void printOctal(unsigned long n) {
  printIntegerInBase(n, 8);
}

void printBinary(unsigned long n) {
  printIntegerInBase(n, 2);
}

