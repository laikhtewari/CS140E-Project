// implement:
//  void uart_init(void)
//
//  int uart_can_get8(void);
//  int uart_get8(void);
//
//  int uart_can_put8(void);
//  void uart_put8(uint8_t c);
//
//  int uart_tx_is_empty(void) {
//
// see that hello world works.
//
//
#include "rpi.h"

enum {
    AUX_ENABLES = 0x20215004,
    AUX_MU_CNTL_REG = 0x20215060,
    AUX_MU_IIR_REG = 0x20215048,
    AUX_MU_IER_REG = 0x20215044,
    AUX_MU_BAUD_REG = 0x20215068,
    AUX_MU_LCR_REG = 0x2021504C,
    AUX_MU_IO_REG = 0x20215040,
    AUX_MU_STAT_REG = 0x20215064,
    AUX_MU_LSR_REG = 0x20215054,
    AUX_MU_MCR_REG = 0x20215050,
};

// called first to setup uart to 8n1 115200  baud,
// no interrupts.
//  - you will need memory barriers, use <dev_barrier()>
//
//  later: should add an init that takes a baud rate.
void uart_init(void) {
    // dev_barrier();

    // // disable uart in case previously running
    // unsigned auxenb = GET32(AUX_ENABLES);
    // unsigned mask = 0b1;
    // PUT32(AUX_ENABLES, auxenb & ~mask);
    dev_barrier();

    // set gpio (p102)
    gpio_set_function(14, GPIO_FUNC_ALT5);
    gpio_set_function(15, GPIO_FUNC_ALT5);
    dev_barrier();

    // enable mini uart (p9)
    PUT32(AUX_ENABLES, GET32(AUX_ENABLES) | 1);
    dev_barrier();

    // disable TX + RX (p17)
    PUT32(AUX_MU_CNTL_REG, 0);

    // disable interrupts (p12)
    PUT32(AUX_MU_IER_REG, 0);

    // set 8 bit (p14) 
    PUT32(AUX_MU_LCR_REG, 0b11);

    PUT32(AUX_MU_MCR_REG, 0);

    // clear FIFO queues (p13)
    PUT32(AUX_MU_IIR_REG, 0b110);

    // set baud rate (p11, 19)
    PUT32(AUX_MU_BAUD_REG, 270);

    // enable TX + RX (p17)
    PUT32(AUX_MU_CNTL_REG, 0b11);
    dev_barrier();
}

// disable the uart.
void uart_disable(void) {
    dev_barrier();
    uart_flush_tx();
    dev_barrier();
    unsigned auxenb = GET32(AUX_ENABLES);
    unsigned mask = 0b1;
    PUT32(AUX_ENABLES, auxenb & ~mask);
}


// returns one byte from the rx queue, if needed
// blocks until there is one.
int uart_get8(void) {
    dev_barrier();
	while (!uart_has_data());
    return GET32(AUX_MU_IO_REG) & ((1 << 8) - 1); 
}

// 1 = space to put at least one byte, 0 otherwise.
int uart_can_put8(void) {
    // make sure to have called dev barrier if coming from somewhere else
    // dev_barrier();
    return (GET32(AUX_MU_STAT_REG) & 0b10) >> 1;
    // return (GET32(AUX_MU_LSR_REG) & 0b100000) >> 5;
}

// put one byte on the tx qqueue, if needed, blocks
// until TX has space.
// returns < 0 on error.
int uart_put8(uint8_t c) {
    dev_barrier();
    while (!uart_can_put8());
    // unsigned io_reg = GET32(AUX_MU_IO_REG);
    // unsigned mask = 0xFF; // << 8;
    // unsigned data = c;// << 8;
    // unsigned out = (io_reg & ~mask) | data;
    PUT32(AUX_MU_IO_REG, c);
    return 0;
}

// simple wrapper routines useful later.

// 1 = at least one byte on rx queue, 0 otherwise
int uart_has_data(void) {
    // return GET32(AUX_MU_LSR_REG) & 1;
    return GET32(AUX_MU_STAT_REG) & 1;
}

// return -1 if no data, otherwise the byte.
int uart_get8_async(void) { 
    if(!uart_has_data())
        return -1;
    return uart_get8();
}

// 1 = tx queue empty, 0 = not empty.
int uart_tx_is_empty(void) {
    return (GET32(AUX_MU_STAT_REG) >> 9) & 1;
}

// flush out all bytes in the uart --- we use this when 
// turning it off / on, etc.
void uart_flush_tx(void) {
    while(!uart_tx_is_empty())
        ;
}
