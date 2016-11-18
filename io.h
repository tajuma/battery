#ifndef IO_H
#define IO_H

#define _high(x) do { PORT_ ## x |= _BV(BIT_ ## x); } while(0)
#define _low(x) do { PORT_ ## x &= ~_BV(BIT_ ## x); } while(0)
#define setPin(x, v) do { if ((v) == 0) _low(x); else _high(x); } while(0)
#define getPin(x) (((PIN_ ## x) & _BV(BIT_ ## x)) != 0)
#define setOutput(x) do { DDR_ ## x |= _BV(BIT_ ## x); } while(0)
#define setInput(x) do { DDR_ ## x &= ~_BV(BIT_ ## x); } while(0)

#endif
