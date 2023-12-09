# Building a CPU with custom ISA and emulator

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

* Relative addressing is 12-bit (4 kB pages)
* `JMP` can only branch to a 12 bit address (within the 4 kB page)
* How do we branch to a different 4 kB page?

# Instruction Set

## Instruction structure

```
15          12       8 7                0
+-----------+----------+----------------+
|  opcode1  |          |                |
+-----------+----------+----------------+
```

```
 -----------+------------------------------
   opcode1  |  value
 -----------+------------------------------
     000    |  Branching
     001    |  Logical operations
     010    |  Load/Store operations
     011    |  Arithmetical operations
     100    |  Flow
     101    |  
     110    |  
     111    |  
```
## `100 flow`

```
15      12       8 7                0
+-------+-----------+----------------+
|  100  |  opcode2  |                |
+-------+-----------+----------------+
```
with `opcode2` one of the following:
```
 -----------+------------------------------
   opcode2  |  value
 -----------+------------------------------
            |  Comparison to zero
            |  Comparison to negative
            |  Set branch condition to zero
            |  Set branch condition to negative
            |  
```



## Instructions required

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

### Load / Store

*    LOAD from memory   to register
*    LOAD from register to register
*    STORE to memory   immediate, direct, indirect
*    STORE to register immediate, direct, indirect
*    LOAD  indexed from memory to register
*    STORE indexed


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
