#include <iostream>
#include <fstream>
#include "cpu.h"

#define MEM_SIZE 512

int main(int argc, char *argv[])
{
    CPU cpu(MEM_SIZE);

    // Passing a parameter with a RAM image to run
    if(argc > 1) {
        uint8_t *buffer = (uint8_t *)malloc(MEM_SIZE);

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
        free(buffer);
    }
    else {
        // DEBUG: memory image
        uint16_t buffer[] = { 0xffff, 0xf800, 0x00a3, 0xff42, 0xf0ff, 0x0000 };
        cpu.loadmem((uint8_t *)buffer, sizeof(buffer), 0x100);
    }

    cpu.reset();

    while(1) {
        cpu.run_once();

        if(cpu.halted())
            break;
    }

    return 0;
}
