#include <stdint.h>
#include "rpi.h"
#include "bit-support.h"

// Returns a heap allocated 
uint8_t* convert_str_to_data(const char* str, unsigned* n);

// sends n (4 bit) digits from data (each element in data contains up to 2 digits) in little endian order
void send_raw_data(const unsigned pin1, const unsigned pin2, const uint8_t* const data, const unsigned n);

void send_str(const unsigned pin1, const unsigned pin2, const char* const str);

// pin1, pin2 should already be initialized!
void send_bit(unsigned v, unsigned pin1, unsigned pin2);

unsigned compute_parity(uint8_t v);

uint8_t reverse_endianness(uint8_t v);

