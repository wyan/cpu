#include <iostream>
#include <fstream>
#include "cpu.h"

#define MEM_SIZE 512

const uint16_t default_program[] = {
    0xffff,    // 0100 NOP
    0x00a3,    // 0101 LOAD r10, (r3)
    0x0142,    // 0102 LOAD r4, #2
    0x0350,    // 0103 LOAD r5, #$FF08
    0xFF08,
    0x2054,    // 0105 ADD r5, r4
    0x3004,    // 0106 NOT r4
    0x3145,    // 0107 AND r4, r5
    0x3345,    // 0108 XOR r4, r5
    0x4044,    // 0109 CMP r4, r5
    0x5002,    // 010A JMPR +2
    0xffff,    // 010B NOP
    0xffff,    // 010C NOP
    0x5100,    // 010D JMP #0x0110
    0x0110,
    0xffff,    // 010F NOP      ; skipped by previous jump
    0x5000,    // 0110 JMPR 0   ; equivalent to a NOP
    // ...
    0xf800,    // HALT
    0xf0ff,    // ;; Invalid instruction, should not get here
    0x0000
};

void load_file_into_memory(CPU & cpu, const std::string filename)
{
    uint16_t *buffer = new uint16_t[MEM_SIZE];

    std::ifstream fin(filename, std::ios::in | std::ios::binary );
    if(fin.fail()) {
        std::cout << "Could not read file " << filename << " for input" << std::endl;
        exit(1);
    }
    fin.read((char *)buffer, MEM_SIZE);
    uint16_t read_bytes = fin.gcount();

    std::cout << "Read " << read_bytes << " bytes from " << filename << std::endl;
    cpu.loadmem(buffer, read_bytes, 0);
    cpu.dump_memory();

    fin.close();
    delete[] buffer;
}

int main(int argc, char *argv[])
{
    CPU cpu(MEM_SIZE);

    // Passing a parameter with a RAM image to run
    if(argc > 1) {
        load_file_into_memory(cpu, argv[1]);
    }
    else {
        cpu.loadmem(default_program, sizeof(default_program), 0x100);
    }

    cpu.reset();

    while(1) {
        cpu.run_once();
        cpu.dump_registers();
        cpu.dump_flags();

        if(cpu.halted())
            break;
    }

    return 0;
}
