# Building a CPU with custom ISA and emulator

At any point in time, this file might be completely out of date, sadly.

## Architecture

* 16 bit instructions
* 16 bit registers

## CPU flags register

```
   15     ...      8     7    ...    4   3   2   1   0
+-----+---------+-----+----+-------+---+---+---+---+---+
| INT |   ...   | HLT | BC |  ...  | C | O | N | E | Z |
+-----+---------+-----+----+-------+---+---+---+---+---+
```

* `INT` : Interrupt flag, SET when handling an interrupt
* `HLT` : CPU is halted
* `BC` : Skip next branch if set
* `C` : Carry bit from last operation
* `O` : Last operation caused an overflow
* `N` : Last operation's result was negative
* `E` : Last comparison yielded equality
* `Z` : Last operation's result was zero

## Memory model

For now, flat 16-bit address space.

# Instruction Set

## Instruction structure

```
15               8 7                0
+-----------------+----------------+
|    opcode       |     param      |
+-----------------+----------------+
```

## Addressing modes

Register direct, register indirect, some sort of indexed addressing.
Basic immediate addressing.

## Instructions required

### Load / Store

*    LOAD to register from register
*    LOAD to register from register indirect
*    LOAD to register from immediate 4-bit
*    LOAD to register from register indirect plus index
*    STORE to memory from register
*    STORE to memory from immediate 4-bit
*    STORE to memory from register indirect plus index


### Flow control

*    `JMP` (unconditional jump)
*    Branch if...
*    Interrupt
*    Halt

### Logic

*    AND / OR / NOT / XOR
*    Comparisons

### Arithmetic

*    ADD / SUB with and without carry


## Opcodes
```
           x0     x1     x2     x3     x4     x5     x6     x7     x8     x9     xA     xB     xC     xD     xE     xF
  0x  :  _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt,
  1x  :  _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt,
  2x  :  _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt,
  3x  :  _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt,
  4x  :  _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt,
  5x  :  _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt,
  6x  :  _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt,
  7x  :  _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt,
  8x  :  _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt,
  9x  :  _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt,
  Ax  :  _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt,
  Bx  :  _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt,
  Cx  :  _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt,
  Dx  :  _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt,
  Ex  :  _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt,
  Fx  :  _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt, _halt
```
