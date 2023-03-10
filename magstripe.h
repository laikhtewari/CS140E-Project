#include <stdint.h>
#include "rpi.h"
#include "bit-support.h"

// Returns a heap allocated 
uint8_t* convert_str_to_data(const char* str, unsigned* n);

// sends n (4 bit) digits from data (each element in data contains up to 2 digits) in little endian order
void send_raw_data(const unsigned pin1, const unsigned pin2, const uint8_t* const data, const unsigned n);

void send_str(const unsigned pin1, const unsigned pin2, const char* const str) {
    unsigned n;
    uint8_t* data = convert_str_to_data(str, &n);
    send_raw_data(pin1, pin2, data, n);
}

void send_bit(unsigned v, unsigned pin1, unsigned pin2);

unsigned compute_parity(uint8_t v) {
    assert(v < 16); // only check parity on lower 4 bits
    unsigned parity = 1;
    for (unsigned i = 0; i < 4; i++) {
        if (v & 1) {
            parity = ~parity;
        }
        v >> 1;
    }
    return parity & 1;
}

uint8_t reverse_endianness(uint8_t v) {
    assert(v < 16);
    uint8_t out = 0;
    for (unsigned i = 0; i < 4; i++) {
        bits_set(out, 3 - i, 3 - i, bit_get(v, i));
    }
}

