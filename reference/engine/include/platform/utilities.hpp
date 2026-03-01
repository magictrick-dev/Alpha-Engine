#pragma once
#include <cstdint>
#include <string_view>

typedef uint64_t timestamp_t;

timestamp_t pde_utilities_get_timestamp();
double pde_utilities_get_time_difference(timestamp_t start, timestamp_t end);

namespace TypeID
{

    template <typename T> constexpr std::string_view Name()
    {

#       if defined(__clang__)

            constexpr std::string_view func = __PRETTY_FUNCTION__;
            constexpr std::string_view prefix = "std::string_view TypeID::Name() [T = ";
            constexpr std::string_view suffix = "]";

            constexpr size_t start = func.find(prefix);
            static_assert(start != std::string_view::npos, "Prefix not found in __PRETTY_FUNCTION__");

            constexpr size_t type_start = start + prefix.size();
            constexpr size_t type_end = func.find(suffix, type_start);
            static_assert(type_end != std::string_view::npos, "Suffix not found in __PRETTY_FUNCTION__");

            return func.substr(type_start, type_end - type_start);

#       else
#           error "Unsupported compiler."
#       endif

    }

    template <typename T> constexpr size_t Hash()
    {

        // NOTE(Chris): FNV1A-64 hashing algorithm, works fine since total hashes is limitted.
        constexpr std::string_view type_name = Name<T>();
        constexpr size_t length = type_name.length();
        constexpr const char *string = type_name.data();

        uint64_t hash = 14695981039346656037ull;
        for (std::size_t i = 0; i < length; ++i)
        {
            hash = (hash ^ static_cast<uint64_t>(string[i])) * 1099511628211ull;
        }

        return hash;

    }

    template <typename... Ts> constexpr std::array<size_t, sizeof...(Ts)> HashList()
    {
        return { Hash<Ts>()... };
    }

};
