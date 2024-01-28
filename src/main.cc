#include <iostream>
#include <fstream>
#include "cpu.h"

#define MEM_SIZE 512

int main(int argc, char *argv[])
{
    CPU cpu(MEM_SIZE);

    // Passing a parameter with a RAM image to run
    if(argc > 1) {
        uint16_t *buffer = new uint16_t[MEM_SIZE];

        std::string filename { argv[1] };
        std::ifstream fin(filename, std::ios::in | std::ios::binary );
        if(fin.fail()) {
            std::cout << "Could not read file " << filename << " for input" << std::endl;
            exit(1);
        }
        fin.read((char *)buffer, MEM_SIZE);
        uint16_t read_bytes = fin.gcount();

        std::cout << "Read " << read_bytes << " bytes from " << filename << std::endl;
        cpu.loadmem(buffer, read_bytes, 0);
        cpu.dump();

        fin.close();
        delete[] buffer;
    }
    else {
        // DEBUG: memory image
        uint16_t buffer[] = {
            0xffff,    // NOP
            0x00a3,    // LOAD r10, (r3)
            0x0142,    // LOAD r4, #2
            0x0153,    // LOAD r5, #3
            0x2054,    // ADD r4, r5
            0xf800,    // HALT
            0xf0ff,
            0x0000
        };
        cpu.loadmem(buffer, sizeof(buffer), 0x100);
    }

    cpu.reset();

    while(1) {
        cpu.run_once();
        cpu.register_dump();

        if(cpu.halted())
            break;
    }

    return 0;
}
