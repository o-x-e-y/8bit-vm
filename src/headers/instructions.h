#ifndef __OXEY_CCE_INSTRUCTIONS_H
#define __OXEY_CCE_INSTRUCTIONS_H

#include <limits.h>
#include <stdio.h>

#include "common.h"
#include "cpu.h"

#define INSTRUCTION_WIDTH 8

typedef void (*Instruction)(CPU *cpu);
#define INSTRUCTION(name) static void name(CPU *cpu)
#define UPDATE_ZF(src) UPDATE_FLAGS(src == 0, zf)
#define UPDATE_SF(src) UPDATE_FLAGS(src &SIGN_BIT, sf)

INSTRUCTION(NOOP) { PC++; }
INSTRUCTION(HALT) {}
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

#define LOAD(variation, src)        \
    INSTRUCTION(LOAD_##variation) { \
        PC++;                       \
        ACC = src;                  \
        UPDATE_ZF(ACC);             \
        UPDATE_SF(ACC);             \
    }

LOAD(I, MEMORY(PC++))
LOAD(IMHL, MEMORY((((uint16_t)MEMORY(PC++) << 8) | (uint16_t)MEMORY(PC++)) + HL))
LOAD(ML, MEMORY(L))
LOAD(MHL, (MEMORY(HL)))
LOAD(R0, R0)
LOAD(R1, R1)
LOAD(L, L)
LOAD(H, H)

#define STORE(variation, dest)       \
    INSTRUCTION(STORE_##variation) { \
        PC++;                        \
        dest = ACC;                  \
        UPDATE_ZF(ACC);              \
        UPDATE_SF(ACC);              \
    }

STORE(IM, MEMORY(((uint16_t)MEMORY(PC++) << 8) | (uint16_t)MEMORY(PC++)))
STORE(IMHL, MEMORY((((uint16_t)MEMORY(PC++) << 8) | (uint16_t)MEMORY(PC++)) + HL))
STORE(ML, MEMORY(L))
STORE(MHL, MEMORY(HL))
STORE(R0, R0)
STORE(R1, R1)
STORE(L, L)
STORE(H, H)

#define XCH(variation, src)        \
    INSTRUCTION(XCH_##variation) { \
        PC++;                      \
        uint8_t help = ACC;        \
        ACC = src;                 \
        src = help;                \
        UPDATE_ZF(ACC);            \
        UPDATE_SF(ACC);            \
    }

XCH(ML, MEMORY(L))
XCH(MHL, MEMORY(HL))
XCH(R0, R0)
XCH(R1, R1)
XCH(L, L)
XCH(H, H)

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
ADD(A, ACC)
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
ADC(A, ACC)
ADC(ML, MEMORY(L))
ADC(MHL, MEMORY(HL))
ADC(R0, R0)
ADC(R1, R1)
ADC(L, L)
ADC(H, H)

#define SUB(variation, src)          \
    INSTRUCTION(SUB_##variation) {   \
        PC++;                        \
        ACC -= src;                  \
        UPDATE_ZF(ACC);              \
        UPDATE_SF(ACC);              \
        UPDATE_FLAGS(ACC > src, cf); \
    }

SUB(I, MEMORY(PC++))
SUB(A, ACC)
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
SBC(A, ACC)
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

INC(A, ACC)
INC(ML, MEMORY(L))
INC(MHL, MEMORY(HL))
INC(R0, R0)
INC(R1, R1)
INC(L, L)
INC(H, H)

#define DEC(variation, src)        \
    INSTRUCTION(DEC_##variation) { \
        PC++;                      \
        src--;                     \
        UPDATE_ZF(src);            \
        UPDATE_SF(src);            \
    }

DEC(A, ACC)
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

NEG(A, ACC)
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

NOT(A, ACC)
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
AND(A, ACC)
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
OR(A, ACC)
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
XOR(A, ACC)
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

#define SWAP(variation, src)                         \
    INSTRUCTION(SWAP_##variation) {                  \
        PC++;                                        \
        uint8_t val = src;                           \
        src = (val & 0x0F) << 4 | (val & 0xF0) >> 4; \
        UPDATE_ZF(src);                              \
        UPDATE_SF(src);                              \
    }

