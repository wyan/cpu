# Building a CPU with custom ISA and emulator

At any point in time, this file might be completely out of date, sadly.

## Architecture

* 16 bit instructions
* 16 bit registers

## CPU flags register

```
   15     ...      8     7      ...      3   2   1   0
+-----+---------+-----+----+-----------+---+---+---+---+
| INT |   ...   | HLT | BC |    ...    | C | O | N | Z |
+-----+---------+-----+----+-----------+---+---+---+---+
```

* `INT` : Interrupt flag, SET when handling an interrupt
* `HLT` : CPU is halted
* `BC` : Skip next branch if set (possibly to be removed)
* `C` : Carry bit from last operation
* `O` : Last operation caused an overflow
* `N` : Last operation's result was negative
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
    * absolute and relative
    * immediate and register
*    Conditional jump (branch)
*    Interrupt
*    Halt

#### Conditional jumps

 * Equal / Zero (Z=1)
 * Not equal / not zero (Z=0)
 * Below / Carry (C=1)
 * Below or equal (C=1 || Z=1)
 * Above (C=0 && Z=0)
 * Above or equal / Not carry (C=0)
 * Less (N != O)
 * Less or equal ((N != O) || Z=1)
 * Greater ((N == O) && Z=0)
 * Greater or equal ((N == O) || Z=1)
 * Overflow (O=1)
 * Not overflow (O=0)
 * Negative (N=1)
 * Not negative (N=0)

0000  No condition
0001  EQ
0010  B
0011  BE
0100  L
0101  LE
0110  N
0111  O

1000  ---
1001  NEQ
1010  AE
1011  A
1100  G
1101  GE
1110  NN
1111  NO


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

# Tests

I am using the Unity framework under the MIT License: https://github.com/ThrowTheSwitch/Unity
