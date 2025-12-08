#include "script.hpp"
#include "bindings.hpp"
#include <raylib.h> //gotta get the logger

void jam::scripting::Script::Initialize()
{
    BindAll(state);
    this->enabled = true;
}

bool jam::scripting::Script::LoadFile(std::string filepath)
{
    if (!enabled)
        Initialize();
    try {
        sol::protected_function_result executable = state.safe_script_file(filepath);
        if (executable.valid())
        {
            self = executable;
        }
    }
    catch (const sol::error& e)
    {
        catch_sol_error(e);
        return false;
    }

    return true;
}

bool jam::scripting::Script::ExecuteCommand(std::string command, char* outBuf)
{
    try
    {
        state.safe_script(command);
        return true;
    }
    catch (const sol::error& e)
    {
        catch_sol_error(e);
        if (outBuf)
            TextCopy(outBuf, e.what());
        return false;
    }
}

void jam::scripting::catch_sol_error(const sol::error& error)
{
    TraceLog(TraceLogLevel::LOG_ERROR, "[Script]\t%s", error.what());
}