SWAP(A, ACC)
SWAP(ML, MEMORY(L))
SWAP(MHL, MEMORY(HL))
SWAP(R0, R0)
SWAP(R1, R1)
SWAP(L, L)
SWAP(H, H)

#define OPW(opname, op, variation, src)     \
    INSTRUCTION(opname##_##variation) {     \
        PC++;                               \
        const uint16_t val = HL op src;     \
        H = (uint8_t)(val >> 8);            \
        L = (uint8_t)(val & 0xFF);          \
        UPDATE_FLAGS(H == 0 && L == 0, zf); \
        UPDATE_FLAGS(H &SIGN_BIT, sf);      \
    }

OPW(ADDW, +, I, MEMORY(PC++))
OPW(ADDW, +, A, ACC)
OPW(ADDW, +, R0, R0)
OPW(ADDW, +, R1, R1)

OPW(SUBW, -, I, MEMORY(PC++))
OPW(SUBW, -, A, ACC)
OPW(SUBW, -, R0, R0)
OPW(SUBW, -, R1, R1)

OPW(MULW, *, I, MEMORY(PC++))
OPW(MULW, *, A, ACC)
OPW(MULW, *, R0, R0)
OPW(MULW, *, R1, R1)

OPW(DIVW, /, I, MEMORY(PC++))
OPW(DIVW, /, A, ACC)
OPW(DIVW, /, R0, R0)
OPW(DIVW, /, R1, R1)

