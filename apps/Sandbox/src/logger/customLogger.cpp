#include "customLogger.hpp"
#include <time.h>
#include "spdlog/spdlog.h"
#include "imgui.h"

enum CustomLogLevel {
    iLOG_ALL = 0,        // Display all logs
    iLOG_TRACE,          // Trace logging, intended for internal use only
    iLOG_DEBUG,          // Debug logging, used for internal debugging, it should be disabled on release builds
    iLOG_INFO,           // Info logging, used for program execution info
    iLOG_WARNING,        // Warning logging, used on recoverable failures
    iLOG_ERROR,          // Error logging, used on unrecoverable failures
    iLOG_FATAL,          // Fatal logging, used to abort program: exit(EXIT_FAILURE)
    iLOG_NONE            // Disable logging
};

static bool s_is_set = false;
void CustomLogger(int log_level, const char* text, va_list args)
{
    if (!s_is_set)
    {
        //for additional formatting, see https://github.com/gabime/spdlog/wiki/Custom-formatting#pattern-flags
        spdlog::set_pattern("[%^%4!l%$] %v"); 
        s_is_set = true;
    }

    ImGuiTextBuffer buff;
    buff.appendfv(text, args);
    switch (log_level)
    {
    case iLOG_INFO: spdlog::info(buff.c_str()); break;
    case iLOG_ERROR: spdlog::error(buff.c_str()); break;
    case iLOG_WARNING: spdlog::warn(buff.c_str()); break;
    case iLOG_DEBUG: spdlog::debug(buff.c_str()); break;
    default:
        spdlog::info(buff.c_str());
        break;
    }
}
