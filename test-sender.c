#include "rpi.h"
#include "magstripe.h"

void test_send_bit() {
    //Connect pin1 to pin1_in, pin2 to pin2_in

    unsigned pin1 = 14;
    unsigned pin2 = 15;
    gpio_set_output(pin1);
    gpio_set_output(pin2);

    unsigned pin1_in = 16;
    unsigned pin2_in = 17;
    gpio_set_input(pin1_in);
    gpio_set_input(pin2_in);
    
    send_bit(0b1, pin1, pin2);
    assert(gpio_read(pin1));
    assert(!gpio_read(pin2));

    send_bit(0b0, pin1, pin2);
    assert(gpio_read(pin1));
    assert(!gpio_read(pin2));
}

void test_convert_str_to_data() {
    char *s = "0?;9"; 
    unsigned n;
    uint8_t *out = convert_str_to_data(s, &n);
    assert(n == 4);
    assert(out[0] == 0b11110000);
    assert(out[1] == 0b11000101);

    s = "0?;95";
    out = convert_str_to_data(s, &n);
    assert(n == 5);
    assert(out[2] == 0b00001010);
}

void notmain() {
    test_send_bit();
    test_convert_str_to_data();
}