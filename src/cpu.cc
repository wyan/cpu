#include <iostream>
#include <iomanip>
#include <bitset>
#include "cpu.h"

void CPU::reset() {
    FLAGS = 0;
    SP = SPX = 0;
    PC = 0x100;        // Start address for code

    for(auto &reg : REG) { reg = 0; }    // Zero out registers

    return;
}

void CPU::loadmem(const uint16_t *buffer, const uint16_t size, const uint16_t start) {
    const uint16_t size_norm = (size > mem_size - start) ? mem_size - start : size;
    memcpy((MEM + start), buffer, size_norm);
}

void CPU::dump_memory() const {
    std::cout << std::right << std::setbase(16) << std::noshowbase << std::setfill('0');
    for(int i = 0; i < mem_size; i++) {
        if(i % 16 == 0) std::cout << std::setw(4) << i << " : ";
        std::cout << std::setw(4) << MEM[i] << " ";
        if(i % 16 == 15) std::cout << std::endl;
    }
    std::cout << std::endl;
}

void CPU::dump_registers() const {
    for(int i = 0; i < 16; i++) {
        std::cout << "R"
                  << std::setbase(10) << std::noshowbase << std::setfill('0') << std::setw(2)
                  << i << "="
                  << std::right << std::setbase(16) << std::noshowbase << std::setfill('0') << std::setw(4)
                  << REG[i] << " ";
        if(i % 4 == 3) std::cout << std::endl;
    }
}

void CPU::dump_flags() const {
    std::bitset<16> flag_bitfield(flags());
    std::cout << "       I......HB..CONEZ" << std::endl;
    std::cout << "FLAGS: " << flag_bitfield << std::endl;
}

void CPU::update_flags(uint32_t val) {
    if(val == 0) {
        FLAGS |= FLAGS_ZERO;
    } else {
        FLAGS &= ~FLAGS_ZERO;
    }

    if(val & 0x8000) {
        FLAGS |= FLAGS_NEG;
    } else {
        FLAGS &= ~FLAGS_NEG;
    }
}

void CPU::update_flags_arithmetic(uint32_t val, uint16_t op1, uint16_t op2) {
    if(val > 0xFFFF) {
        FLAGS |= FLAGS_CARRY;
    } else {
        FLAGS &= ~FLAGS_CARRY;
    }

    //(~(a ^ b))&(a ^ c)&0x80
    if( (~(op1 ^ op2)) & (op1 ^ val) & 0x8000 ) {
        FLAGS |= FLAGS_OVERFLOW;
    } else {
        FLAGS &= ~FLAGS_OVERFLOW;
    }
}

std::string CPU::condition_to_letters(uint16_t condition) const {
    // We only compare the relevant 4-bits
    switch(condition & 0x000F) {
        case 0b0000:            // No condition
            return std::string("");
            break;
        case 0b0001:            // Equality / zero
            return std::string(".EQ");
            break;
        case 0b0010:            // Below (unsigned)
            return std::string(".B");
            break;
        case 0b0011:            // Below or equal
            return std::string(".BE");
            break;
        case 0b0100:            // Less (signed)
            return std::string(".L");
            break;
        case 0b0101:            // Less or equal
            return std::string(".LE");
            break;
        case 0b0110:            // Negative
            return std::string(".N");
            break;
        case 0b0111:            // Overflow
            return std::string(".O");
            break;
        case 0b1001:            // Not equal
            return std::string(".NEQ");
            break;
        case 0b1010:            // Above or equal
            return std::string(".AE");
            break;
        case 0b1011:            // Above
            return std::string(".A");
            break;
        case 0b1100:            // Greater than
            return std::string(".G");
            break;
        case 0b1101:            // Greater or equal
            return std::string(".GE");
            break;
        case 0b1110:            // Not negative (positive)
            return std::string(".P");
            break;
        case 0b1111:            // Not overflow
            return std::string(".NO");
            break;
        default:                // Undefined
            return std::string("UNDEFINED");
            break;
    }
}


