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
INSTRUCTION(HALT) {
    UNUSED(cpu);
}
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
        dest = ACC;                    \
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

#define ADD(variation, src)          \
    INSTRUCTION(ADD_##variation) {   \
        PC++;                        \
        uint8_t val = src;           \
        ACC += val;                  \
        UPDATE_ZF(ACC);              \
        UPDATE_SF(ACC);              \
        UPDATE_FLAGS(ACC < val, cf); \
    }

ADD(I, MEMORY(PC++))
ADD(ACC, ACC)
ADD(ML, MEMORY(L))
ADD(MHL, MEMORY(HL))
ADD(R0, R0)
ADD(R1, R1)
ADD(L, L)
ADD(H, H)

#define ADC(variation, src)               \
    INSTRUCTION(ADC_##variation) {        \
        PC++;                             \
        uint8_t val = src + CARRY_FLAG(); \
        ACC += val;                       \
        UPDATE_ZF(ACC);                   \
        UPDATE_SF(ACC);                   \
        UPDATE_FLAGS(ACC < val, cf);      \
    }

ADC(I, MEMORY(PC++))
ADC(ACC, ACC)
ADC(ML, MEMORY(L))
ADC(MHL, MEMORY(HL))
ADC(R0, R0)
ADC(R1, R1)
ADC(L, L)
ADC(H, H)

#define SUB(variation, src)          \
    INSTRUCTION(SUB_##variation) {   \
        PC++;                        \
        uint8_t val = src;           \
        ACC -= val;                  \
        UPDATE_ZF(ACC);              \
        UPDATE_SF(ACC);              \
        UPDATE_FLAGS(ACC > val, cf); \
    }

SUB(I, MEMORY(PC++))
SUB(ACC, ACC)
SUB(ML, MEMORY(L))
SUB(MHL, MEMORY(HL))
SUB(R0, R0)
SUB(R1, R1)
SUB(L, L)
SUB(H, H)

#define SBC(variation, src)                     \
    INSTRUCTION(SBC_##variation) {              \
        PC++;                                   \
        uint8_t val = src - (1 - CARRY_FLAG()); \
        ACC -= val;                             \
        UPDATE_ZF(ACC);                         \
        UPDATE_SF(ACC);                         \
        UPDATE_FLAGS(ACC > val, cf);            \
    }

SBC(I, MEMORY(PC++))
SBC(ACC, ACC)
SBC(ML, MEMORY(L))
SBC(MHL, MEMORY(HL))
SBC(R0, R0)
SBC(R1, R1)
SBC(L, L)
SBC(H, H)

#define INC(variation, src)         \
    INSTRUCTION(INC_##variation) {  \
        PC++;                       \
        src++;                      \
        UPDATE_ZF(src);             \
        UPDATE_SF(src);             \
        UPDATE_FLAGS(src == 0, cf); \
    }

INC(ACC, ACC)
INC(ML, MEMORY(L))
INC(MHL, MEMORY(HL))
INC(R0, R0)
INC(R1, R1)
INC(L, L)
INC(H, H)

INSTRUCTION(INC_HL) {
    PC++;
    uint16_t inc = HL + 1;
    H = (uint8_t)(inc >> 8);
    L = (uint8_t)(inc & 0xFF);
    UPDATE_ZF(inc);
    UPDATE_SF(inc);
    UPDATE_FLAGS(inc == 0, cf);
}

