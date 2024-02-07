#ifndef CPU_H_
#define CPU_H_

#include <cstdint>
#include <cstring>
#include <string>

// CPU flags

#define FLAGS_INT      (1u << 15)    //  - we are handling an interrupt
#define FLAGS_HALT     (1u <<  8)    //  - CPU halted
#define FLAGS_BRANCH   (1u <<  7)    //  - branch flag: skip next branch if set
#define FLAGS_CARRY    (1u <<  3)    //  - last operation's carry bit
#define FLAGS_OVERFLOW (1u <<  2)    //  - last operation overflowed
#define FLAGS_NEG      (1u <<  1)    //  - last operation's result is < 0
#define FLAGS_ZERO     (1u <<  0)    //  - last operation was zero / equality

#define FLAGS_COND     (FLAGS_ZERO | FLAGS_NEG | FLAGS_OVERFLOW | FLAGS_CARRY)

// Opcode mask
#define OPCODE_MASK    (0xFF00)
#define PARAM_MASK     (0x00FF)

// CPU with mem_size bytes of memory

class CPU {

    const uint16_t mem_size; // size of system memory
    uint16_t *MEM;        // system memory

    uint16_t PC;          // Program Counter
    uint16_t FLAGS;       // CPU flags register
    uint16_t REG[16];     // 16 general purpose registers r0...r15
    uint16_t SP, SPX;     // stack pointer and shadow stack pointer

    uint16_t IR;          // internal instruction register

    void halt() { FLAGS |= FLAGS_HALT; }

public:

    CPU(const uint16_t mem_size) : mem_size(mem_size) { MEM = new uint16_t[mem_size]; }
    ~CPU() { delete MEM; }

    // initialization
    void reset();

    // run one CPU instruction
    void run_once();

    // functions representing the CPU pinout & I/O
    // ...

    // getters & setters
    uint16_t flags() const { return FLAGS; }
    bool halted() const { return (FLAGS & FLAGS_HALT); }
    bool carry() const { return (FLAGS & FLAGS_CARRY); }
    bool overflow() const { return (FLAGS & FLAGS_OVERFLOW); }
    bool negative() const { return (FLAGS & FLAGS_NEG); }
    bool zero() const { return (FLAGS & FLAGS_ZERO); }

    uint16_t getPC() const { return PC; }

    void loadmem(const uint16_t *buffer, const uint16_t size, const uint16_t start);
    uint16_t getmem_at(const uint16_t) const;

    void dump_memory() const;
    void dump_registers() const;
    void dump_flags() const;

    std::string condition_to_letters(uint16_t) const;
    bool check_condition(uint16_t) const;

    void update_flags(uint32_t val);
    void update_flags_arithmetic(uint32_t val, uint16_t op1, uint16_t op2);

};


#endif // CPU_H_
