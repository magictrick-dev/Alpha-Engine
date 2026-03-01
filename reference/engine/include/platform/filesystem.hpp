#pragma once
#include <cstdint>
#include <string>
#include <vector>

bool pde_filesystem_path_exists(std::string path);
bool pde_filesystem_path_exists_as_file(std::string path);
bool pde_filesystem_path_exists_as_folder(std::string path);
bool pde_filesystem_path_is_relative(std::string path);
bool pde_filesystem_path_is_absolute(std::string path);
bool pde_filesystem_file_is_ready(std::string path);
size_t pde_filesystem_file_size(std::string path);
size_t pde_filesystem_file_time(std::string path);
std::string pde_filesystem_canonicalize(std::string path);
std::string pde_filesystem_resolve(std::string path);
std::string pde_filesystem_get_executable_directory();
std::string pde_filesystem_get_current_working_directory();
std::vector<uint8_t> pde_filesystem_read_entire_file(std::string path);

