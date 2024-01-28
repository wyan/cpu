#include <iostream>
#include <iomanip>
#include "cpu.h"

void CPU::reset() {
    FLAGS = 0;
    SP = SPX = 0;
    PC = 0x100;        // Start address for code

    return;
}

void CPU::loadmem(uint16_t *buffer, const uint16_t size, const uint16_t start) {
    const uint16_t size_norm = (size > mem_size - start) ? mem_size - start : size;
    memcpy((MEM + start), buffer, size_norm);
}

void CPU::dump() const {
    std::cout << std::right << std::setbase(16) << std::noshowbase << std::setfill('0');
    for(int i = 0; i < mem_size; i++) {
        if(i % 16 == 0) std::cout << std::setw(4) << i << " : ";
        std::cout << std::setw(2) << (static_cast<uint16_t>(MEM[i]) & 0xFF) << " ";
        if(i % 16 == 15) std::cout << std::endl;
    }
    std::cout << std::endl;
}

void CPU::register_dump() const {
    for(int i = 0; i < 16; i++) {
        std::cout << "r" << std::setbase(10) << std::setw(2) << i << " = "
                  << std::right << std::setbase(16) << std::noshowbase << std::setfill('0') << std::setw(4)
                  << REG[i] << " ";
        if((i % 4) == 3) std::cout << std::endl;
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
              << "params=" << std::setw(2) << params << std::endl;

    // Execute
    std::cout << "Running : ";
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

        REG[reg] = MEM[REG[add]];

        std::cout << std::setbase(10) << "LOAD r" << reg << ", (r" << add << ")" << std::endl;
    }
    else if(opcode == 0x01)     // LOAD immediate REG[param_high] <- imm4[param_low]
    {
        uint16_t imm = (params & 0x000F);
        uint16_t reg = (params >> 4) & 0x000F;

        REG[reg] = imm;

        std::cout << std::setbase(10) << "LOAD r" << reg << ", " << imm << std::endl;
    }
    else if(opcode == 0x02)     // LOAD register REG[param_high] <- REG[param_low]
    {
        uint16_t src = (params & 0x000F);
        uint16_t dst = (params >> 4) & 0x000F;

        REG[dst] = REG[src];

        std::cout << std::setbase(10) << "LOAD r" << dst << ", r" << src << std::endl;
    }
    else if(opcode == 0x10)     // STORE indirect (REG[param_low]) <- REG[param_high]
    {
        uint16_t add = (params & 0x000F);
        uint16_t reg = (params >> 4) & 0x000F;

        MEM[REG[add]] = REG[reg];

        std::cout << std::setbase(10) << "STORE (r" << add << "), r" << reg << std::endl;
    }
    else if(opcode == 0x20)     // ADD REG[param_low] <- REG[param_low] + REG[param_high]
    {
        uint16_t acc = (params & 0x000F);
        uint16_t reg = (params >> 4) & 0x000F;

        REG[acc] += REG[reg];
        std::cout << std::setbase(10) << "ADD r" << acc << ", r" << reg << std::endl;
    }
    else                        // Illegal opcode: halt CPU for now, maybe add trapping later
    {
        std::cout << "Illegal opcode: CPU halted" << std::endl;
        halt();
    }

    return;
}
