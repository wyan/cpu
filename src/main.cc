#include <iostream>
#include <fstream>
#include <regex>
#include "cpu.h"

#define MEM_SIZE 4096

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

    std::cout << "Custom 16-bit ISA CPU Emulator" << std::endl;
    std::cout << "(c) Alice Wyan, 2024" << std::endl << std::endl;

    // Passing a parameter with a RAM image to run
    if(argc > 1) {
        load_file_into_memory(cpu, argv[1]);
        std::cout << "Loaded " << argv[1] << "at memory address 0x0100" << std::endl;
    }
    else {
        cpu.loadmem(default_program, sizeof(default_program), 0x100);
        std::cout << "Loaded default program at memory address 0x0100" << std::endl;
    }

    cpu.reset();
    std::cout << "CPU RESET" << std::endl << std::endl;

    while(1) {
        std::string cmd;
        std::cout << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << cpu.getPC() << " > ";
        std::getline(std::cin, cmd);

        std::regex cmd_pattern("^([qxrng?])\\s?(.*)$");
        std::smatch m;

        if(std::regex_match(cmd, m, cmd_pattern)) {
            if(m[1] == "q") {
                return 0;
            }
            else if(m[1] == "x") {
                cpu.dump_registers();
                cpu.dump_flags();
            }
            else if(m[1] == "r") { cpu.reset(); }
            else if(m[1] == "n") { cpu.run_once(); }
            else if(m[1] == "g") {
                while(!cpu.halted())
                    cpu.run_once();
            }
            else if(m[1] == "?") {
                std::cout <<
                    "    q - quit\n" <<
                    "    x - dump registers & flags\n" <<
                    "    r - CPU reset\n" <<
                    "    n - run next instruction\n" <<
                    "    g - go (run until HALT)\n" <<
                    "    ? - this help\n";
            }
            else {
                std::cout << "Command not recognised: " << m[1] << std::endl;
            }
        }
    }

    return 0;
}
