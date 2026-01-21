#pragma once
#include <stdint.h>
#include <assert.h>

#define ALPHA_ASSERTIONS        1
#define ALPHA_ENGINE_CONSOLE    1
#define GLM_ENABLE_EXPERIMENTAL 1

#if defined(ALPHA_ENGINE_CONSOLE) && ALPHA_ENGINE_CONSOLE != 0
#   include <stdio.h>
#   include <iostream>
#   include <string>
#   include <sstream>
#endif

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtx/io.hpp>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

#if defined(ALPHA_ASSERTIONS)
#   define ALPHA_ASSERT(stm) assert((stm))
#   define ALPHA_ASSERT_POINTER(ptr) assert((ptr) != NULL)
#   define ALPHA_ASSERT_NO_IMPLEMENTATION() assert(!"No implementation defined.")
#   define ALPHA_ASSERT_NONE_REACHABLE_CONDITION() assert(!"Non-reachable condition hit.")
#else
#   define ALPHA_ASSERT(stm) 
#   define ALPHA_ASSERT_POINTER(ptr) 
#   define ALPHA_ASSERT_NO_IMPLEMENTATION() 
#   define ALPHA_ASSERT_NONE_REACHABLE_CONDITION() 
#endif

namespace TypeID
{

    template <typename T> 
    constexpr std::string_view Name()
    {
#       if defined(__clang__)

            constexpr std::string_view func = __PRETTY_FUNCTION__;
            constexpr std::string_view prefix = "std::string_view TypeID::Name() [T = ";
            constexpr std::string_view postfix = "]";

            constexpr size_t start = func.find(prefix);
            static_assert(start != std::string_view::npos, "Prefix not found.");

            constexpr size_t type_start = start + prefix.size();
            constexpr size_t type_end = func.find(postfix, type_start);
            static_assert(type_end != std::string_view::npos, "Suffix not found.");

            return func.substr(type_start, type_end - type_start);

#       else
#           error "Unsupported compiler."
#       endif
    }

    

};

