# Custom Console Emulator

Open learning personal project. My goal is to create an emulator based on a custom designed CPU spec, then write a playable tetris game for it.

## Design and specification

The core of the system features an 8-bit CPU, similar to existing 8-bit processors like the 6502 or the Z80. It has the following properties:

* 64kB of memory
* An accumulator: the result of all ALU operations are implicitly stored here
* Four 8-bit general purpose registers:
    - 0, 1: General purpose
    - H, L: Used to access memory. When not used for this purpose, you can also use them as general purpose registers.
* Flags register
* Program counter
* Eight addressing modes
* 256 byte stack
* Stack pointer and base pointer to create stack frames

## Addressing modes

For most instructions, The first 5 bits are to specify a specific instruction. The last 3 bits are to specify different addressing modes. The data required for a particular operation is selected based on these 8 different values.

1. `0b000` -> Immediate value
1. `0b001` -> Accumulator
1. `0b010` -> Memory at `(L)`
1. `0b011` -> Memory at `(HL)`
1. `0b100` -> Register 0
1. `0b101` -> Register 1
1. `0b110` -> Register H
1. `0b111` -> Register L

Not all instructions require addressing modes though, so that leaves space for other limited-purpose instructions.

## Instructions

```
LOAD   -  Load value into accumulator
STORE  -  Store accumulator
XCH    -  Exchange value with accumulator
ADD    -  Add value to accumulator
ADC    -  Add value with carry flag to the accumulator
SUB    -  Subtract value from accumulator
SBC    -  Subtract value with carry flag from accumulator
INC    -  Increment value
DEC    -  Decrement value
NEG    -  Negate value
AND    -  Bitwise AND
OR     -  Bitwise OR
XOR    -  Bitwise XOR
NOT    -  Bitwise NOT
SHL    -  Shift left
SHR    -  Shift right
ROL    -  Rotate left
ROR    -  Rotate right
SWAP   -  Swap upper and lower 4 bits of value
CMP    -  Compare value with accumulator and set flags accordingly

The next four instructions only work with operands immediate, accumulator and registers 0 and 1:

ADDW   -  Adds value to HL, treating HL as a single 16-bit value
SUBW   -  Subtracts value from HL, treating HL as a single 16-bit value
MULW   -  Multiplies HL with value, treating HL as a single 16-bit value
DIVW   -  Divides HL by value, treating HL as a single 16-bit value

CALL   -  Call subroutine, jump to immediate
CALLHL -  Call subroutine, jump to (HL)
RET    -  Return from subroutine: pop return address from stack to HL and jump to it
PUSH   -  Push value onto the stack
POP    -  Pops value from the stack
PUSHWI -  Push immediate word to stack
PUSHW  -  Push HL to stack
POPW   -  Pop word off stack into HL
PUSHF  -  Push flags to stack
POPF   -  Pop flags from stack

NOOP   -  No operation
HALT   -  Halt the program
RESET  -  Soft reset the CPU
EI     -  Enable interrupts
DI     -  Disable interrupts
CLRA   -  Clear accumulator (shortcut to skip the need for an immediate value)
FILL   -  Fill memory block at (HL) with value, auto increments (HL)
MOVS   -  Copy block of memory from (HL) to register pair (01). Accumulator stores length

CALL   -  Call subroutine: push return address to stack and jump to address
ENTER  -  Creates a new stack frame of size N based on value
LEAVE  -  Returns to last stack frame

Jump instructions

JMP    -  Unconditional jump to immediate address
JHL    -  Unconditional jump to address at HL
JL     -  Unconditional jump to address at L
JZ     -  Jump if zero flag is set
JNZ    -  Jump if zero flag is not set
JC     -  Jump if carry flag is set
JNC    -  Jump if carry flag is not set
JEXT   -  Jump with custom flag matching in the extended byte


```

You might find `HALT` to be missing and you would be right. Certain instructions like `NOOP` don't make use of the addressing modes, so some have been repurposed.

### NOOP addressing modes

```
0b000:  NOOP   -  No operation
0b001:  HALT   -  Halt the program
0b010:  RESET  -  Soft reset the CPU
0b011:  EI     -  Enable interrupts
0b100:  DI     -  Disable interrupts
0b101:  CLRA   -  Clear accumulator (shortcut to skip the need for an immediate value)
0b110:  PUSHF  -  Push flags to stack
0b111:  POPF   -  Pop flags from stack
```

### JMP addressing modes

```
0b000:  JMP    -  Unconditional jump to immediate address
0b001:  JHL    -  Unconditional jump to address at HL
0b010:  JL     -  Unconditional jump to address at L
0b011:  JZ     -  Jump if zero flag is set
0b100:  JNZ    -  Jump if zero flag is not set
0b101:  JC     -  Jump if carry flag is set
0b110:  JNC    -  Jump if carry flag is not set
0b111:  JEXT   -  Jump with custom flag matching in the extended byte
```

### JEXT

```
CC     = Comparison type (how to interpret FFFFF)
FFFFFF = Flags to test

0b00:  ANY    -  Jump if any specified flag is 1
0b01:  ALL    -  Jump if all specified flags are 1
0b10:  NONE   -  Jump if all specified flags are 0
0b11:  NOT    -  Jump if any specified flag is 0

0b000001: CF  -  Check Carry flag
0b000010: AF  -  Check Auxillary carry flag
0b000100: ZF  -  Check Zero flag
0b001000: SF  -  Check Signed flag
0b010000: TF  -  Check Trap flag
0b100000: OF  -  Check Overflow flag
```

### CALL addressing modes

```
0b000:  CALL   -  Call subroutine, jump to immediate
0b001:  CALLHL -  Call subroutine, jump to (HL)
0b010:  RET    -  Return from subroutine: pop return address from stack to HL and jump to it
0b011:  PUSHWI -  Push immediate word to stack
0b100:  PUSHW  -  Push HL to stack
0b101:  POPW   -  Pop word off stack into HL
0b110:  LOADPC -  
0b111:  
```

```
0b000:  PUSH   -  PUSH accumulator to stack
0b010:  PUSHW  -  Push HL to stack
0b001:  PUSHWI -  Push immediate word to stack
0b011:  POP    -  POP top of stack into accumulator
0b100:  POPW   -  Pop word off stack into HL
0b101:  CALL   -  Call subroutine, jump to immediate
0b110:  CALLHL -  Call subroutine, jump to (HL)
0b111:  RET    -  Return from subroutine: pop return address from stack and jump to it
```
