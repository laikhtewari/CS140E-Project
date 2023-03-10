#include "magstripe.h"

uint8_t* convert_str_to_data(const char* str, unsigned* n){
    unsigned out_size = strlen(str) >> 1;
    if (strlen(str) & 1) {
        out_size++; // if strlen odd, want an extra byte for last character
    }
    *n = out_size;
    uint8_t* out = kmalloc(out_size);

    for (unsigned i = 0; i < strlen(str); i++) {
        char value = str[i] - '0'; // '0' = 48, converts ascii #s to actual values
        assert(value < 16);
        if (i & 1) {
            // want to fill top half 
            out[i >> 1] = bits_set(out[i >> 1], 4, 7, value);
        } else {
            // fill bottom half
            out[i >> 1] = bits_set(out[i >> 1], 0, 3, value);
        }
    }
    return out;
}

// n is number of digits
void send_raw_data(const unsigned pin1, const unsigned pin2, const uint8_t* const data, const unsigned n) {
    gpio_set_output(pin1);
    gpio_set_output(pin2);
    
    for (unsigned i = 0; i < n; i++) {
        uint8_t val;
        if (n & 1) {
            val = bits_get(data[i >> 1], 4, 7);
        } else {
            val = bits_get(data[i >> 1], 0, 3);
        }
        for (unsigned j = 0; j < 4; j++) {
            send_bit(bit_get(val, j), pin1, pin2);
        }
        send_bit(compute_parity(val), pin1, pin2);
    }
}

void send_bit(unsigned v, unsigned pin1, unsigned pin2) {
    assert(v < 2);
    if (v) {
        gpio_set_on(pin1);
        gpio_set_off(pin2);
    } else {
        gpio_set_on(pin2);
        gpio_set_off(pin1);
    }
}
