# 8-bit VM

Open learning personal project. My goal is to create an emulator based on a custom designed CPU spec, then write a playable game for it.

## Usage

To build, simply run `make`. Then, you can do

```sh
./build/vm <filepath>.casm
```

Note that your executable might be called `./build/vm.exe` on windows.

## Design and specification

The core of the system features an 8-bit CPU, similar to existing 8-bit processors like the 6502 or the Z80. It has the following properties:

* 64kB of memory
* Program counter
* An accumulator: the result of all ALU operations are implicitly stored here
* Four 8-bit general purpose registers: `R0`, `R1`, `H`, `L`
* Index memory with the `HL` pair, which also allows for general-purpose 16-bit computing
* Flags register
* Different addressing modes for each instruction
* 256 byte stack
* Stack pointer and base pointer to create stack frames

## Instructions

There are 44 different instructions, many with a set of different addressing modes:

```
NOOP   -  No Operation
HALT   -  Halts program execution
EI     -  Enables interrupt flag
DI     -  Disables interrupt flag
ET     -  Enables trap flag, which allows you to step through a program
DT     -  Disables trap flag
RESET  -  Soft resets CPU
LOAD   -  Load a value into the accumulator
STORE  -  Stores value from the accumulator
XCH    -  Exchanges value with the accumulator
ADD    -  Adds value to the accumulator
ADC    -  Adds value + possible carry bit to the accumulator
SUB    -  Subtracts value from the accumulator
SBC    -  Subtracts value + possible carry bit from the accumulator
INC    -  Increments a value
DEC    -  Decrements a value
NEG    -  Negates a value
NOT    -  Applies bitwise NOT to value
AND    -  Bitwise AND between value and accumulator
OR     -  Bitwise OR between value and accumulator
XOR    -  Bitwise XOR between value and accumulator
SHL    -  Shift accumulator left based on value
SHR    -  Shift accumulator right based on value
ROL    -  Rotates accumulator left based on value
ROR    -  Rotates accumulator right based on value
ADDW   -  Adds value to HL pair
SUBW   -  Subtracts value from HL pair
MULW   -  Multiplies value with HL pair
DIVW   -  Divides HL pair by value
JMP    -  Jumps to address
JZ     -  Jumps to address if the zero flag is set
JNZ    -  Jumps to address if the zero flag is not set
JC     -  Jumps to address if the carry flag is set
JNC    -  Jumps to address if the carry flag is not set
JEXT   -  Extended jump check which jumps if condition is met
CMP    -  Compares two values and sets flags accordingly
PUSH   -  Push value onto the stack
POP    -  Pop value from the stack
CALL   -  Calls function and pushes return value onto the stack
RET    -  Pops return value from the stack and jumps back to it
ENTER  -  Creates a stack frame of size N
LEAVE  -  Leaves stack frame
MIN    -  Takes the minimum of two values and stores in the accumulator
MAX    -  Takes the maximum of two values and stores in the accumulator
```
