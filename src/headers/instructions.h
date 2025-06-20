#ifndef __OXEY_CCE_INSTRUCTIONS_H
#define __OXEY_CCE_INSTRUCTIONS_H

#include <limits.h>

#include "cpu.h"

#define INSTRUCTION_WIDTH 8

typedef void (*Instruction)(CPU *cpu);
#define UNUSED(x) (void)(x)
#define INSTRUCTION(name) static void name(CPU *cpu)
#define UPDATE_ZF(src) UPDATE_FLAGS(src == 0, zf)
#define UPDATE_SF(src) UPDATE_FLAGS(src &SIGN_BIT, sf)

INSTRUCTION(NOOP) { PC++; }
INSTRUCTION(HALT) { UNUSED(cpu); }
INSTRUCTION(EI) {
    PC++;
    FLAGS = set_if(FLAGS);
}
INSTRUCTION(DI) {
    PC++;
    FLAGS = unset_if(FLAGS);
}
INSTRUCTION(ET) {
    PC++;
    FLAGS = set_tf(FLAGS);
}
INSTRUCTION(DT) {
    PC++;
    FLAGS = unset_tf(FLAGS);
}
INSTRUCTION(CLRA) {
    PC++;
    ACC = 0;
    FLAGS = set_zf(FLAGS);
}
INSTRUCTION(RESET) { resetCpu(cpu); }

#define LOAD(variation, pc_inc, src) \
    INSTRUCTION(LOAD_##variation) {  \
        pc_inc;                      \
        ACC = src;                   \
        UPDATE_ZF(ACC);              \
        UPDATE_SF(ACC);              \
    }

LOAD(I, PC += 2, MEMORY(PC - 1))
LOAD(IM, PC += 3, MEMORY((((uint16_t)MEMORY(PC - 2) << 8) | (uint16_t)MEMORY(PC - 1))))
LOAD(ML, PC += 1, MEMORY(L))
LOAD(MHL, PC += 1, (MEMORY(HL)))
LOAD(R0, PC += 1, R0)
LOAD(R1, PC += 1, R1)
LOAD(L, PC += 1, L)
LOAD(H, PC += 1, H)

#define STORE(variation, pc_inc, dest) \
    INSTRUCTION(STORE_##variation) {   \
        pc_inc;                        \
        uint8_t val = ACC;             \
        dest = val;                    \
        UPDATE_ZF(ACC);                \
        UPDATE_SF(ACC);                \
    }

STORE(IM, PC += 3, MEMORY(((uint16_t)MEMORY(PC - 2) << 8) | (uint16_t)MEMORY(PC - 1)))
STORE(ML, PC += 1, MEMORY(L))
STORE(MHL, PC += 1, MEMORY(HL))
STORE(R0, PC += 1, R0)
STORE(R1, PC += 1, R1)
STORE(L, PC += 1, L)
STORE(H, PC += 1, H)

#define XCH(variation, pc_inc, src) \
    INSTRUCTION(XCH_##variation) {  \
        pc_inc;                     \
        uint8_t help = ACC;         \
        ACC = src;                  \
        src = help;                 \
        UPDATE_ZF(ACC);             \
        UPDATE_SF(ACC);             \
    }

XCH(IM, PC += 3, MEMORY(((uint16_t)MEMORY(PC - 2) << 8) | (uint16_t)MEMORY(PC - 1)))
XCH(ML, PC += 1, MEMORY(L))
XCH(MHL, PC += 1, MEMORY(HL))
XCH(R0, PC += 1, R0)
XCH(R1, PC += 1, R1)
XCH(L, PC += 1, L)
XCH(H, PC += 1, H)

#define ADD(variation, pc_inc, src)  \
    INSTRUCTION(ADD_##variation) {   \
        pc_inc;                      \
        uint8_t val = src;           \
        ACC += val;                  \
        UPDATE_ZF(ACC);              \
        UPDATE_SF(ACC);              \
        UPDATE_FLAGS(ACC < val, cf); \
    }

ADD(I, PC += 2, MEMORY(PC - 1))
ADD(ACC, PC += 1, ACC)
ADD(ML, PC += 1, MEMORY(L))
ADD(MHL, PC += 1, MEMORY(HL))
ADD(R0, PC += 1, R0)
ADD(R1, PC += 1, R1)
ADD(L, PC += 1, L)
ADD(H, PC += 1, H)

#define ADC(variation, pc_inc, src)                \
    INSTRUCTION(ADC_##variation) {                 \
        pc_inc;                                    \
        uint16_t total = ACC + src + CARRY_FLAG(); \
        ACC = (uint8_t)total;                      \
        UPDATE_ZF(ACC);                            \
        UPDATE_SF(ACC);                            \
        UPDATE_FLAGS(total >= 256, cf);            \
    }

ADC(I, PC += 2, MEMORY(PC - 1))
ADC(ACC, PC += 1, ACC)
ADC(ML, PC += 1, MEMORY(L))
ADC(MHL, PC += 1, MEMORY(HL))
ADC(R0, PC += 1, R0)
ADC(R1, PC += 1, R1)
ADC(L, PC += 1, L)
ADC(H, PC += 1, H)

