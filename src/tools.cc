#include <fstream>
#include <iostream>
#include <regex>

#include <filesystem>
namespace fs = std::filesystem;

#include "tools.h"

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

    std::string line;
    std::smatch match;
    uint16_t words_read = 0;

    while(std::getline(file, line)) {
        // Skip comments
        if (!line.empty() && line[0] == ';') continue;
        if(line.size() > 2 && line.substr(0, 2) == "//") continue;

        // Search for a hexadecimal constant at the beginning of the line
        if (std::regex_search(line, match, std::regex("((?:0x)?[0-9A-Fa-f]+)"))) {
            std::string hex_str = match[0];
            uint16_t value = std::stoi(hex_str, nullptr, 16);

            if(words_read > buffer_size) {
                return 0;       // Signal error on overflow
            } else {
                buffer[words_read++] = value;
            }

            // Process the hexadecimal value, here we just print it
            std::cout << "Hexadecimal: " << hex_str << " -> Decimal: " << value << std::endl;
        }
    }

    return words_read * 2; // expected: bytes read
}

uint16_t load_file(const std::string filename, uint16_t *buffer, uint16_t buffer_size)
{
    std::string::size_type idx = filename.rfind('.');
    std::string file_extension = (idx != std::string::npos) ? filename.substr(idx) : "";

    if(file_extension == ".cpu") {
        return load_file_text(filename, buffer, buffer_size);
    } else {
        return load_file_binary(filename, buffer, buffer_size);
    }
}