INSTRUCTION(JMP) {
    PC++;
    PC = ((uint16_t)MEMORY(PC++) << 8) | ((uint16_t)MEMORY(PC++));
}
INSTRUCTION(JIHL) {
    PC++;
    PC = (((uint16_t)MEMORY(PC++) << 8) | ((uint16_t)MEMORY(PC++))) + HL;
}
INSTRUCTION(JHL) { PC = HL; }
INSTRUCTION(JZ) {
    PC++;
    if (get_zf(FLAGS))
        PC = ((uint16_t)MEMORY(PC++) << 8) | ((uint16_t)MEMORY(PC++));
    else
        PC += 2;
}
INSTRUCTION(JNZ) {
    PC++;
    if (!get_zf(FLAGS))
        PC = ((uint16_t)MEMORY(PC++) << 8) | ((uint16_t)MEMORY(PC++));
    else
        PC += 2;
}
INSTRUCTION(JC) {
    PC++;
    if (get_cf(FLAGS))
        PC = ((uint16_t)MEMORY(PC++) << 8) | (uint16_t)MEMORY(PC++);
    else
        PC += 2;
}
INSTRUCTION(JNC) {
    PC++;
    if (!get_cf(FLAGS))
        PC = ((uint16_t)MEMORY(PC++) << 8) | ((uint16_t)MEMORY(PC++));
    else
        PC += 2;
}
INSTRUCTION(JEXT) {
    PC++;
    uint8_t jext = MEMORY(PC);
    uint8_t flags = FLAGS & 0b111111;
    uint8_t mask = jext & 0b111111;
    switch ((jext >> 6) & 0b11) {
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
CMP(A, ACC)
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

PUSH(A, ACC)
PUSH(I, MEMORY(PC++))
PUSH(R0, R0)
PUSH(R1, R1)
PUSH(L, L)
PUSH(H, H)
PUSH(BP, BP);
PUSH(F, FLAGS);

#define POP(variation, dest)       \
    INSTRUCTION(POP_##variation) { \
        PC++;                      \
        uint8_t val = STACK(--SP); \
        dest = val;                \
        UPDATE_ZF(val);            \
        UPDATE_SF(val);            \
    }

POP(A, ACC)
POP(IM, MEMORY(((uint16_t)MEMORY(PC++) << 8) | (uint16_t)MEMORY(PC++)))
POP(R0, R0)
POP(R1, R1)
POP(L, L)
POP(H, H)
POP(BP, BP);
POP(F, FLAGS);

INSTRUCTION(CALL) {
    PC++;
    uint16_t addr = ((uint16_t)MEMORY(PC++) << 8) | (uint16_t)MEMORY(PC++);
    STACK(SP++) = (uint8_t)(PC >> 8);
    STACK(SP++) = (uint8_t)(PC);
    PC = addr;
}
INSTRUCTION(RET) {
    uint8_t low = (uint16_t)STACK(--SP);
    uint8_t high = (uint16_t)STACK(--SP);
    PC = low | (high << 8);
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
LOAD(BPI, STACK(BP - MEMORY(PC++)))
STORE(BPI, STACK(BP - MEMORY(PC++)))

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
const static Instruction OP_TABLE[256] = {
    NOOP,       HALT,       EI,         DI,         ET,         DT,         CLRA,       RESET, 
    LOAD_I,     LOAD_IMHL,  LOAD_ML,    LOAD_MHL,   LOAD_R0,    LOAD_R1,    LOAD_L,     LOAD_H,
    STORE_IM,   STORE_IMHL, STORE_ML,   STORE_MHL,  STORE_R0,   STORE_R1,   STORE_L,    STORE_H,
    unused,     unused,     XCH_ML,     XCH_MHL,    XCH_R0,     XCH_R1,     XCH_L,      XCH_H,
    ADD_I,      ADD_A,      ADD_ML,     ADD_MHL,    ADD_R0,     ADD_R1,     ADD_L,      ADD_H,
    ADC_I,      ADC_A,      ADC_ML,     ADC_MHL,    ADC_R0,     ADC_R1,     ADC_L,      ADC_H,
    SUB_I,      SUB_A,      SUB_ML,     SUB_MHL,    SUB_R0,     SUB_R1,     SUB_L,      SUB_H,
    SBC_I,      SBC_A,      SBC_ML,     SBC_MHL,    SBC_R0,     SBC_R1,     SBC_L,      SBC_H,
    unused,     INC_A,      INC_ML,     INC_MHL,    INC_R0,     INC_R1,     INC_L,      INC_H,
    unused,     DEC_A,      DEC_ML,     DEC_MHL,    DEC_R0,     DEC_R1,     DEC_L,      DEC_H,
    unused,     NEG_A,      NEG_ML,     NEG_MHL,    NEG_R0,     NEG_R1,     NEG_L,      NEG_H,
    unused,     NOT_A,      NOT_ML,     NOT_MHL,    NOT_R0,     NOT_R1,     NOT_L,      NOT_H, 
    AND_I,      AND_A,      AND_ML,     AND_MHL,    AND_R0,     AND_R1,     AND_L,      AND_H,
    OR_I,       OR_A,       OR_ML,      OR_MHL,     OR_R0,      OR_R1,      OR_L,       OR_H, 
    XOR_I,      XOR_A,      XOR_ML,     XOR_MHL,    XOR_R0,     XOR_R1,     XOR_L,      XOR_H, 
    SHL_I,      unused,     SHL_ML,     SHL_MHL,    SHL_R0,     SHL_R1,     SHL_L,      SHL_H,
    SHR_I,      unused,     SHR_ML,     SHR_MHL,    SHR_R0,     SHR_R1,     SHR_L,      SHR_H,
    ROL_I,      unused,     ROL_ML,     ROL_MHL,    ROL_R0,     ROL_R1,     ROL_L,      ROL_H,
    ROR_I,      unused,     ROR_ML,     ROR_MHL,    ROR_R0,     ROR_R1,     ROR_L,      ROR_H,
    ADDW_I,     ADDW_A,     ADDW_R0,    ADDW_R1,    SUBW_I,     SUBW_A,     SUBW_R0,    SUBW_R1,
    MULW_I,     MULW_A,     MULW_R0,    MULW_R1,    DIVW_I,     DIVW_A,     DIVW_R0,    DIVW_R1,
    JMP,        JIHL,       JHL,        JZ,         JNZ,        JC,         JNC,        JEXT,
    CMP_I,      CMP_A,      CMP_ML,     CMP_MHL,    CMP_R0,     CMP_R1,     CMP_L,      CMP_H,
    PUSH_A,     PUSH_I,     PUSH_R0,    PUSH_R1,    PUSH_L,     PUSH_H,     PUSH_BP,    PUSH_F,
    POP_A,      POP_IM,     POP_R0,     POP_R1,     POP_L,      POP_H,      POP_BP,     POP_F,
    CALL,       RET,        ENTER,      LEAVE,      LOAD_BPI,   STORE_BPI,  unused,     unused, 
    MIN_I,      unused,     MIN_ML,     MIN_MHL,    MIN_R0,     MIN_R1,     MIN_L,      MIN_H, 
    MAX_I,      unused,     MAX_ML,     MAX_MHL,     MAX_R0,    MAX_R1,     MAX_L,      MAX_H, 
    unused,     unused,     unused,     unused,     unused,     unused,     unused,     unused, 
    unused,     unused,     unused,     unused,     unused,     unused,     unused,     unused, 
    unused,     unused,     unused,     unused,     unused,     unused,     unused,     unused, 
    unused,     unused,     unused,     unused,     unused,     unused,     unused,     unused,
};

typedef enum Opcode {
    OP_NOOP         = 0b00000000,
    OP_HALT         = 0b00000001,
    OP_EI           = 0b00000010,
    OP_DI           = 0b00000011,
    OP_ET           = 0b00000100,
    OP_DT           = 0b00000101,
    OP_CLRA         = 0b00000110,
    OP_RESET        = 0b00000111,
    OP_LOAD_I       = 0b00001000,
    OP_LOAD_IMHL    = 0b00001001,
    OP_LOAD_ML      = 0b00001010,
    OP_LOAD_MHL     = 0b00001011,
    OP_LOAD_R0      = 0b00001100,
    OP_LOAD_R1      = 0b00001101,
    OP_LOAD_L       = 0b00001110,
    OP_LOAD_H       = 0b00001111,
    OP_STORE_IM     = 0b00010000,
    OP_STORE_IMHL   = 0b00010001,
    OP_STORE_ML     = 0b00010010,
    OP_STORE_MHL    = 0b00010011,
    OP_STORE_R0     = 0b00010100,
    OP_STORE_R1     = 0b00010101,
    OP_STORE_L      = 0b00010110,
    OP_STORE_H      = 0b00010111,
    // OP_unused    = 0b00011000,
    // OP_unused    = 0b00011001,
    OP_XCH_ML       = 0b00011010,
    OP_XCH_MHL      = 0b00011011,
    OP_XCH_R0       = 0b00011100,
    OP_XCH_R1       = 0b00011101,
    OP_XCH_L        = 0b00011110,
    OP_XCH_H        = 0b00011111,
    OP_ADD_I        = 0b00100000,
    OP_ADD_A        = 0b00100001,
    OP_ADD_ML       = 0b00100010,
    OP_ADD_MHL      = 0b00100011,
    OP_ADD_R0       = 0b00100100,
    OP_ADD_R1       = 0b00100101,
    OP_ADD_L        = 0b00100110,
    OP_ADD_H        = 0b00100111,
    OP_ADC_I        = 0b00101000,
    OP_ADC_A        = 0b00101001,
    OP_ADC_ML       = 0b00101010,
    OP_ADC_MHL      = 0b00101011,
    OP_ADC_R0       = 0b00101100,
    OP_ADC_R1       = 0b00101101,
    OP_ADC_L        = 0b00101110,
    OP_ADC_H        = 0b00101111,
    OP_SUB_I        = 0b00110000,
    OP_SUB_A        = 0b00110001,
    OP_SUB_ML       = 0b00110010,
    OP_SUB_MHL      = 0b00110011,
    OP_SUB_R0       = 0b00110100,
    OP_SUB_R1       = 0b00110101,
    OP_SUB_L        = 0b00110110,
    OP_SUB_H        = 0b00110111,
    OP_SBC_I        = 0b00111000,
    OP_SBC_A        = 0b00111001,
    OP_SBC_ML       = 0b00111010,
    OP_SBC_MHL      = 0b00111011,
    OP_SBC_R0       = 0b00111100,
    OP_SBC_R1       = 0b00111101,
    OP_SBC_L        = 0b00111110,
    OP_SBC_H        = 0b00111111,
    // OP_unused    = 0b01000000,
    OP_INC_A        = 0b01000001,
    OP_INC_ML       = 0b01000010,
    OP_INC_MHL      = 0b01000011,
    OP_INC_R0       = 0b01000100,
    OP_INC_R1       = 0b01000101,
    OP_INC_L        = 0b01000110,
    OP_INC_H        = 0b01000111,
    // OP_unused    = 0b01001000,
    OP_DEC_A        = 0b01001001,
    OP_DEC_ML       = 0b01001010,
    OP_DEC_MHL      = 0b01001011,
    OP_DEC_R0       = 0b01001100,
    OP_DEC_R1       = 0b01001101,
    OP_DEC_L        = 0b01001110,
    OP_DEC_H        = 0b01001111,
    // OP_unused    = 0b01010000,
    OP_NEG_A        = 0b01010001,
    OP_NEG_ML       = 0b01010010,
    OP_NEG_MHL      = 0b01010011,
    OP_NEG_R0       = 0b01010100,
    OP_NEG_R1       = 0b01010101,
    OP_NEG_L        = 0b01010110,
    OP_NEG_H        = 0b01010111,
    // OP_unused    = 0b01011000,
    OP_NOT_A        = 0b01011001,
    OP_NOT_ML       = 0b01011010,
    OP_NOT_MHL      = 0b01011011,
    OP_NOT_R0       = 0b01011100,
    OP_NOT_R1       = 0b01011101,
    OP_NOT_L        = 0b01011110,
    OP_NOT_H        = 0b01011111,
    OP_AND_I        = 0b01100000,
    OP_AND_A        = 0b01100001,
    OP_AND_ML       = 0b01100010,
    OP_AND_MHL      = 0b01100011,
    OP_AND_R0       = 0b01100100,
    OP_AND_R1       = 0b01100101,
    OP_AND_L        = 0b01100110,
    OP_AND_H        = 0b01100111,
    OP_OR_I         = 0b01101000,
    OP_OR_A         = 0b01101001,
    OP_OR_ML        = 0b01101010,
    OP_OR_MHL       = 0b01101011,
    OP_OR_R0        = 0b01101100,
    OP_OR_R1        = 0b01101101,
    OP_OR_L         = 0b01101110,
    OP_OR_H         = 0b01101111,
    OP_XOR_I        = 0b01110000,
    OP_XOR_A        = 0b01110001,
    OP_XOR_ML       = 0b01110010,
    OP_XOR_MHL      = 0b01110011,
    OP_XOR_R0       = 0b01110100,
    OP_XOR_R1       = 0b01110101,
    OP_XOR_L        = 0b01110110,
    OP_XOR_H        = 0b01110111,
    OP_SHL_I        = 0b01111000,
    // OP_unused    = 0b01111001,
    OP_SHL_ML       = 0b01111010,
    OP_SHL_MHL      = 0b01111011,
    OP_SHL_R0       = 0b01111100,
    OP_SHL_R1       = 0b01111101,
    OP_SHL_L        = 0b01111110,
    OP_SHL_H        = 0b01111111,
    OP_SHR_I        = 0b10000000,
    // OP_unused    = 0b10000001,
    OP_SHR_ML       = 0b10000010,
    OP_SHR_MHL      = 0b10000011,
    OP_SHR_R0       = 0b10000100,
    OP_SHR_R1       = 0b10000101,
    OP_SHR_L        = 0b10000110,
    OP_SHR_H        = 0b10000111,
    OP_ROL_I        = 0b10001000,
    // OP_unused    = 0b10001001,
    OP_ROL_ML       = 0b10001010,
    OP_ROL_MHL      = 0b10001011,
    OP_ROL_R0       = 0b10001100,
    OP_ROL_R1       = 0b10001101,
    OP_ROL_L        = 0b10001110,
    OP_ROL_H        = 0b10001111,
    OP_ROR_I        = 0b10010000,
    // OP_unused    = 0b10010001,
    OP_ROR_ML       = 0b10010010,
    OP_ROR_MHL      = 0b10010011,
    OP_ROR_R0       = 0b10010100,
    OP_ROR_R1       = 0b10010101,
    OP_ROR_L        = 0b10010110,
    OP_ROR_H        = 0b10010111,
    OP_ADDW_I       = 0b10011000,
    OP_ADDW_A       = 0b10011001,
    OP_ADDW_R0      = 0b10011010,
    OP_ADDW_R1      = 0b10011011,
    OP_SUBW_I       = 0b10011100,
    OP_SUBW_A       = 0b10011101,
    OP_SUBW_R0      = 0b10011110,
    OP_SUBW_R1      = 0b10011111,
    OP_MULW_I       = 0b10100000,
    OP_MULW_A       = 0b10100001,
    OP_MULW_R0      = 0b10100010,
    OP_MULW_R1      = 0b10100011,
    OP_DIVW_I       = 0b10100100,
    OP_DIVW_A       = 0b10100101,
    OP_DIVW_R0      = 0b10100110,
    OP_DIVW_R1      = 0b10100111,
    OP_JMP          = 0b10101000,
    OP_JIHL         = 0b10101001,
    OP_JHL          = 0b10101010,
    OP_JZ           = 0b10101011,
    OP_JNZ          = 0b10101100,
    OP_JC           = 0b10101101,
    OP_JNC          = 0b10101110,
    OP_JEXT         = 0b10101111,
    OP_CMP_I        = 0b10110000,
    OP_CMP_A        = 0b10110001,
    OP_CMP_ML       = 0b10110010,
    OP_CMP_MHL      = 0b10110011,
    OP_CMP_R0       = 0b10110100,
    OP_CMP_R1       = 0b10110101,
    OP_CMP_L        = 0b10110110,
    OP_CMP_H        = 0b10110111,
    OP_PUSH_A       = 0b10111000,
    OP_PUSH_I       = 0b10111001,
    OP_PUSH_R0      = 0b10111010,
    OP_PUSH_R1      = 0b10111011,
    OP_PUSH_L       = 0b10111100,
    OP_PUSH_H       = 0b10111101,
    OP_PUSH_BP      = 0b10111110,
    OP_PUSH_F       = 0b10111111,
    OP_POP_A        = 0b11000000,
    OP_POP_IM       = 0b11000001,
    OP_POP_R0       = 0b11000010,
    OP_POP_R1       = 0b11000011,
    OP_POP_L        = 0b11000100,
    OP_POP_H        = 0b11000101,
    OP_POP_BP       = 0b11000110,
    OP_POP_F        = 0b11000111,
    OP_CALL         = 0b11001000,
    OP_RET          = 0b11001001,
    OP_ENTER        = 0b11001010,
    OP_LEAVE        = 0b11001011,
    OP_LOAD_BPI     = 0b11001100,
    OP_STORE_BPI    = 0b11001101,
    // OP_unused    = 0b11001110,
    // OP_unused    = 0b11001111,
    // OP_unused    = 0b11010000,
    // OP_unused    = 0b11010001,
    // OP_unused    = 0b11010010,
    // OP_unused    = 0b11010011,
    // OP_unused    = 0b11010100,
    // OP_unused    = 0b11010101,
    // OP_unused    = 0b11010110,
    // OP_unused    = 0b11010111,
    // OP_unused    = 0b11011000,
    // OP_unused    = 0b11011001,
    // OP_unused    = 0b11011010,
    // OP_unused    = 0b11011011,
    // OP_unused    = 0b11011100,
    // OP_unused    = 0b11011101,
    // OP_unused    = 0b11011110,
    // OP_unused    = 0b11011111,
    // OP_unused    = 0b11100000,
    // OP_unused    = 0b11100001,
    // OP_unused    = 0b11100010,
    // OP_unused    = 0b11100011,
    // OP_unused    = 0b11100100,
    // OP_unused    = 0b11100101,
    // OP_unused    = 0b11100110,
    // OP_unused    = 0b11100111,
    // OP_unused    = 0b11101000,
    // OP_unused    = 0b11101001,
    // OP_unused    = 0b11101010,
    // OP_unused    = 0b11101011,
    // OP_unused    = 0b11101100,
    // OP_unused    = 0b11101101,
    // OP_unused    = 0b11101110,
    // OP_unused    = 0b11101111,
    // OP_unused    = 0b11110000,
    // OP_unused    = 0b11110001,
    // OP_unused    = 0b11110010,
    // OP_unused    = 0b11110011,
    // OP_unused    = 0b11110100,
    // OP_unused    = 0b11110101,
    // OP_unused    = 0b11110110,
    // OP_unused    = 0b11110111,
    // OP_unused    = 0b11111000,
    // OP_unused    = 0b11111001,
    // OP_unused    = 0b11111010,
    // OP_unused    = 0b11111011,
    // OP_unused    = 0b11111100,
    // OP_unused    = 0b11111101,
    // OP_unused    = 0b11111110,
    // OP_unused    = 0b11111111,
} Opcode;

// clang-format on

#endif
