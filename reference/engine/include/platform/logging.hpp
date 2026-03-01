#pragma once
#include <cstdint>
#include <cassert>
#include <string>
#include <vector>

enum MessageOriginType : uint32_t
{
    MessageOriginType_Internal,
    MessageOriginType_Platform,
    MessageOriginType_Engine,
    MessageOriginType_Resource,
    MessageOriginType_Audio,
};

inline std::string to_string(MessageOriginType type)
{
    switch (type)
    {
        case MessageOriginType_Internal: return "Internal";
        case MessageOriginType_Platform: return "Platform";
        case MessageOriginType_Engine: return "Engine";
        case MessageOriginType_Resource: return "Resource";
        case MessageOriginType_Audio: return "Audio";
        default: assert(!"Unimplemented string conversion.");
    }
}

enum MessageSeverityType : uint32_t
{
    MessageSeverityType_Debug,
    MessageSeverityType_Info,
    MessageSeverityType_Warning,
    MessageSeverityType_Critical,
    MessageSeverityType_Error,
};

inline std::string to_string(MessageSeverityType type)
{
    switch (type)
    {
        case MessageSeverityType_Debug: return "Debug";
        case MessageSeverityType_Info: return "Info";
        case MessageSeverityType_Warning: return "Warning";
        case MessageSeverityType_Critical: return "Critical";
        case MessageSeverityType_Error: return "Error";
        default: assert(!"Unimplemented string conversion.");
    }
}

struct MessageLog
{
    MessageOriginType origin;
    MessageSeverityType severity;
    std::string message;
};

void pde_logging_set_limit(uint32_t limit);
void pde_logging_add_message(MessageLog log);
void pde_logging_add_message(MessageOriginType origin, MessageSeverityType severity, std::string message);
std::vector<MessageLog> pde_logging_get_logs();

inline void pde_logging_engine_debug(std::string message)
    { pde_logging_add_message(MessageOriginType_Engine, MessageSeverityType_Debug, message); }
inline void pde_logging_engine_info(std::string message)
    { pde_logging_add_message(MessageOriginType_Engine, MessageSeverityType_Info, message); }
inline void pde_logging_engine_warning(std::string message)
    { pde_logging_add_message(MessageOriginType_Engine, MessageSeverityType_Warning, message); }
inline void pde_logging_engine_critical(std::string message)
    { pde_logging_add_message(MessageOriginType_Engine, MessageSeverityType_Critical, message); }
inline void pde_logging_engine_error(std::string message)
    { pde_logging_add_message(MessageOriginType_Engine, MessageSeverityType_Critical, message); }

inline void pde_logging_resource_debug(std::string message)
    { pde_logging_add_message(MessageOriginType_Resource, MessageSeverityType_Debug, message); }
inline void pde_logging_resource_info(std::string message)
    { pde_logging_add_message(MessageOriginType_Resource, MessageSeverityType_Info, message); }
inline void pde_logging_resource_warning(std::string message)
    { pde_logging_add_message(MessageOriginType_Resource, MessageSeverityType_Warning, message); }
inline void pde_logging_resource_critical(std::string message)
    { pde_logging_add_message(MessageOriginType_Resource, MessageSeverityType_Critical, message); }
inline void pde_logging_resource_error(std::string message)
    { pde_logging_add_message(MessageOriginType_Resource, MessageSeverityType_Critical, message); }

inline void pde_logging_audio_debug(std::string message)
    { pde_logging_add_message(MessageOriginType_Audio, MessageSeverityType_Debug, message); }
inline void pde_logging_audio_info(std::string message)
    { pde_logging_add_message(MessageOriginType_Audio, MessageSeverityType_Info, message); }
inline void pde_logging_audio_warning(std::string message)
    { pde_logging_add_message(MessageOriginType_Audio, MessageSeverityType_Warning, message); }
inline void pde_logging_audio_critical(std::string message)
    { pde_logging_add_message(MessageOriginType_Audio, MessageSeverityType_Critical, message); }
inline void pde_logging_audio_error(std::string message)
    { pde_logging_add_message(MessageOriginType_Audio, MessageSeverityType_Critical, message); }
