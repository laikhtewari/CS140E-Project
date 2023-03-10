#ifndef __VECTOR_BASE_SET_H__
#define __VECTOR_BASE_SET_H__
#include "libc/bit-support.h"
#include "asm-helpers.h"

/*
 * vector base address register:
 *   3-121 --- let's us control where the exception jump table is!
 *
 * defines: 
 *  - vector_base_set  
 *  - vector_base_get
 */
cp_asm_get(vector_base_aux, p15, 0, c12, c0, 0);
cp_asm_set(vector_base_aux, p15, 0, c12, c0, 0);

// return the current value vector base is set to.
static inline void *vector_base_get(void) {
    // void *out = NULL;
    // asm volatile ("mrc p15, #0, %0, c12, c0, #0" : "=r" (out) : : ); 
    // return out;
    return (void *)vector_base_aux_get();
}

// check that not null and alignment is good.
static inline int vector_base_chk(void *vector_base) {
    return vector_base && ((((unsigned)vector_base) & 0b11111) == 0);
}

// set vector base: must not have been set already.
static inline void vector_base_set(void *vec) {
    if(!vector_base_chk(vec))
        panic("illegal vector base %p\n", vec);

    void *v = vector_base_get();
    // trace("Got vector base %p\n", v);
    if(v) 
        panic("vector base register already set=%p\n", v);

    vector_base_aux_set((uint32_t)vec);

    v = vector_base_get();
    if(v != vec)
        panic("set vector=%p, but have %p\n", vec, v);
}

// reset vector base and return old value: could have
// been previously set.
static inline void *
vector_base_reset(void *vec) {
    if(!vector_base_chk(vec))
        panic("illegal vector base %p\n", vec);

    void *old_vec = vector_base_get();
    vector_base_aux_set((uint32_t)vec);

    assert(vector_base_get() == vec);
    return old_vec;
}
#endif
