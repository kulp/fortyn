/**
 * @file
 * *HC08 simulator header
 */

#ifndef HC08_H_
#define HC08_H_

#define _BSD_SOURCE

#include <sys/types.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MEMORY_SIZE     (1UL << 16)
#define MAX_INSN_LEN    4

#if !(defined(BYTE_ORDER) && (BYTE_ORDER == LITTLE_ENDIAN || \
                              BYTE_ORDER == BIG_ENDIAN))
#   error "Need BYTE_ORDER to be LITTLE_ENDIAN or BIG_ENDIAN to compile"
#endif

/// "as word pointer"
#define _WP(X) ((uint16_t*)&(X))

/// do byte swapping for 16-bit words
#if BYTE_ORDER == BIG_ENDIAN
#   define WORD(X) (*_WP(X))
#else
#   define WORD(X) (((*_WP(X) >> 8) & 0xFF) | (*_WP(X) << 8))
#endif

#define CHECK_OVERFLOW(a,m,r) \
    (( ((a) & 0x80) &  ((m) & 0x80) & ~((r) & 0x80)) \
   | (~((a) & 0x80) & ~((m) & 0x80) &  ((r) & 0x80)))
#define CHECK_HALF_CARRY(a,m,c) \
    ((((a) & 0xF) + ((m) & 0xF) + (c)) & (1 << 4))

#define CHECK_MSB(r)    (!!((r) & (1 << (8 * (sizeof (r)) - 1))))
#define CHECK_CARRY(r)  (!!((r) & (1 << (8 * (sizeof (r))    ))))

/** @todo */
#define HC08_IRQ_STATE 0

typedef uint16_t addr_t;

typedef struct hc_state_s {
    enum {
        POWER_OFF, STOP1, STOP2, STOP3, WAITING, RUNNING
    } state;                    ///< the run-state of the state machine
    long cycle_count;           ///< how many cycles have been consumed
    short offset;               ///< position within an op under decode
    struct {
        uint8_t  A;             ///< accumulator
        union {
            uint16_t word;
            struct {
#if BYTE_ORDER == BIG_ENDIAN
                uint8_t  H;     ///< index register (high)
                uint8_t  X;     ///< index register (low)
#else
                uint8_t  X;     ///< index register (low)
                uint8_t  H;     ///< index register (high)
#endif
            } bytes;
        } HX;
        union {
            addr_t word;       ///< stack pointer
            struct {
#if BYTE_ORDER == BIG_ENDIAN
                uint8_t SPH;    ///< high byte of SP
                uint8_t SPL;    ///< low byte of SP
#else
                uint8_t SPL;    ///< low byte of SP
                uint8_t SPH;    ///< high byte of SP
#endif
            } bytes;
        } SP;
        union {
            addr_t word;       ///< program counter
            struct {
#if BYTE_ORDER == BIG_ENDIAN
                uint8_t PCH;    ///< high byte of PC
                uint8_t PCL;    ///< low byte of PC
#else
                uint8_t PCL;    ///< low byte of PC
                uint8_t PCH;    ///< high byte of PC
#endif
            } bytes;
        } PC;
        union {
            uint8_t byte;       ///< contents of entire CCR register
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

    struct hc_state_s *prev;    ///< pointer to previous state
    struct hc_state_s *next;    ///< pointer to next state
} hc_state_t;

int hc_state_init(hc_state_t *st);
int hc_do_reset(hc_state_t *st);
int hc_do_op(hc_state_t *st);

static inline void hc_error(const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    vfprintf(stderr, fmt, vl);
    va_end(vl);
    abort();
}

/**
 * Determine which op page the operation at the current PC belongs to.
 *
 * @param st the processor state, which holds the PC and instruction to be
 * decoded at st->mem[st->regs.PC.word]
 *
 * @return the page number. No error conditions can be reported.
 */
int hc_op_page(hc_state_t *st);

#endif

/* vi:set ts=4 sw=4 et: */
/* vim:set syntax=c.doxygen: */


