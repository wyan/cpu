#include <iostream>
#include <fstream>
#include <regex>
#include <filesystem>

namespace fs = std::filesystem;

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


// Returns bytes_read if everything went right, 0 if something went wrong
uint16_t load_file_binary(const std::string filename, uint16_t *buffer, uint16_t buffer_size)
{
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if(file.fail()) return false;

    uint16_t filesize = fs::file_size(filename);
    if(filesize > buffer_size) return false;

    file.read((char *)buffer, buffer_size);

    uint16_t bytes_read = file.gcount();
    if(bytes_read != filesize) return false;

    return bytes_read;
}

// Returns bytes_read (after conversion) if everything went right, 0 if something went wrong
uint16_t load_file_text(const std::string filename, uint16_t *buffer, uint16_t buffer_size)
{
    std::ifstream file(filename, std::ios::in);
    if(file.fail()) return false;

}

int main(int argc, char *argv[])
{
    CPU cpu(MEM_SIZE);

    std::cout << "Custom 16-bit ISA CPU Emulator" << std::endl;
    std::cout << "(c) Alice Wyan, 2024" << std::endl << std::endl;

    // Passing a parameter with a RAM image to run
    if(argc > 1) {
        uint16_t buffer[MEM_SIZE];
        uint16_t location = 0x100;
        uint16_t filesize;

        if(argc > 2) location = std::stoi(argv[2], nullptr, 16);
        // load_file_into_memory(cpu, argv[1], location);
        filesize = load_file_binary(argv[1], buffer, MEM_SIZE);
        if(filesize) {
            cpu.loadmem(buffer, filesize, location);
            std::cout << "Loaded " << argv[1] << "at memory address " << location << std::endl;
        } else {
            std::cout << "Could not load file" << std::endl;
        }
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

        if(!std::getline(std::cin, cmd)) {
            if (std::cin.eof()) {
                cmd = "q";
            }
        }

        std::regex cmd_pattern("^([a-z!?])\\s?(.*)$");
        std::smatch m;

        if(std::regex_match(cmd, m, cmd_pattern)) {
            if(m[1] == "q") {
                return 0;
            }
            else if(m[1] == "d") { std::cout << "Not implemented yet" << std::endl; }
            else if(m[1] == "g") {
                while(!cpu.halted())
                    cpu.run_once();
            }
            else if(m[1] == "l") {
                std::smatch f;
                std::string args(m[2]);
                // Match filename (possibly containing escaped spaces)
                if(std::regex_match(args, f, std::regex("^((?:\\\\[ ]|[^ ])+)(?:\\s+(.*))?$"))) {
                    std::string filename(f[1]);
                    std::string loc_str(f[2]);
                    uint16_t location;

                    if(loc_str.empty()) {
                        location = 0x100;
                    } else {
                        try {
                            location = std::stoi(loc_str, nullptr, 16);
                        } catch(std::invalid_argument const &e) {
                            std::cerr << "Could not parse location: " << loc_str << std::endl;
                            continue;
                        } catch (std::out_of_range const &e) {
                            std::cerr << "Location out of range: " << loc_str << std::endl;
                            continue;
                        }
                    }

                    uint16_t buffer[MEM_SIZE];
                    uint16_t bytes_read;
                    bytes_read = load_file_binary(filename, buffer, MEM_SIZE);
                    if(bytes_read) {
                        cpu.loadmem(buffer, bytes_read, location);
                        std::cout << "Loaded " << filename << " at address "
                                  << std::hex << std::setw(4) << std::setfill('0')
                                  << location << std::endl;
                    } else {
                        std::cout << "Could not read file" << std::endl;
                    }
                }
            }
            else if(m[1] == "p") { std::cout << "Not implemented yet" << std::endl; }
            else if(m[1] == "n") { cpu.run_once(); }
            else if(m[1] == "r") {
                cpu.dump_flags();
                cpu.dump_registers();
            }
            else if(m[1] == "x") {
                std::string arg(m[2]);
                uint16_t location;

                if(arg.empty()) {
                    location = cpu.getPC();
                } else {
                    location = std::stoi(arg, nullptr, 16);
                }

                location = (location >> 4) << 4; // round to greatest multiple of 16 lower than location

                for(uint16_t i = 0; i < 4; i++) {
                    std::cout << std::hex << std::setw(4) << std::setfill('0') << (location + 16 * i) << " :  ";
                    for(uint16_t j = 0; j < 16; j++) {
                        std::cout << std::setw(4) <<cpu.getmem_at(location + 16 * i + j) << " ";
                    }
                    std::cout << std::endl;
                }
            }
            else if(m[1] == "!") { cpu.reset(); }
            else if(m[1] == "?") {
                std::cout <<
                    "    d [m [v]] - deposit values into memory\n" <<
                    "    g         - go (run until HALT)\n" <<
                    "    l f [m]   - load file f in memory position m (0x0100 if not specified)" <<
                    "    n         - run next instruction\n" <<
                    "    p [m]     - deposit the value m into the PC register (0x0100 if not specified) \n" <<
                    "    q         - quit emulator\n" <<
                    "    r         - dump CPU flags and register file\n" <<
                    "    x [m]     - examine memory at position m (PC if not specified)\n" <<
                    "    !         - perform a CPU reset\n" <<
                    "    ?         - this help\n";
            }
            else { std::cout << "Command not recognised" << std::endl; }
        }
    }

    return 0;
}
