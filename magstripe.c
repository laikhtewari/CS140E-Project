#include "magstripe.h"

uint8_t* convert_str_to_data(const char* str, unsigned* n){
    unsigned out_size = strlen(str) >> 1;
    if (strlen(str) & 1) {
        out_size++; // if strlen odd, want an extra byte for last character
    }
    *n = out_size;
    uint8_t* out = kmalloc(out_size); // allocate array for data

    for (unsigned i = 0; i < strlen(str); i++) {
        char value = str[i] - '0'; // '0' = 48, converts ascii #s to actual values
        assert(value < 16);
        if (i & 1) { // odd
            // want to fill top half 
            out[i >> 1] = bits_set(out[i >> 1], 4, 7, value);
        } else { // even
            // fill bottom half
            out[i >> 1] = bits_set(out[i >> 1], 0, 3, value);
        }
    }
    return out;
}

// n is number of digits
void send_raw_data(const unsigned pin1, const unsigned pin2, const uint8_t* const data, const unsigned n) {
    gpio_set_output(pin1); // want to minimize doing this
    gpio_set_output(pin2);

    for (unsigned i = 0; i < n; i++) {
        uint8_t val;
        if (n & 1) { // odd
            val = bits_get(data[i >> 1], 4, 7); //send upper half of byte
        } else {
            val = bits_get(data[i >> 1], 0, 3); // send lower half of byte
        }
        for (unsigned j = 0; j < 4; j++) {
            send_bit(bit_get(val, j), pin1, pin2); // send 4 bits
        }
        send_bit(compute_parity(val), pin1, pin2); // append parity bit
    }
}

void send_bit(unsigned v, unsigned pin1, unsigned pin2) {
    assert(v < 2);
    if (v) {  
        gpio_set_on(pin1); // test exact polarity
        gpio_set_off(pin2);
    } else {
        gpio_set_on(pin2);
        gpio_set_off(pin1);
    }
}

void send_str(const unsigned pin1, const unsigned pin2, const char* const str) {
    unsigned n;
    uint8_t* data = convert_str_to_data(str, &n);
    send_raw_data(pin1, pin2, data, n);
}

// pin1, pin2 should already be initialized!
void send_bit(unsigned v, unsigned pin1, unsigned pin2);

unsigned compute_parity(uint8_t v) {
    assert(v < 16); // only check parity on lower 4 bits
    unsigned parity = 1;
    for (unsigned i = 0; i < 4; i++) {
        if (v & 1) {
            parity = ~parity;
        }
        v >>= 1;
    }
    return parity & 1;
}

uint8_t reverse_endianness(uint8_t v) {
    assert(v < 16);
    uint8_t out = 0;
    for (unsigned i = 0; i < 4; i++) {
        bits_set(out, 3 - i, 3 - i, bit_get(v, i));
    }
    return v;
}