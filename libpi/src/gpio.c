/*
 * Implement the following routines to set GPIO pins to input or output,
 * and to read (input) and write (output) them.
 *
 * DO NOT USE loads and stores directly: only use GET32 and PUT32
 * to read and write memory.  Use the minimal number of such calls.
 *
 * See rpi.h in this directory for the definitions.
 */
#include "rpi.h"

// see broadcomm documents for magic addresses.
enum {
    GPIO_BASE = 0x20200000,
    gpio_set0     = (GPIO_BASE + 0x1C),
    gpio_clr0     = (GPIO_BASE + 0x28),
    gpio_lev0     = (GPIO_BASE + 0x34),
    gpio_pud      = (GPIO_BASE + 0x94),
    gpio_pudclk0  = (GPIO_BASE + 0x98),
};

enum {
  PINS_PER_FSEL = 10,
  BITS_PER_PIN_FSEL = 3,
  OUTPUT_FUNC = 0b001,
  INPUT_FUNC = 0b000,
};

//
// Part 1 implement gpio_set_on, gpio_set_off, gpio_set_output
//

void gpio_set_function(unsigned pin, gpio_func_t function) {
  if (pin >= 32) {
    return;
  }

  if (function > 7) {
    return;
  }

  unsigned *fsel_reg = (unsigned *)GPIO_BASE + (pin / PINS_PER_FSEL);
  unsigned shift = (pin % PINS_PER_FSEL) * BITS_PER_PIN_FSEL;
  unsigned mask = ((0b1 <<  BITS_PER_PIN_FSEL) - 1) << shift;
  unsigned value = function << shift;

  unsigned curr = GET32((int)fsel_reg);
  curr &= ~mask;
  curr |= value;
  PUT32((int)fsel_reg, curr);
}

void gpio_set_output(unsigned pin) {
  gpio_set_function(pin, GPIO_FUNC_OUTPUT);
}

void gpio_set_input(unsigned pin) {
  gpio_set_function(pin, GPIO_FUNC_INPUT);
}

// set <pin> to be an output pin.
//
// note: fsel0, fsel1, fsel2 are contiguous in memory, so you
// can (and should) use array calculations!
// void gpio_set_output(unsigned pin) {
//   if(pin >= 32)
//       return;

//   // implement this
//   // use <gpio_fsel0>

//   unsigned *fsel_reg = (unsigned *)GPIO_BASE + (pin / PINS_PER_FSEL);
//   unsigned shift = (pin % PINS_PER_FSEL) * BITS_PER_PIN_FSEL;
//   unsigned mask = ((0b1 <<  BITS_PER_PIN_FSEL) - 1) << shift;
//   unsigned value = OUTPUT_FUNC << shift;

//   unsigned curr = GET32((int)fsel_reg);
//   curr &= ~mask;
//   curr |= value;
//   PUT32((int)fsel_reg, curr);
// }

// set GPIO <pin> on.
void gpio_set_on(unsigned pin) {
    if(pin >= 32)
        return;
  // implement this
  // use <gpio_set0>
  PUT32((int)gpio_set0, 0b1 << pin);
}

// set GPIO <pin> off
void gpio_set_off(unsigned pin) {
    if(pin >= 32)
        return;
  // implement this
  // use <gpio_clr0>
  PUT32((int)gpio_clr0, 0b1 << pin);
}

// set <pin> to <v> (v \in {0,1})
void gpio_write(unsigned pin, unsigned v) {
    if(v)
        gpio_set_on(pin);
    else
        gpio_set_off(pin);
}

//
// Part 2: implement gpio_set_input and gpio_read
//

// set <pin> to input.
// void gpio_set_input(unsigned pin) {
//   // implement.
//   if (pin >= 32) {
//     return;
//   }
//   unsigned *fsel_reg = (unsigned *)GPIO_BASE + (pin / PINS_PER_FSEL);
//   unsigned shift = (pin % PINS_PER_FSEL) * BITS_PER_PIN_FSEL;
//   // unsigned mask = ((0b1 <<  BITS_PER_PIN_FSEL) - 1) << shift;
//   unsigned mask = 0b111 << shift;
//   unsigned value = INPUT_FUNC << shift;

//   unsigned curr = GET32((int)fsel_reg);
//   curr &= ~mask;
//   curr |= value;
//   PUT32((int)fsel_reg, curr);
// }

// return the value of <pin>
int gpio_read(unsigned pin) {
  unsigned v = 0;

  // implement.
  if (pin >= 32) {
    return DEV_VAL32(0);
  }

  v = GET32((int)gpio_lev0); // + (pin % 32)); // mod 32 shouldn't be needed yet
  v &= 0b1 << pin; // modify if supporting > 32
  v >>= pin;
  return DEV_VAL32(v);
}

// void gpio_set_pud(unsigned pin, unsigned func) {
//   if (pin >= 32) {
//     return;
//   }

//   PUT32((int)gpio_pud, func); // can write only since clk reg must also be set for effect
//   delay_cycles(150);
//   PUT32((int)gpio_pudclk0, 0b1 << pin);
//   delay_cycles(150);
//   PUT32((int)gpio_pudclk0, 0); // clr
// }

// void gpio_set_pullup(unsigned pin) {
//   if (pin >= 32) {
//     return;
//   }
//   gpio_set_pud(pin, 0b10);
// }

// void gpio_set_pulldown(unsigned pin) {
//   if (pin >= 32) {
//     return;
//   }
//   gpio_set_pud(pin, 0b01);
// }

// void gpio_pud_off(unsigned pin) {
//   if (pin >= 32) {
//     return;
//   }
//   gpio_set_pud(pin, 0b00);
// }