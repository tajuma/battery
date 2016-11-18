#ifndef CONFIG_H
#define CONFIG_H

// battery connector pins
#define SDA_PIN 3 
#define SDA_PORT PORTD 
#define SCL_PIN 2
#define SCL_PORT PORTD

// led pins
#define PIN_led PINB
#define DDR_led DDRB
#define PORT_led PORTB
#define BIT_led 5

typedef unsigned char byte;
typedef byte boolean;
typedef boolean bool;

#define true (1)
#define false (0)


#endif
