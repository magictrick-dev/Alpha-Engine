#include <platform/filesystem.hpp>
#include <filesystem>
#include <fstream>
#include <cassert>
#if defined(_WIN32)
#   include <windows.h>
#   include <shlwapi.h>
#endif

bool
pde_filesystem_path_exists(std::string path)
{
    const bool exists = std::filesystem::exists(path);
    return exists;
}

bool
pde_filesystem_path_exists_as_file(std::string path)
{
    const bool result = std::filesystem::is_regular_file(path);
    return result;
}

bool
pde_filesystem_path_exists_as_folder(std::string path)
{
    const bool result = std::filesystem::is_directory(path);
    return result;
}

bool
pde_filesystem_path_is_relative(std::string path)
{
    const bool result = std::filesystem::path(path).is_relative();
    return result;
}

bool
pde_filesystem_path_is_absolute(std::string path)
{
    const bool result = std::filesystem::path(path).is_absolute();
    return result;
}

bool 
pde_filesystem_file_is_ready(std::string path)
{

#   if defined(_WIN32)

    HANDLE file_handle = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file_handle == INVALID_HANDLE_VALUE)
        return false;
    else
    {
        CloseHandle(file_handle);
        return true;
    }
    
#   else
#       pragma error "Undefined platform implementation."
#   endif

}

size_t
pde_filesystem_file_size(std::string path)
{
    const size_t file_size = std::filesystem::file_size(path);
    return file_size;
}

size_t
pde_filesystem_file_time(std::string path)
{

#   if defined(_WIN32)

    HANDLE file_handle = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file_handle == INVALID_HANDLE_VALUE)
        return 0;

    FILETIME last_write = {};
    GetFileTime(file_handle, NULL, NULL, &last_write);

    LARGE_INTEGER quad_cast = {};
    quad_cast.LowPart = last_write.dwLowDateTime;
    quad_cast.HighPart = last_write.dwHighDateTime;

    CloseHandle(file_handle);

    return ((size_t)quad_cast.QuadPart);

#   else
#       pragma error "Undefined platform implementation."
#   endif

}

std::string
pde_filesystem_canonicalize(std::string path)
{
    std::string result = std::filesystem::weakly_canonical(path).string();
    return result;
}

std::string
pde_filesystem_resolve(std::string path)
{

    if (pde_filesystem_path_is_absolute(path))
    {
        const bool exists = pde_filesystem_path_exists_as_file(path);
        if (exists) return path;
        else return "";
    }

    std::string executable_directory = pde_filesystem_get_executable_directory();

    // NOTE(Chris): In most cases, all paths and resources relative directly in the same
    //              location as the executable for release builds, since resources and such
    //              are resolved in the same directory after bundling.
    std::string location_a = pde_filesystem_canonicalize(executable_directory + "\\" + path);
    if (pde_filesystem_path_exists_as_file(location_a)) return location_a;

    // NOTE(Chris): Generally if the executable directory is located in ./bin for development builds.
    std::string location_b = pde_filesystem_canonicalize(executable_directory + "\\..\\" + path);
    if (pde_filesystem_path_exists_as_file(location_b)) return location_b;

    return "";

}

std::string
pde_filesystem_get_executable_directory()
{

#   if defined(_WIN32)
        static char buffer[MAX_PATH];
        static bool initialized = false;
        if (initialized == false)
        {

            DWORD result = GetModuleFileNameA(NULL, buffer, MAX_PATH);
            assert(result > 0 && result < MAX_PATH);

            BOOL success = PathRemoveFileSpecA(buffer);
            assert(success);
            initialized = true;

        }

        return buffer;
#   else
#       pragma error "Undefined platform implementation."
#   endif

}

std::string
pde_filesystem_get_current_working_directory()
{

#   if defined(_WIN32)
        static char buffer[MAX_PATH];
        static bool initialized = false;
        if (initialized == false)
        {

            DWORD result = GetCurrentDirectoryA(MAX_PATH, buffer);
            assert(result != 0);
            assert(result < MAX_PATH);

            initialized = true;

        }

        return buffer;
#   else
#       pragma error "Undefined platform implementation."
#   endif

}

std::vector<uint8_t>
pde_filesystem_read_entire_file(std::string path)
{

    if (!pde_filesystem_path_exists_as_file(path)) return {};

    size_t size = pde_filesystem_file_size(path);

    std::ifstream stream(path, std::ios::binary);
    if (!stream.is_open()) return {};

    std::vector<uint8_t> result(size);
    stream.read((char*)result.data(), size);
    stream.close();

    return result;

}

