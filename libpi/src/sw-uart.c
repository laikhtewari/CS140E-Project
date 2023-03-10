// simple software 8n1 UART bit-banging implemention.
//   - look for todo() and implement!
//
// NOTE: using usec is going to be too slow for high baudrate, 
// but should be ok for 115200.
//
#include "rpi.h"
#include "cycle-count.h"
#include "cycle-util.h"
#include "sw-uart.h"

// simple putk to the given <uart>
void sw_uart_putk(sw_uart_t *uart, const char *msg) {
    for(; *msg; msg++)
        sw_uart_put8(uart, *msg);
}

// helper: cleans up the code: do a write for <usec> microseconds
//
// code that uses it will have a lot of compounding error, however.  
// if you switch to using cycles for faster baud rates, you should
// instead use
//      <write_cyc_until> in libpi/include/cycle-util.h
static inline void timed_write(int pin, int v, unsigned usec) {
    gpio_write(pin,v);
    delay_us(usec);
}

// do this first: used timed_write to cleanup.
//  recall: time to write each bit (0 or 1) is in <uart->usec_per_bit>
void sw_uart_put8(sw_uart_t *uart, unsigned char c) {
    // timed_write(uart->tx, 0, uart->usec_per_bit);
    write_cyc_until(uart->tx, 0, cycle_cnt_read(), uart->cycle_per_bit);
    for (uint32_t i = 0; i < 8; i++) {
        // timed_write(uart->tx, c & 1, uart->usec_per_bit);
        write_cyc_until(uart->tx, c&1, cycle_cnt_read(), uart->cycle_per_bit);
        c >>= 1;
    }
    // timed_write(uart->tx, 1, uart->usec_per_bit);
    write_cyc_until(uart->tx, 1, cycle_cnt_read(), uart->cycle_per_bit);
}

// do this second: you can type in pi-cat to send stuff.
//      EASY BUG: if you are reading input, but you do not get here in 
//      time it will disappear.
int sw_uart_get8(sw_uart_t *uart) {
    todo("getc is an extension");
}

// setup the GPIO pins
sw_uart_t sw_uart_mk_helper(unsigned tx, unsigned rx,
        unsigned baud,
        unsigned cyc_per_bit,
        unsigned usec_per_bit) {

    // implement:
    //  1. set rx and tx as input/output.
    //  2: what is the default value of tx for 8n1?  make sure
    //     this is set!!
    gpio_set_input(rx);
    gpio_set_output(tx);
    gpio_set_on(tx);
    // gpio_set_pullup(tx);
    // delay_us(usec_per_bit);

    // check that the given values make sense.
    //
    // we give you the rest.
    // make sure the value makes sense.
    unsigned mhz = 700 * 1000 * 1000;
    unsigned derived = cyc_per_bit * baud;
    assert((mhz - baud) <= derived && derived <= (mhz + baud));
    // panic("cyc_per_bit = %d * baud = %d\n", cyc_per_bit, cyc_per_bit * baud);

    return (sw_uart_t) {
            .tx = tx,
            .rx = rx,
            .baud = baud,
            .cycle_per_bit = cyc_per_bit ,
            .usec_per_bit = usec_per_bit
    };
}