void CPU::run_once() {
    // Keep initial PC for logging purposes
    uint16_t initial_pc = PC;

    // Fetch instruction & increment PC
    IR = MEM[PC++];

    // Decode instruction into opcode and parameters
    uint16_t opcode = (IR & OPCODE_MASK) >> 8;
    uint16_t params = IR & PARAM_MASK;

    std::cout << "Fetched at PC=" << std::right << std::setbase(16) << std::noshowbase << std::setfill('0')
              << std::setw(4) << initial_pc << " : "              
              << std::setw(4) << IR << "   "
              << "opcode=" << std::setw(2) << opcode << "   "
              << "params=" << std::setw(2) << params << "   ";

    // Execute
    // std::cout << "Running : ";
    if(opcode == 0xFF)          // NOP
    {
        std::cout << "NOP" << std::endl;
    }
    else if(opcode == 0xF8)     // HALT
    {
        halt();

        std::cout << "HALT" << std::endl;
    }
    else if(opcode == 0x00)     // LOAD indirect  REG[param_high] <- (REG[param_low])
    {
        uint16_t add = (params & 0x000F);
        uint16_t reg = (params >> 4) & 0x000F;

        uint16_t val = REG[reg] = MEM[REG[add]];
        update_flags(val);

        std::cout << std::setbase(10) << "LOAD r" << reg << ", (r" << add << ")" << std::endl;
    }
    else if(opcode == 0x01)     // LOAD immediate REG[param_high] <- imm4[param_low]
    {
        uint16_t imm = (params & 0x000F);
        uint16_t reg = (params >> 4) & 0x000F;

        uint16_t val = REG[reg] = imm;
        update_flags(val);

        std::cout << std::setbase(10) << "LOAD r" << reg << ", #" << imm << std::endl;
    }
    else if(opcode == 0x02)     // LOAD register REG[param_high] <- REG[param_low]
    {
        uint16_t src = (params & 0x000F);
        uint16_t dst = (params >> 4) & 0x000F;

        uint16_t val = REG[dst] = REG[src];
        update_flags(val);

        std::cout << std::setbase(10) << "LOAD r" << dst << ", r" << src << std::endl;
    }
    else if(opcode == 0x03)     // LOAD immediate REG[param_high] <- imm16
    {
        uint16_t reg = (params >> 4) & 0x000F; // this wastes the lower 8 bits of the opcode
        uint16_t imm = MEM[PC++];

        uint16_t val = REG[reg] = imm;
        update_flags(val);

        std::cout << std::setbase(10) << "LOAD r" << reg << ", #$" << std::setbase(16) << imm << std::endl;
    }
    else if(opcode == 0x10)     // STORE indirect (REG[param_high]) <- REG[param_low]
    {
        uint16_t reg = (params & 0x000F);
        uint16_t add = (params >> 4) & 0x000F;

        uint16_t val = MEM[REG[add]] = REG[reg];
        update_flags(val);

        std::cout << std::setbase(10) << "STORE (r" << add << "), r" << reg << std::endl;
    }
    else if(opcode == 0x11)     // STORE indirect (REG[param_high]) <- imm4[param_low]
    {
        uint16_t add = (params & 0x000F);
        uint16_t imm = (params >> 4) & 0x000F;

        uint16_t val = MEM[REG[add]] = imm;
        update_flags(val);

        std::cout << std::setbase(10) << "STORE (r" << add << "), " << imm << std::endl;
    }
    else if(opcode == 0x20)     // ADD REG[param_high] <- REG[param_high] + REG[param_low]
    {
        uint16_t reg = (params & 0x000F);
        uint16_t acc = (params >> 4) & 0x000F;

        uint16_t op1 = REG[acc];
        uint16_t op2 = REG[reg];

        uint32_t val = REG[acc] += REG[reg];
        update_flags(val);
        update_flags_arithmetic(val, op1, op2);

        std::cout << std::setbase(10) << "ADD r" << acc << ", r" << reg << std::endl;
    }
    else if(opcode == 0x21)     // ADC REG[param_high] <- REG[param_high] + REG[param_low]
    {
        uint16_t reg = (params & 0x000F);
        uint16_t acc = (params >> 4) & 0x000F;

        if(carry()) ++REG[acc];

        uint16_t op1 = REG[acc];
        uint16_t op2 = REG[reg];

        uint32_t val = REG[acc] += REG[reg];

        update_flags(val);
        update_flags_arithmetic(val, op1, op2);

        std::cout << std::setbase(10) << "ADD r" << acc << ", r" << reg << std::endl;
    }
    else if(opcode == 0x30)     // NOT REG[param_low]
    {
        uint16_t reg = (params & 0x000F);

        uint32_t val = REG[reg] = ~REG[reg];

        update_flags(val);

        std::cout << std::setbase(10) << "NOT r" << reg << std::endl;
    }
    else if(opcode == 0x31)     // AND REG[param_high] <- REG[param_low]
    {
        uint16_t reg = (params & 0x000F);
        uint16_t acc = (params >> 4) & 0x000F;

        uint32_t val = REG[acc] &= REG[reg];

        update_flags(val);

        std::cout << std::setbase(10) << "AND r" << acc << ", r" << reg << std::endl;
    }
    else if(opcode == 0x32)     // OR REG[param_high] <- REG[param_low]
    {
        uint16_t reg = (params & 0x000F);
        uint16_t acc = (params >> 4) & 0x000F;

        uint32_t val = REG[acc] |= REG[reg];

        update_flags(val);

        std::cout << std::setbase(10) << "OR r" << acc << ", r" << reg << std::endl;
    }
    else if(opcode == 0x33)     // XOR REG[param_high] <- REG[param_low]
    {
        uint16_t reg = (params & 0x000F);
        uint16_t acc = (params >> 4) & 0x000F;

        uint32_t val = REG[acc] ^= REG[reg];

        update_flags(val);

        std::cout << std::setbase(10) << "XOR r" << acc << ", r" << reg << std::endl;
    }
    else if(opcode == 0x40)     // CMP REG[param_high], REG[param_low]
    {
        uint16_t reg = (params & 0x000F);
        uint16_t acc = (params >> 4) & 0x000F;

        uint32_t val = (REG[acc] - REG[reg]);

        update_flags(val);
        update_flags_arithmetic(val, REG[acc], REG[reg]);

        std::cout << std::setbase(10) << "CMP r" << acc << ", r" << reg << std::endl;
    }
    else if(opcode == 0x41)     // CMP REG[param_high], imm4[param_low]
    {
        uint16_t imm = (params & 0x000F);
        uint16_t acc = (params >> 4) & 0x000F;

        uint32_t val = (REG[acc] - imm);

        update_flags(val);
        update_flags_arithmetic(val, REG[acc], imm);

        std::cout << std::setbase(10) << "CMP r" << acc << ", " << imm << std::endl;
    }
    else if(opcode == 0x42)     // CMP immediate REG[param_high], imm16
    {
        uint16_t acc = (params >> 4) & 0x000F;
        uint16_t imm = MEM[PC++];

        uint32_t val = (REG[acc] - imm);

        update_flags(val);
        update_flags_arithmetic(val, REG[acc], imm);

        std::cout << std::setbase(10) << "CMP r" << acc << ", #$" << std::setbase(16) << imm << std::endl;
    }
    else if(opcode == 0x50)     // JMPR rel [signed param]
    {
        int16_t rel = (int16_t)params;

        PC += rel;              // JMPR 0 is a nop
        std::cout << "JMPR #$" << std::setbase(16) << rel << std::endl;
    }
    else if(opcode == 0x51)     // JMP abs imm16
    {
        uint16_t abs = MEM[PC++];
        PC = abs;

        std::cout << "JMP" << condition_to_letters(params)
                  << " #$" <<  std::setbase(16) << abs << std::endl;
    }
    else                        // Illegal opcode: halt CPU for now, maybe add trapping later
    {
        std::cout << "Illegal opcode: CPU halted" << std::endl;
        halt();
    }

    return;
}
