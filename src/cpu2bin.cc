#include <fstream>
#include <iostream>

#include "tools.h"

/* Converts a .cpu file containing a list of hex words to binary */
int main(int argc, char **argv)
{
    if(argc != 3) {
        std::cout << "Usage: cpu2bin input.cpu output.bin" << std::endl;
        return 1;
    }

    std::string input_filename(argv[1]);
    std::string output_filename(argv[2]);

    const uint16_t buffer_size = 65535;
    uint16_t buffer[buffer_size];

    uint16_t bytes_read = load_file_text(input_filename, buffer, buffer_size);
    if(!bytes_read){
        std::cerr << "Could not read " << input_filename << std::endl;
        return 1;
    }

    std::ofstream output_file(output_filename, std::ios::binary);
    if(!output_file) {
        std::cerr << "Could not open " << output_filename << " for writing" << std::endl;
        return 1;
    }

    auto initial_position = output_file.tellp();
    output_file.write(reinterpret_cast<const char*>(buffer), bytes_read);
    auto final_position = output_file.tellp();

    std::streamsize bytes_written = final_position - initial_position;
    std::cout << "Wrote " << bytes_written << " bytes to " << output_filename << std::endl;

    output_file.close();
    return 0;
}
