#ifndef TOOLS_H_
#define TOOLS_H_

#include <cstdint>
#include <string>

uint16_t load_file_binary(const std::string filename, uint16_t *buffer, uint16_t buffer_size);
uint16_t load_file_text(const std::string filename, uint16_t *buffer, uint16_t buffer_size);
uint16_t load_file(const std::string filename, uint16_t *buffer, uint16_t buffer_size);

#endif // TOOLS_H_
