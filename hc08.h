/**
 * @file
 * *HC08 simulator header
 */

#ifndef HC08_H_
#define HC08_H_

#include <stdint.h>

#define MEMORY_SIZE (1UL << 16)

#if !defined(_BIG_ENDIAN) && !defined(_LITTLE_ENDIAN)
#   error "Define _BIG_ENDIAN or _LITTLE_ENDIAN"
#endif

typedef struct hc_state_s {
    enum {
        POWER_OFF, STOP1, STOP2, STOP3, RUNNING
    } state;                    ///< the run-state of the state machine
    short offset;               ///< position within an op under decode
    struct {
        uint8_t  A;             ///< accumulator
        uint8_t  H;             ///< index register (high)
        uint8_t  X;             ///< index register (low)
        union {
            uint16_t whole;     ///< stack pointer
            struct {
#if _BIG_ENDIAN
                uint8_t SPH;    ///< high byte of SP
                uint8_t SPL;    ///< low byte of SP
#else
                uint8_t SPL;    ///< low byte of SP
                uint8_t SPH;    ///< high byte of SP
#endif
            } bytes;
        } SP;
        union {
            uint16_t whole;     ///< program counter
            struct {
#if _BIG_ENDIAN
                uint8_t PCH;    ///< high byte of PC
                uint8_t PCL;    ///< low byte of PC
#else
                uint8_t PCL;    ///< low byte of PC
                uint8_t PCH;    ///< high byte of PC
#endif
            } bytes;
        } PC;
        union {
            uint8_t whole;      ///< contents of entire CCR register
            struct {
                /// @note We're depending on LSB->MSB ordering of bits within
                /// a word here, though this ordering is not defined by C.
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

int hc_state_init(hc_state_t *st);
int hc_do_reset(hc_state_t *st);
int hc_do_op(hc_state_t *st);

#endif

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */

