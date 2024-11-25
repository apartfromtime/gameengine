#pragma once
#include <SDL3\SDL.h>
#include <string>

namespace gameErrorNS
{
    // Error codes
    // Negative numbers are fatal errors that may require the game to be shutdown.
    // Positive numbers are warnings that do not require the game to be shutdown.
    const int FATAL_ERROR = -1;
    const int WARNING = 1;
}

// Game Error class. Thrown when an error is detected by the game engine.
inline void GameError(int code, const char* format, ...)
{
    va_list argptr;
    char text[256];

    va_start(argptr, format);
    vsnprintf(text, sizeof(text), format, argptr);
    va_end(argptr);

    switch (code)
    {
    case gameErrorNS::FATAL_ERROR:
    {
        SDL_LogError(SDL_LOG_PRIORITY_ERROR, text);
    } break;
    case gameErrorNS::WARNING:
    {
        SDL_LogWarn(SDL_LOG_PRIORITY_WARN, text);
    } break;
    }
}