#define DEC(variation, src)                 \
    INSTRUCTION(DEC_##variation) {          \
        PC++;                               \
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

DEC(ACC, ACC)
DEC(ML, MEMORY(L))
DEC(MHL, MEMORY(HL))
DEC(R0, R0)
DEC(R1, R1)
DEC(L, L)
DEC(H, H)

#define NEG(variation, src)        \
    INSTRUCTION(NEG_##variation) { \
        PC++;                      \
        src = -src;                \
        UPDATE_ZF(src);            \
        UPDATE_SF(src);            \
    }

INSTRUCTION(NEG_HL) {
    PC++;
    uint16_t neg = -HL;
    H = (uint8_t)(neg >> 8);
    L = (uint8_t)(neg & 0xFF);
    UPDATE_ZF(neg);
    UPDATE_SF(neg);
}

NEG(ACC, ACC)
NEG(ML, MEMORY(L))
NEG(MHL, MEMORY(HL))
NEG(R0, R0)
NEG(R1, R1)
NEG(L, L)
NEG(H, H)

#define NOT(variation, src)        \
    INSTRUCTION(NOT_##variation) { \
        PC++;                      \
        src = ~src;                \
        UPDATE_ZF(src);            \
        UPDATE_SF(src);            \
    }

INSTRUCTION(NOT_HL) {
    PC++;
    uint16_t neg = ~HL;
    H = (uint8_t)(neg >> 8);
    L = (uint8_t)(neg & 0xFF);
    UPDATE_ZF(neg);
    UPDATE_SF(neg);
}

NOT(ACC, ACC)
NOT(ML, MEMORY(L))
NOT(MHL, MEMORY(HL))
NOT(R0, R0)
NOT(R1, R1)
NOT(L, L)
NOT(H, H)

#define AND(variation, src)        \
    INSTRUCTION(AND_##variation) { \
        PC++;                      \
        ACC &= src;                \
        UPDATE_ZF(ACC);            \
        UPDATE_SF(ACC);            \
    }

AND(I, MEMORY(PC++))
AND(ACC, ACC)
AND(ML, MEMORY(L))
AND(MHL, MEMORY(HL))
AND(R0, R0)
AND(R1, R1)
AND(L, L)
AND(H, H)

#define OR(variation, src)        \
    INSTRUCTION(OR_##variation) { \
        PC++;                     \
        ACC |= src;               \
        UPDATE_ZF(ACC);           \
        UPDATE_SF(ACC);           \
    }

OR(I, MEMORY(PC++))
OR(ACC, ACC)
OR(ML, MEMORY(L))
OR(MHL, MEMORY(HL))
OR(R0, R0)
OR(R1, R1)
OR(L, L)
OR(H, H)

#define XOR(variation, src)        \
    INSTRUCTION(XOR_##variation) { \
        PC++;                      \
        ACC ^= src;                \
        UPDATE_ZF(ACC);            \
        UPDATE_SF(ACC);            \
    }

XOR(I, MEMORY(PC++))
XOR(ACC, ACC)
XOR(ML, MEMORY(L))
XOR(MHL, MEMORY(HL))
XOR(R0, R0)
XOR(R1, R1)
XOR(L, L)
XOR(H, H)

#define SHL(variation, src)        \
    INSTRUCTION(SHL_##variation) { \
        PC++;                      \
        ACC <<= src;               \
        UPDATE_ZF(ACC);            \
        UPDATE_SF(ACC);            \
    }

SHL(I, MEMORY(PC++))
SHL(ML, MEMORY(L))
SHL(MHL, MEMORY(HL))
SHL(R0, R0)
SHL(R1, R1)
SHL(L, L)
SHL(H, H)

#define SHR(variation, src)        \
    INSTRUCTION(SHR_##variation) { \
        PC++;                      \
        ACC >>= src;               \
        UPDATE_ZF(ACC);            \
        UPDATE_SF(ACC);            \
    }

SHR(I, MEMORY(PC++))
SHR(ML, MEMORY(L))
SHR(MHL, MEMORY(HL))
SHR(R0, R0)
SHR(R1, R1)
SHR(L, L)
SHR(H, H)

#define ROL(variation, src)                                  \
    INSTRUCTION(ROL_##variation) {                           \
        PC++;                                                \
        const uint8_t mask = CHAR_BIT * sizeof(uint8_t) - 1; \
        const uint8_t rot = src;                             \
        ACC = (ACC << rot) | (ACC >> (-rot & mask));         \
        UPDATE_ZF(ACC);                                      \
        UPDATE_SF(ACC);                                      \
    }

ROL(I, MEMORY(PC++))
ROL(ML, MEMORY(L))
ROL(MHL, MEMORY(HL))
ROL(R0, R0)
ROL(R1, R1)
ROL(L, L)
ROL(H, H)

#define ROR(variation, src)                                  \
    INSTRUCTION(ROR_##variation) {                           \
        PC++;                                                \
        const uint8_t mask = CHAR_BIT * sizeof(uint8_t) - 1; \
        const uint8_t rot = src;                             \
        ACC = (ACC >> rot) | (ACC << (-rot & mask));         \
        UPDATE_ZF(ACC);                                      \
        UPDATE_SF(ACC);                                      \
    }

ROR(I, MEMORY(PC++))
ROR(ML, MEMORY(L))
ROR(MHL, MEMORY(HL))
ROR(R0, R0)
ROR(R1, R1)
ROR(L, L)
ROR(H, H)

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

INSTRUCTION(JMP) {
    PC += 3;
    PC = ((uint16_t)MEMORY(PC - 2) << 8) | ((uint16_t)MEMORY(PC - 1));
}
INSTRUCTION(JIHL) {
    PC += 3;
    PC = (((uint16_t)MEMORY(PC - 2) << 8) | ((uint16_t)MEMORY(PC - 1))) + HL;
}
INSTRUCTION(JHL) { PC = HL; }
INSTRUCTION(JZ) {
    PC += 3;
    if (get_zf(FLAGS)) PC = ((uint16_t)MEMORY(PC - 2) << 8) | ((uint16_t)MEMORY(PC - 1));
}
INSTRUCTION(JNZ) {
    PC += 3;
    if (!get_zf(FLAGS)) {
        PC = ((uint16_t)MEMORY(PC - 2) << 8) | ((uint16_t)MEMORY(PC - 1));
    }
}
INSTRUCTION(JC) {
    PC += 3;
    if (get_cf(FLAGS)) PC = ((uint16_t)MEMORY(PC - 2) << 8) | (uint16_t)MEMORY(PC - 1);
}
INSTRUCTION(JNC) {
    PC += 3;
    if (!get_cf(FLAGS)) PC = ((uint16_t)MEMORY(PC - 2) << 8) | ((uint16_t)MEMORY(PC - 1));
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

#define CMP(variation, src)        \
    INSTRUCTION(CMP_##variation) { \
        PC++;                      \
        uint8_t val = ACC - src;   \
        UPDATE_ZF(val);            \
        UPDATE_SF(val);            \
    }

CMP(I, MEMORY(PC++))
CMP(ACC, ACC)
CMP(ML, MEMORY(L))
CMP(MHL, MEMORY(HL))
CMP(R0, R0)
CMP(R1, R1)
CMP(L, L)
CMP(H, H)

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
POP(FLAGS, PC += 1, FLAGS)

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
INSTRUCTION(ENTER) {
    PC++;
    BP = SP;
    SP += MEMORY(PC++);  // space for local variables
    STACK(SP++) = BP;
    BP = SP;
}
INSTRUCTION(LEAVE) {
    PC++;
    BP = STACK(--SP);
    SP = BP;
}
LOAD(BPI, PC += 2, STACK(BP - MEMORY(PC - 1)))
STORE(BPI, PC += 2, STACK(BP - MEMORY(PC - 1)))
INSTRUCTION(ADD_L_I) {
    PC += 2;
    L += MEMORY(PC - 1);
}
INSTRUCTION(ADD_HL_I) {
    PC += 3;
    uint16_t val = HL + (((uint16_t)(MEMORY(PC - 2)) << 8) | ((uint16_t)MEMORY(PC - 1)));
    H = (uint8_t)((val >> 8) & 0xFF);
    L = (uint8_t)MEMORY(PC - 1);
}
INSTRUCTION(LOAD_L_I) {
    PC += 2;
    L = MEMORY(PC - 1);
}
INSTRUCTION(LOAD_HL_I) {
    PC += 3;
    uint16_t val = ((uint16_t)(MEMORY(PC - 2)) << 8) | ((uint16_t)MEMORY(PC - 1));
    H = (uint8_t)((val >> 8) & 0xFF);
    L = (uint8_t)MEMORY(PC - 1);
}

#define MIN(variation, src)        \
    INSTRUCTION(MIN_##variation) { \
        PC++;                      \
        if (src < ACC) {           \
            ACC = src;             \
        }                          \
        UPDATE_ZF(ACC);            \
        UPDATE_SF(ACC);            \
    }

MIN(I, MEMORY(PC++))
MIN(ML, MEMORY(L))
MIN(MHL, MEMORY(HL))
MIN(R0, R0)
MIN(R1, R1)
MIN(L, L)
MIN(H, H)

#define MAX(variation, src)        \
    INSTRUCTION(MAX_##variation) { \
        PC++;                      \
        if (src > ACC) {           \
            ACC = src;             \
        }                          \
        UPDATE_ZF(ACC);            \
        UPDATE_SF(ACC);            \
    }

MAX(I, MEMORY(PC++))
MAX(ML, MEMORY(L))
MAX(MHL, MEMORY(HL))
MAX(R0, R0)
MAX(R1, R1)
MAX(L, L)
MAX(H, H)

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
    SHL_I,      unused,     SHL_ML,     SHL_MHL,    SHL_R0,     SHL_R1,     SHL_L,      SHL_H,
    SHR_I,      unused,     SHR_ML,     SHR_MHL,    SHR_R0,     SHR_R1,     SHR_L,      SHR_H,
    ROL_I,      unused,     ROL_ML,     ROL_MHL,    ROL_R0,     ROL_R1,     ROL_L,      ROL_H,
    ROR_I,      unused,     ROR_ML,     ROR_MHL,    ROR_R0,     ROR_R1,     ROR_L,      ROR_H,
    ADDW_I,     ADDW_ACC,   ADDW_R0,    ADDW_R1,    SUBW_I,     SUBW_ACC,   SUBW_R0,    SUBW_R1,
    MULW_I,     MULW_ACC,   MULW_R0,    MULW_R1,    DIVW_I,     DIVW_ACC,   DIVW_R0,    DIVW_R1,
    JMP,        JIHL,       JHL,        JZ,         JNZ,        JC,         JNC,        JEXT,
    CMP_I,      CMP_ACC,    CMP_ML,     CMP_MHL,    CMP_R0,     CMP_R1,     CMP_L,      CMP_H,
    PUSH_I,     PUSH_ACC,   PUSH_R0,    PUSH_R1,    PUSH_L,     PUSH_H,     PUSH_BP,    PUSH_FLAGS,
    POP_IM,     POP_ACC,    POP_R0,     POP_R1,     POP_L,      POP_H,      POP_BP,     POP_FLAGS,
    CALL,       RET,        ENTER,      LEAVE,      LOAD_BPI,   STORE_BPI,  ADD_L_I,    ADD_HL_I,
    MIN_I,      unused,     MIN_ML,     MIN_MHL,    MIN_R0,     MIN_R1,     MIN_L,      MIN_H, 
    MAX_I,      unused,     MAX_ML,     MAX_MHL,    MAX_R0,     MAX_R1,     MAX_L,      MAX_H, 
    unused,     unused,     unused,     unused,     unused,     unused,     unused,     unused, 
    unused,     unused,     unused,     unused,     unused,     unused,     unused,     unused, 
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
    // OP_unused    = 121,
    OP_SHL_ML       = 122,
    OP_SHL_MHL      = 123,
    OP_SHL_R0       = 124,
    OP_SHL_R1       = 125,
    OP_SHL_L        = 126,
    OP_SHL_H        = 127,
    OP_SHR_I        = 128,
    // OP_unused    = 129,
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
    OP_JIHL         = 169,
    OP_JHL          = 170,
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
    // OP_unused    = 209,
    OP_MIN_ML       = 210,
    OP_MIN_MHL      = 211,
    OP_MIN_R0       = 212,
    OP_MIN_R1       = 213,
    OP_MIN_L        = 214,
    OP_MIN_H        = 215,
    OP_MAX_I        = 216,
    // OP_unused    = 217,
    OP_MAX_ML       = 218,
    OP_MAX_MHL      = 219,
    OP_MAX_R0       = 220,
    OP_MAX_R1       = 221,
    OP_MAX_L        = 222,
    OP_MAX_H        = 223,
    // OP_unused    = 224,
    // OP_unused    = 225,
    // OP_unused    = 226,
    // OP_unused    = 227,
    // OP_unused    = 228,
    // OP_unused    = 229,
    // OP_unused    = 230,
    // OP_unused    = 231,
    // OP_unused    = 232,
    // OP_unused    = 233,
    // OP_unused    = 234,
    // OP_unused    = 235,
    // OP_unused    = 236,
    // OP_unused    = 237,
    // OP_unused    = 238,
    // OP_unused    = 239,
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
