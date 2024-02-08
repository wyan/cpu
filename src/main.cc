#include <iostream>
#include <fstream>
#include <regex>
#include <filesystem>

namespace fs = std::filesystem;

#include "cpu.h"
#include "tools.h"

#define MEM_SIZE 4096


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
        filesize = load_file(argv[1], buffer, MEM_SIZE);
        if(filesize) {
            cpu.loadmem(buffer, filesize, location);
            std::cout << "Loaded " << filesize << " bytes"
                      << " from " << argv[1] << " at memory address "
                      << std::hex << std::setw(4) << std::setfill('0') << std::uppercase
                      << location << std::endl;
        } else {
            std::cout << "Could not load file" << std::endl;
        }
    }

    cpu.reset();

    while(1) {
        std::string cmd;

        std::cout << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << cpu.getPC() << " > ";

        if(!std::getline(std::cin, cmd)) {
            if (std::cin.eof()) {
                cmd = "q";
            }
        }

        std::regex cmd_pattern("^([a-zA-Z!?])\\s?(.*)$");
        std::smatch m;

        // Parse command: if it starts with "!" run shell command
        if(cmd[0] == '!') {
            system(cmd.substr(1).c_str());
            continue;
        }

        // Otherwise parse emulator command
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
                    bytes_read = load_file(filename, buffer, MEM_SIZE);
                    if(bytes_read) {
                        cpu.loadmem(buffer, bytes_read, location);
                        std::cout << "Loaded " << bytes_read << " bytes"
                                  << " from " << filename << " at memory address "
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
            else if(m[1] == "t") {
                cpu.toggle_tracing();
                std::cout << "Tracing " << (cpu.tracing() ? "on" : "off") << std::endl;
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
            else if(m[1] == "R") {
                cpu.reset();
                std::cout << "CPU reset" << std::endl;
            }
            else if(m[1] == "?") {
                std::cout <<
                    "    d [m [v]] - deposit values into memory\n" <<
                    "    g         - go (run until HALT)\n" <<
                    "    l f [m]   - load file f in memory position m (0x0100 if not specified)" <<
                    "    n         - run next instruction\n" <<
                    "    p [m]     - deposit the value m into the PC register (0x0100 if not specified) \n" <<
                    "    q         - quit emulator\n" <<
                    "    r         - dump CPU flags and register file\n" <<
                    "    t         - toggle instruction tracing\n" <<
                    "    x [m]     - examine memory at position m (PC if not specified)\n" <<
                    "    R         - perform a CPU reset\n" <<
                    "    ?         - this help\n";
            }
            else { std::cout << "Command not recognised" << std::endl; }
        }
    }

    return 0;
}