#define SUB(variation, pc_inc, src)  \
    INSTRUCTION(SUB_##variation) {   \
        pc_inc;                      \
        uint8_t val = src;           \
        ACC -= val;                  \
        UPDATE_ZF(ACC);              \
        UPDATE_SF(ACC);              \
        UPDATE_FLAGS(ACC > val, cf); \
    }

SUB(I, PC += 2, MEMORY(PC - 1))
SUB(ACC, PC += 1, ACC)
SUB(ML, PC += 1, MEMORY(L))
SUB(MHL, PC += 1, MEMORY(HL))
SUB(R0, PC += 1, R0)
SUB(R1, PC += 1, R1)
SUB(L, PC += 1, L)
SUB(H, PC += 1, H)

#define SBC(variation, pc_inc, src)                        \
    INSTRUCTION(SBC_##variation) {                         \
        pc_inc;                                            \
        uint16_t old_acc = ACC;                            \
        uint16_t val = (uint16_t)src - (1 - CARRY_FLAG()); \
        ACC -= (uint8_t)val;                               \
        UPDATE_ZF(ACC);                                    \
        UPDATE_SF(ACC);                                    \
        UPDATE_FLAGS(old_acc >= val, cf);                  \
    }

SBC(I, PC += 2, MEMORY(PC - 1))
SBC(ACC, PC += 1, ACC)
SBC(ML, PC += 1, MEMORY(L))
SBC(MHL, PC += 1, MEMORY(HL))
SBC(R0, PC += 1, R0)
SBC(R1, PC += 1, R1)
SBC(L, PC += 1, L)
SBC(H, PC += 1, H)

#define INC(variation, pc_inc, src) \
    INSTRUCTION(INC_##variation) {  \
        pc_inc;                     \
        src++;                      \
        UPDATE_ZF(src);             \
        UPDATE_SF(src);             \
        UPDATE_FLAGS(src == 0, cf); \
    }

INC(ACC, PC += 1, ACC)
INC(ML, PC += 1, MEMORY(L))
INC(MHL, PC += 1, MEMORY(HL))
INC(R0, PC += 1, R0)
INC(R1, PC += 1, R1)
INC(L, PC += 1, L)
INC(H, PC += 1, H)

INSTRUCTION(INC_HL) {
    PC++;
    uint16_t inc = HL + 1;
    H = (uint8_t)(inc >> 8);
    L = (uint8_t)(inc & 0xFF);
    UPDATE_ZF(inc);
    UPDATE_SF(inc);
    UPDATE_FLAGS(inc == 0, cf);
}

#define DEC(variation, pc_inc, src)         \
    INSTRUCTION(DEC_##variation) {          \
        pc_inc;                             \
        src--;                              \
        UPDATE_ZF(src);                     \
        UPDATE_SF(src);                     \
        UPDATE_FLAGS(src == UINT8_MAX, cf); \
    }

INSTRUCTION(DEC_HL) {
    PC++;
    uint16_t dec = HL - 1;
    H = (uint8_t)(dec >> 8);
    L = (uint8_t)(dec & 0xFF);
    UPDATE_ZF(dec);
    UPDATE_SF(dec);
    UPDATE_FLAGS(dec == UINT16_MAX, cf);
}

DEC(ACC, PC += 1, ACC)
DEC(ML, PC += 1, MEMORY(L))
DEC(MHL, PC += 1, MEMORY(HL))
DEC(R0, PC += 1, R0)
DEC(R1, PC += 1, R1)
DEC(L, PC += 1, L)
DEC(H, PC += 1, H)

#define NEG(variation, pc_inc, src) \
    INSTRUCTION(NEG_##variation) {  \
        pc_inc;                     \
        src = -src;                 \
        UPDATE_ZF(src);             \
        UPDATE_SF(src);             \
    }

INSTRUCTION(NEG_HL) {
    PC++;
    uint16_t neg = -HL;
    H = (uint8_t)(neg >> 8);
    L = (uint8_t)(neg & 0xFF);
    UPDATE_ZF(neg);
    UPDATE_SF(neg);
}

NEG(ACC, PC += 1, ACC)
NEG(ML, PC += 1, MEMORY(L))
NEG(MHL, PC += 1, MEMORY(HL))
NEG(R0, PC += 1, R0)
NEG(R1, PC += 1, R1)
NEG(L, PC += 1, L)
NEG(H, PC += 1, H)

#define NOT(variation, pc_inc, src) \
    INSTRUCTION(NOT_##variation) {  \
        pc_inc;                     \
        src = ~src;                 \
        UPDATE_ZF(src);             \
        UPDATE_SF(src);             \
    }

INSTRUCTION(NOT_HL) {
    PC++;
    uint16_t neg = ~HL;
    H = (uint8_t)(neg >> 8);
    L = (uint8_t)(neg & 0xFF);
    UPDATE_ZF(neg);
    UPDATE_SF(neg);
}

NOT(ACC, PC += 1, ACC)
NOT(ML, PC += 1, MEMORY(L))
NOT(MHL, PC += 1, MEMORY(HL))
NOT(R0, PC += 1, R0)
NOT(R1, PC += 1, R1)
NOT(L, PC += 1, L)
NOT(H, PC += 1, H)

#define AND(variation, pc_inc, src) \
    INSTRUCTION(AND_##variation) {  \
        pc_inc;                     \
        ACC &= src;                 \
        UPDATE_ZF(ACC);             \
        UPDATE_SF(ACC);             \
    }

AND(I, PC += 2, MEMORY(PC - 1))
AND(ACC, PC += 1, ACC)
AND(ML, PC += 1, MEMORY(L))
AND(MHL, PC += 1, MEMORY(HL))
AND(R0, PC += 1, R0)
AND(R1, PC += 1, R1)
AND(L, PC += 1, L)
AND(H, PC += 1, H)

#define OR(variation, pc_inc, src) \
    INSTRUCTION(OR_##variation) {  \
        pc_inc;                    \
        ACC |= src;                \
        UPDATE_ZF(ACC);            \
        UPDATE_SF(ACC);            \
    }

OR(I, PC += 2, MEMORY(PC - 1))
OR(ACC, PC += 1, ACC)
OR(ML, PC += 1, MEMORY(L))
OR(MHL, PC += 1, MEMORY(HL))
OR(R0, PC += 1, R0)
OR(R1, PC += 1, R1)
OR(L, PC += 1, L)
OR(H, PC += 1, H)

#define XOR(variation, pc_inc, src) \
    INSTRUCTION(XOR_##variation) {  \
        pc_inc;                     \
        ACC ^= src;                 \
        UPDATE_ZF(ACC);             \
        UPDATE_SF(ACC);             \
    }

XOR(I, PC += 2, MEMORY(PC - 1))
XOR(ACC, PC += 1, ACC)
XOR(ML, PC += 1, MEMORY(L))
XOR(MHL, PC += 1, MEMORY(HL))
XOR(R0, PC += 1, R0)
XOR(R1, PC += 1, R1)
XOR(L, PC += 1, L)
XOR(H, PC += 1, H)

#define SHL(variation, pc_inc, src) \
    INSTRUCTION(SHL_##variation) {  \
        pc_inc;                     \
        ACC <<= src;                \
        UPDATE_ZF(ACC);             \
        UPDATE_SF(ACC);             \
    }

SHL(I, PC += 2, MEMORY(PC - 1))
SHL(ML, PC += 1, MEMORY(L))
SHL(MHL, PC += 1, MEMORY(HL))
SHL(R0, PC += 1, R0)
SHL(R1, PC += 1, R1)
SHL(L, PC += 1, L)
SHL(H, PC += 1, H)

#define SHR(variation, pc_inc, src) \
    INSTRUCTION(SHR_##variation) {  \
        pc_inc;                     \
        ACC >>= src;                \
        UPDATE_ZF(ACC);             \
        UPDATE_SF(ACC);             \
    }

SHR(I, PC += 2, MEMORY(PC - 1))
SHR(ML, PC += 1, MEMORY(L))
SHR(MHL, PC += 1, MEMORY(HL))
SHR(R0, PC += 1, R0)
SHR(R1, PC += 1, R1)
SHR(L, PC += 1, L)
SHR(H, PC += 1, H)

#define ROL(variation, pc_inc, src)                          \
    INSTRUCTION(ROL_##variation) {                           \
        pc_inc;                                              \
        const uint8_t mask = CHAR_BIT * sizeof(uint8_t) - 1; \
        const uint8_t rot = src;                             \
        ACC = (ACC << rot) | (ACC >> (-rot & mask));         \
        UPDATE_ZF(ACC);                                      \
        UPDATE_SF(ACC);                                      \
    }

ROL(I, PC += 2, MEMORY(PC - 1))
ROL(ML, PC += 1, MEMORY(L))
ROL(MHL, PC += 1, MEMORY(HL))
ROL(R0, PC += 1, R0)
ROL(R1, PC += 1, R1)
ROL(L, PC += 1, L)
ROL(H, PC += 1, H)

#define ROR(variation, pc_inc, src)                          \
    INSTRUCTION(ROR_##variation) {                           \
        pc_inc;                                              \
        const uint8_t mask = CHAR_BIT * sizeof(uint8_t) - 1; \
        const uint8_t rot = src;                             \
        ACC = (ACC >> rot) | (ACC << (-rot & mask));         \
        UPDATE_ZF(ACC);                                      \
        UPDATE_SF(ACC);                                      \
    }

ROR(I, PC += 2, MEMORY(PC - 1))
ROR(ML, PC += 1, MEMORY(L))
ROR(MHL, PC += 1, MEMORY(HL))
ROR(R0, PC += 1, R0)
ROR(R1, PC += 1, R1)
ROR(L, PC += 1, L)
ROR(H, PC += 1, H)

/* #define SWAP(variation, src)                         \
//     INSTRUCTION(SWAP_##variation) {                  \
//         PC++;                                        \
//         uint8_t val = src;                           \
//         src = (val & 0x0F) << 4 | (val & 0xF0) >> 4; \
//         UPDATE_ZF(src);                              \
//         UPDATE_SF(src);                              \
//     }

// SWAP(ACC, ACC)
// SWAP(ML, MEMORY(L))
// SWAP(MHL, MEMORY(HL))
// SWAP(R0, R0)
// SWAP(R1, R1)
// SWAP(L, L)
// SWAP(H, H) */

#define OPW(opname, op, pc_inc, variation, src)    \
    INSTRUCTION(opname##_##variation) {            \
        pc_inc;                                    \
        const uint16_t val = HL op(uint16_t)(src); \
        H = (uint8_t)(val >> 8);                   \
        L = (uint8_t)(val & 0xFF);                 \
        UPDATE_FLAGS(H == 0 && L == 0, zf);        \
        UPDATE_FLAGS(H &SIGN_BIT, sf);             \
    }

OPW(ADDW, +, PC += 3, I, ((uint16_t)MEMORY(PC - 2) << 8) | (uint16_t)MEMORY(PC - 1))
OPW(ADDW, +, PC += 1, ACC, ACC)
OPW(ADDW, +, PC += 1, R0, R0)
OPW(ADDW, +, PC += 1, R1, R1)

OPW(SUBW, -, PC += 3, I, ((uint16_t)MEMORY(PC - 2) << 8) | (uint16_t)MEMORY(PC - 1))
OPW(SUBW, -, PC += 1, ACC, ACC)
OPW(SUBW, -, PC += 1, R0, R0)
OPW(SUBW, -, PC += 1, R1, R1)

OPW(MULW, *, PC += 3, I, ((uint16_t)MEMORY(PC - 2) << 8) | (uint16_t)MEMORY(PC - 1))
OPW(MULW, *, PC += 1, ACC, ACC)
OPW(MULW, *, PC += 1, R0, R0)
OPW(MULW, *, PC += 1, R1, R1)

OPW(DIVW, /, PC += 3, I, ((uint16_t)MEMORY(PC - 2) << 8) | (uint16_t)MEMORY(PC - 1))
OPW(DIVW, /, PC += 1, ACC, ACC)
OPW(DIVW, /, PC += 1, R0, R0)
OPW(DIVW, /, PC += 1, R1, R1)

INSTRUCTION(JMP) { PC = ((uint16_t)MEMORY(PC + 1) << 8) | ((uint16_t)MEMORY(PC + 2)); }
INSTRUCTION(JS) {
    PC += 3;
    if (get_sf(FLAGS)) {
        PC = ((uint16_t)MEMORY(PC - 2) << 8) | ((uint16_t)MEMORY(PC - 1));
    }
}
INSTRUCTION(JNS) {
    PC += 3;
    if (!get_sf(FLAGS)) {
        PC = ((uint16_t)MEMORY(PC - 2) << 8) | ((uint16_t)MEMORY(PC - 1));
    }
}
INSTRUCTION(JZ) {
    PC += 3;
    if (get_zf(FLAGS)) {
        PC = ((uint16_t)MEMORY(PC - 2) << 8) | ((uint16_t)MEMORY(PC - 1));
    }
}
INSTRUCTION(JNZ) {
    PC += 3;
    if (!get_zf(FLAGS)) {
        PC = ((uint16_t)MEMORY(PC - 2) << 8) | ((uint16_t)MEMORY(PC - 1));
    }
}
INSTRUCTION(JC) {
    PC += 3;
    if (get_cf(FLAGS)) {
        PC = ((uint16_t)MEMORY(PC - 2) << 8) | (uint16_t)MEMORY(PC - 1);
    }
}
INSTRUCTION(JNC) {
    PC += 3;
    if (!get_cf(FLAGS)) {
        PC = ((uint16_t)MEMORY(PC - 2) << 8) | ((uint16_t)MEMORY(PC - 1));
    }
}
INSTRUCTION(JEXT) {
    PC++;
    uint8_t jext = MEMORY(PC);
    uint8_t flags = FLAGS & 0x3f;
    uint8_t mask = jext & 0x3f;
    switch ((jext >> 6) & 0x3) {
        // jump if any specified flag is 1
        case 0:
            if ((flags & mask) != 0) JMP(cpu);
            break;
        // jump if all specified flags are 1
        case 1:
            if ((flags & mask) == mask) JMP(cpu);
            break;
        // jump if all specified flags are 0
        case 2:
            if ((flags & mask) == 0) JMP(cpu);
            break;
        // jump if any specified flag is 0
        case 3:
            if ((flags & mask) != mask) JMP(cpu);
            break;
    }
}

#define CMP(variation, pc_inc, src) \
    INSTRUCTION(CMP_##variation) {  \
        pc_inc;                     \
        uint8_t val = ACC - src;    \
        UPDATE_ZF(val);             \
        UPDATE_SF(val);             \
    }

CMP(I, PC += 2, MEMORY(PC - 1))
CMP(ACC, PC += 1, ACC)
CMP(ML, PC += 1, MEMORY(L))
CMP(MHL, PC += 1, MEMORY(HL))
CMP(R0, PC += 1, R0)
CMP(R1, PC += 1, R1)
CMP(L, PC += 1, L)
CMP(H, PC += 1, H)

#define PUSH(variation, src)        \
    INSTRUCTION(PUSH_##variation) { \
        PC++;                       \
        uint8_t val = src;          \
        STACK(SP++) = val;          \
        UPDATE_ZF(val);             \
        UPDATE_SF(val);             \
    }

PUSH(I, MEMORY(PC++))
PUSH(ACC, ACC)
PUSH(R0, R0)
PUSH(R1, R1)
PUSH(L, L)
PUSH(H, H)
PUSH(BP, BP)
PUSH(FLAGS, FLAGS)

#define POP(variation, pc_inc, dest) \
    INSTRUCTION(POP_##variation) {   \
        pc_inc;                      \
        uint8_t val = STACK(--SP);   \
        dest = val;                  \
        UPDATE_ZF(val);              \
        UPDATE_SF(val);              \
    }

POP(ACC, PC += 1, ACC)
POP(IM, PC += 3, MEMORY(((uint16_t)MEMORY(PC - 2) << 8) | (uint16_t)MEMORY(PC - 1)))
POP(R0, PC += 1, R0)
POP(R1, PC += 1, R1)
POP(L, PC += 1, L)
POP(H, PC += 1, H)
POP(BP, PC += 1, BP)
INSTRUCTION(POP_FLAGS) {
    PC += 1;
    uint8_t val = STACK(--SP);
    FLAGS = val;
}

INSTRUCTION(CALL) {
    PC += 3;
    uint16_t addr = ((uint16_t)MEMORY(PC - 2) << 8) | (uint16_t)MEMORY(PC - 1);
    STACK(SP++) = (uint8_t)(PC >> 8);
    STACK(SP++) = (uint8_t)(PC);
    PC = addr;
}
INSTRUCTION(RET) {
    uint16_t low = (uint16_t)STACK(--SP);
    uint16_t high = (uint16_t)STACK(--SP);
    PC = (high << 8) | low;
}
INSTRUCTION(RET_I) {
    uint8_t to_pop = MEMORY(PC + 1);
    uint16_t low = (uint16_t)STACK(--SP);
    uint16_t high = (uint16_t)STACK(--SP);
    PC = (high << 8) | low;
    SP -= to_pop;
}
INSTRUCTION(ENTER) {
    PC += 2;
    STACK(SP++) = BP;
    BP = SP;
    SP += MEMORY(PC - 1);  // space for local variables
}
INSTRUCTION(LEAVE) {
    PC++;
    SP = BP;
    BP = STACK(--SP);
}
INSTRUCTION(ADD_L_I) {
    PC += 2;
    L += MEMORY(PC - 1);
}
INSTRUCTION(ADD_HL_I) {
    PC += 3;
    uint16_t val = HL + (((uint16_t)(MEMORY(PC - 2)) << 8) | ((uint16_t)MEMORY(PC - 1)));
    H = (uint8_t)((val >> 8) & 0xFF);
    L = (uint8_t)(val & 0xFF);
}
INSTRUCTION(LOAD_L_I) {
    PC += 2;
    L = MEMORY(PC - 1);
}
INSTRUCTION(LOAD_HL_I) {
    PC += 3;
    H = (uint8_t)MEMORY(PC - 2);
    L = (uint8_t)MEMORY(PC - 1);
}

#define MIN(variation, pc_inc, src) \
    INSTRUCTION(MIN_##variation) {  \
        pc_inc;                     \
        if (src < ACC) {            \
            ACC = src;              \
        }                           \
        UPDATE_ZF(ACC);             \
        UPDATE_SF(ACC);             \
    }

MIN(I, PC += 2, MEMORY(PC - 2))
MIN(ML, PC += 1, MEMORY(L))
MIN(MHL, PC += 1, MEMORY(HL))
MIN(R0, PC += 1, R0)
MIN(R1, PC += 1, R1)
MIN(L, PC += 1, L)
MIN(H, PC += 1, H)

#define MAX(variation, pc_inc, src) \
    INSTRUCTION(MAX_##variation) {  \
        pc_inc;                     \
        if (src > ACC) {            \
            ACC = src;              \
        }                           \
        UPDATE_ZF(ACC);             \
        UPDATE_SF(ACC);             \
    }

MAX(I, PC += 2, MEMORY(PC - 1))
MAX(ML, PC += 1, MEMORY(L))
MAX(MHL, PC += 1, MEMORY(HL))
MAX(R0, PC += 1, R0)
MAX(R1, PC += 1, R1)
MAX(L, PC += 1, L)
MAX(H, PC += 1, H)

LOAD(BPI, PC += 2, STACK((uint8_t)(BP - MEMORY(PC - 1))))
STORE(BPI, PC += 2, STACK((uint8_t)(BP - MEMORY(PC - 1))))
MIN(BPI, PC += 2, STACK((uint8_t)(BP - MEMORY(PC - 1))))
MAX(BPI, PC += 2, STACK((uint8_t)(BP - MEMORY(PC - 1))))
CMP(BPI, PC += 2, STACK((uint8_t)(BP - MEMORY(PC - 1))))
XCH(BPI, PC += 2, STACK((uint8_t)(BP - MEMORY(PC - 1))))
ADD(BPI, PC += 2, STACK((uint8_t)(BP - MEMORY(PC - 1))))
ADC(BPI, PC += 2, STACK((uint8_t)(BP - MEMORY(PC - 1))))
SUB(BPI, PC += 2, STACK((uint8_t)(BP - MEMORY(PC - 1))))
SBC(BPI, PC += 2, STACK((uint8_t)(BP - MEMORY(PC - 1))))
INC(BPI, PC += 2, STACK((uint8_t)(BP - MEMORY(PC - 1))))
DEC(BPI, PC += 2, STACK((uint8_t)(BP - MEMORY(PC - 1))))
NEG(BPI, PC += 2, STACK((uint8_t)(BP - MEMORY(PC - 1))))
NOT(BPI, PC += 2, STACK((uint8_t)(BP - MEMORY(PC - 1))))
AND(BPI, PC += 2, STACK((uint8_t)(BP - MEMORY(PC - 1))))
OR(BPI, PC += 2, STACK((uint8_t)(BP - MEMORY(PC - 1))))
XOR(BPI, PC += 2, STACK((uint8_t)(BP - MEMORY(PC - 1))))
SHL(BPI, PC += 2, STACK((uint8_t)(BP - MEMORY(PC - 1))))
SHR(BPI, PC += 2, STACK((uint8_t)(BP - MEMORY(PC - 1))))
ROL(BPI, PC += 2, STACK((uint8_t)(BP - MEMORY(PC - 1))))
ROR(BPI, PC += 2, STACK((uint8_t)(BP - MEMORY(PC - 1))))

#define unused NOOP

// clang-format off

/// Table containing function pointers to all 256 different operations
static const Instruction OP_TABLE[256] = {
    NOOP,       HALT,       EI,         DI,         ET,         DT,         CLRA,       RESET, 
    LOAD_I,     LOAD_IM,    LOAD_ML,    LOAD_MHL,   LOAD_R0,    LOAD_R1,    LOAD_L,     LOAD_H,
    LOAD_L_I,   STORE_IM,   STORE_ML,   STORE_MHL,  STORE_R0,   STORE_R1,   STORE_L,    STORE_H,
    LOAD_HL_I,  XCH_IM,     XCH_ML,     XCH_MHL,    XCH_R0,     XCH_R1,     XCH_L,      XCH_H,
    ADD_I,      ADD_ACC,    ADD_ML,     ADD_MHL,    ADD_R0,     ADD_R1,     ADD_L,      ADD_H,
    ADC_I,      ADC_ACC,    ADC_ML,     ADC_MHL,    ADC_R0,     ADC_R1,     ADC_L,      ADC_H,
    SUB_I,      SUB_ACC,    SUB_ML,     SUB_MHL,    SUB_R0,     SUB_R1,     SUB_L,      SUB_H,
    SBC_I,      SBC_ACC,    SBC_ML,     SBC_MHL,    SBC_R0,     SBC_R1,     SBC_L,      SBC_H,
    INC_HL,     INC_ACC,    INC_ML,     INC_MHL,    INC_R0,     INC_R1,     INC_L,      INC_H,
    DEC_HL,     DEC_ACC,    DEC_ML,     DEC_MHL,    DEC_R0,     DEC_R1,     DEC_L,      DEC_H,
    NEG_HL,     NEG_ACC,    NEG_ML,     NEG_MHL,    NEG_R0,     NEG_R1,     NEG_L,      NEG_H,
    NOT_HL,     NOT_ACC,    NOT_ML,     NOT_MHL,    NOT_R0,     NOT_R1,     NOT_L,      NOT_H, 
    AND_I,      AND_ACC,    AND_ML,     AND_MHL,    AND_R0,     AND_R1,     AND_L,      AND_H,
    OR_I,       OR_ACC,     OR_ML,      OR_MHL,     OR_R0,      OR_R1,      OR_L,       OR_H, 
    XOR_I,      XOR_ACC,    XOR_ML,     XOR_MHL,    XOR_R0,     XOR_R1,     XOR_L,      XOR_H, 
    SHL_I,      MIN_BPI,     SHL_ML,     SHL_MHL,    SHL_R0,     SHL_R1,     SHL_L,      SHL_H,
    SHR_I,      MAX_BPI,     SHR_ML,     SHR_MHL,    SHR_R0,     SHR_R1,     SHR_L,      SHR_H,
    ROL_I,      unused,     ROL_ML,     ROL_MHL,    ROL_R0,     ROL_R1,     ROL_L,      ROL_H,
    ROR_I,      unused,     ROR_ML,     ROR_MHL,    ROR_R0,     ROR_R1,     ROR_L,      ROR_H,
    ADDW_I,     ADDW_ACC,   ADDW_R0,    ADDW_R1,    SUBW_I,     SUBW_ACC,   SUBW_R0,    SUBW_R1,
    MULW_I,     MULW_ACC,   MULW_R0,    MULW_R1,    DIVW_I,     DIVW_ACC,   DIVW_R0,    DIVW_R1,
    JMP,        JS,         JNS,        JZ,         JNZ,        JC,         JNC,        JEXT,
    CMP_I,      CMP_ACC,    CMP_ML,     CMP_MHL,    CMP_R0,     CMP_R1,     CMP_L,      CMP_H,
    PUSH_I,     PUSH_ACC,   PUSH_R0,    PUSH_R1,    PUSH_L,     PUSH_H,     PUSH_BP,    PUSH_FLAGS,
    POP_IM,     POP_ACC,    POP_R0,     POP_R1,     POP_L,      POP_H,      POP_BP,     POP_FLAGS,
    CALL,       RET,        ENTER,      LEAVE,      LOAD_BPI,   STORE_BPI,  ADD_L_I,    ADD_HL_I,
    MIN_I,      RET_I,      MIN_ML,     MIN_MHL,    MIN_R0,     MIN_R1,     MIN_L,      MIN_H, 
    MAX_I,      CMP_BPI,    MAX_ML,     MAX_MHL,    MAX_R0,     MAX_R1,     MAX_L,      MAX_H, 
    XCH_BPI,    ADD_BPI,    ADC_BPI,    SUB_BPI,    SBC_BPI,    INC_BPI,    DEC_BPI,    NEG_BPI,
    NOT_BPI,    AND_BPI,    OR_BPI,     XOR_BPI,    SHL_BPI,    SHR_BPI,    ROL_BPI,    ROR_BPI, 
    unused,     unused,     unused,     unused,     unused,     unused,     unused,     unused, 
    unused,     unused,     unused,     unused,     unused,     unused,     unused,     unused,
};

typedef enum Opcode {
    OP_NOOP         = 0,
    OP_HALT         = 1,
    OP_EI           = 2,
    OP_DI           = 3,
    OP_ET           = 4,
    OP_DT           = 5,
    OP_CLRA         = 6,
    OP_RESET        = 7,
    OP_LOAD_I       = 8,
    OP_LOAD_IM      = 9,
    OP_LOAD_ML      = 10,
    OP_LOAD_MHL     = 11,
    OP_LOAD_R0      = 12,
    OP_LOAD_R1      = 13,
    OP_LOAD_L       = 14,
    OP_LOAD_H       = 15,
    OP_LOAD_L_I     = 16,
    OP_STORE_IM     = 17,
    OP_STORE_ML     = 18,
    OP_STORE_MHL    = 19,
    OP_STORE_R0     = 20,
    OP_STORE_R1     = 21,
    OP_STORE_L      = 22,
    OP_STORE_H      = 23,
    OP_LOAD_HL_I    = 24,
    OP_XCH_IM       = 25,
    OP_XCH_ML       = 26,
    OP_XCH_MHL      = 27,
    OP_XCH_R0       = 28,
    OP_XCH_R1       = 29,
    OP_XCH_L        = 30,
    OP_XCH_H        = 31,
    OP_ADD_I        = 32,
    OP_ADD_ACC      = 33,
    OP_ADD_ML       = 34,
    OP_ADD_MHL      = 35,
    OP_ADD_R0       = 36,
    OP_ADD_R1       = 37,
    OP_ADD_L        = 38,
    OP_ADD_H        = 39,
    OP_ADC_I        = 40,
    OP_ADC_ACC      = 41,
    OP_ADC_ML       = 42,
    OP_ADC_MHL      = 43,
    OP_ADC_R0       = 44,
    OP_ADC_R1       = 45,
    OP_ADC_L        = 46,
    OP_ADC_H        = 47,
    OP_SUB_I        = 48,
    OP_SUB_ACC      = 49,
    OP_SUB_ML       = 50,
    OP_SUB_MHL      = 51,
    OP_SUB_R0       = 52,
    OP_SUB_R1       = 53,
    OP_SUB_L        = 54,
    OP_SUB_H        = 55,
    OP_SBC_I        = 56,
    OP_SBC_ACC      = 57,
    OP_SBC_ML       = 58,
    OP_SBC_MHL      = 59,
    OP_SBC_R0       = 60,
    OP_SBC_R1       = 61,
    OP_SBC_L        = 62,
    OP_SBC_H        = 63,
    OP_INC_HL       = 64,
    OP_INC_ACC      = 65,
    OP_INC_ML       = 66,
    OP_INC_MHL      = 67,
    OP_INC_R0       = 68,
    OP_INC_R1       = 69,
    OP_INC_L        = 70,
    OP_INC_H        = 71,
    OP_DEC_HL       = 72,
    OP_DEC_ACC      = 73,
    OP_DEC_ML       = 74,
    OP_DEC_MHL      = 75,
    OP_DEC_R0       = 76,
    OP_DEC_R1       = 77,
    OP_DEC_L        = 78,
    OP_DEC_H        = 79,
    OP_NEG_HL       = 80,
    OP_NEG_ACC      = 81,
    OP_NEG_ML       = 82,
    OP_NEG_MHL      = 83,
    OP_NEG_R0       = 84,
    OP_NEG_R1       = 85,
    OP_NEG_L        = 86,
    OP_NEG_H        = 87,
    OP_NOT_HL       = 88,
    OP_NOT_ACC      = 89,
    OP_NOT_ML       = 90,
    OP_NOT_MHL      = 91,
    OP_NOT_R0       = 92,
    OP_NOT_R1       = 93,
    OP_NOT_L        = 94,
    OP_NOT_H        = 95,
    OP_AND_I        = 96,
    OP_AND_ACC      = 97,
    OP_AND_ML       = 98,
    OP_AND_MHL      = 99,
    OP_AND_R0       = 100,
    OP_AND_R1       = 101,
    OP_AND_L        = 102,
    OP_AND_H        = 103,
    OP_OR_I         = 104,
    OP_OR_ACC       = 105,
    OP_OR_ML        = 106,
    OP_OR_MHL       = 107,
    OP_OR_R0        = 108,
    OP_OR_R1        = 109,
    OP_OR_L         = 110,
    OP_OR_H         = 111,
    OP_XOR_I        = 112,
    OP_XOR_ACC      = 113,
    OP_XOR_ML       = 114,
    OP_XOR_MHL      = 115,
    OP_XOR_R0       = 116,
    OP_XOR_R1       = 117,
    OP_XOR_L        = 118,
    OP_XOR_H        = 119,
    OP_SHL_I        = 120,
    OP_MIN_BPI      = 121,
    OP_SHL_ML       = 122,
    OP_SHL_MHL      = 123,
    OP_SHL_R0       = 124,
    OP_SHL_R1       = 125,
    OP_SHL_L        = 126,
    OP_SHL_H        = 127,
    OP_SHR_I        = 128,
    OP_MAX_BPI      = 129,
    OP_SHR_ML       = 130,
    OP_SHR_MHL      = 131,
    OP_SHR_R0       = 132,
    OP_SHR_R1       = 133,
    OP_SHR_L        = 134,
    OP_SHR_H        = 135,
    OP_ROL_I        = 136,
    // OP_unused    = 137,
    OP_ROL_ML       = 138,
    OP_ROL_MHL      = 139,
    OP_ROL_R0       = 140,
    OP_ROL_R1       = 141,
    OP_ROL_L        = 142,
    OP_ROL_H        = 143,
    OP_ROR_I        = 144,
    // OP_unused    = 145,
    OP_ROR_ML       = 146,
    OP_ROR_MHL      = 147,
    OP_ROR_R0       = 148,
    OP_ROR_R1       = 149,
    OP_ROR_L        = 150,
    OP_ROR_H        = 151,
    OP_ADDW_I       = 152,
    OP_ADDW_ACC     = 153,
    OP_ADDW_R0      = 154,
    OP_ADDW_R1      = 155,
    OP_SUBW_I       = 156,
    OP_SUBW_ACC     = 157,
    OP_SUBW_R0      = 158,
    OP_SUBW_R1      = 159,
    OP_MULW_I       = 160,
    OP_MULW_ACC     = 161,
    OP_MULW_R0      = 162,
    OP_MULW_R1      = 163,
    OP_DIVW_I       = 164,
    OP_DIVW_ACC     = 165,
    OP_DIVW_R0      = 166,
    OP_DIVW_R1      = 167,
    OP_JMP          = 168,
    OP_JS           = 169,
    OP_JNS          = 170,
    OP_JZ           = 171,
    OP_JNZ          = 172,
    OP_JC           = 173,
    OP_JNC          = 174,
    OP_JEXT         = 175,
    OP_CMP_I        = 176,
    OP_CMP_ACC      = 177,
    OP_CMP_ML       = 178,
    OP_CMP_MHL      = 179,
    OP_CMP_R0       = 180,
    OP_CMP_R1       = 181,
    OP_CMP_L        = 182,
    OP_CMP_H        = 183,
    OP_PUSH_I       = 184,
    OP_PUSH_ACC     = 185,
    OP_PUSH_R0      = 186,
    OP_PUSH_R1      = 187,
    OP_PUSH_L       = 188,
    OP_PUSH_H       = 189,
    OP_PUSH_BP      = 190,
    OP_PUSH_FLAGS   = 191,
    OP_POP_IM       = 192,
    OP_POP_ACC      = 193,
    OP_POP_R0       = 194,
    OP_POP_R1       = 195,
    OP_POP_L        = 196,
    OP_POP_H        = 197,
    OP_POP_BP       = 198,
    OP_POP_FLAGS    = 199,
    OP_CALL         = 200,
    OP_RET          = 201,
    OP_ENTER        = 202,
    OP_LEAVE        = 203,
    OP_LOAD_BPI     = 204,
    OP_STORE_BPI    = 205,
    OP_ADD_L_I      = 206,
    OP_ADD_HL_I     = 207,
    OP_MIN_I        = 208,
    OP_RET_I        = 209,
    OP_MIN_ML       = 210,
    OP_MIN_MHL      = 211,
    OP_MIN_R0       = 212,
    OP_MIN_R1       = 213,
    OP_MIN_L        = 214,
    OP_MIN_H        = 215,
    OP_MAX_I        = 216,
    OP_CMP_BPI      = 217,
    OP_MAX_ML       = 218,
    OP_MAX_MHL      = 219,
    OP_MAX_R0       = 220,
    OP_MAX_R1       = 221,
    OP_MAX_L        = 222,
    OP_MAX_H        = 223,
    OP_XCH_BPI      = 224,
    OP_ADD_BPI      = 225,
    OP_ADC_BPI      = 226,
    OP_SUB_BPI      = 227,
    OP_SBC_BPI      = 228,
    OP_INC_BPI      = 229,
    OP_DEC_BPI      = 230,
    OP_NEG_BPI      = 231,
    OP_NOT_BPI      = 232,
    OP_AND_BPI      = 233,
    OP_OR_BPI       = 234,
    OP_XOR_BPI      = 235,
    OP_SHL_BPI      = 236,
    OP_SHR_BPI      = 237,
    OP_ROL_BPI      = 238,
    OP_ROR_BPI      = 239,
    // OP_unused    = 240,
    // OP_unused    = 241,
    // OP_unused    = 242,
    // OP_unused    = 243,
    // OP_unused    = 244,
    // OP_unused    = 245,
    // OP_unused    = 246,
    // OP_unused    = 247,
    // OP_unused    = 248,
    // OP_unused    = 249,
    // OP_unused    = 250,
    // OP_unused    = 251,
    // OP_unused    = 252,
    // OP_unused    = 253,
    // OP_unused    = 254,
    // OP_unused    = 255,
} Opcode;

// clang-format on

#endif
