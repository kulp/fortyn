/**
 * @file
 * *HC08 simulator header
 */

#ifndef HC08_H_
#define HC08_H_

#include <stdint.h>

#define MEMORY_SIZE (1UL << 15)

typedef struct hc_state_s {
    struct {
        uint8_t  A;             ///< accumulator
        uint8_t  H;             ///< index register (high)
        uint8_t  X;             ///< index register (low)
        uint16_t SP;            ///< stack pointer
        uint16_t PC;            ///< program counter
        union {
            uint8_t whole;      ///< contents of entire CCR register
            struct {
                /// @todo conditional compilation for ordering of bits in
                /// bitfield, since ordering is not defined by C
                unsigned C:1;   ///< carry
                unsigned Z:1;   ///< zero
                unsigned N:1;   ///< negative
                unsigned I:1;   ///< interrupt mask
                unsigned H:1;   ///< half-carry (from bit 3)
                unsigned  :1;   ///< 1
                unsigned  :1;   ///< 1
                unsigned V:1;   ///< two's complement overflow
            } bits;
        } CCR;
    } regs;
    uint8_t mem[MEMORY_SIZE];
} hc_state_t;

#endif

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */

