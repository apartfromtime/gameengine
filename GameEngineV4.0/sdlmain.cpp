#include <SDL3\sdl.h>
#include "game.h"

void TranslateAndDispatchEvent(const SDL_Event* msg);
bool CreateMainWindow(SDL_Window** window, int show);

static Game* s_game = NULL;
static SDL_Window* s_hwnd = NULL;

//=============================================================================
// Starting point for application
//=============================================================================
int main(int argc, const char* argv[])
{
    SDL_Event msg = {};

    // Create the game, sets up message handler
    s_game = new NullGame();

    // Create the window
    if (!CreateMainWindow(&s_hwnd, 1))
    {
        return 1;
    }

    try
    {
        s_game->initialize(s_hwnd);

        SDL_StartTextInput(s_hwnd);

        // main message loop
        int done = 0;

        while (!done)
        {
            SDL_PumpEvents();

            if (SDL_PeepEvents(&msg, 1, SDL_GETEVENT, SDL_EVENT_FIRST,
                SDL_EVENT_LAST) > 0)
            {
                if (msg.type == SDL_EVENT_QUIT)
                {
                    done = 1;
                }

                TranslateAndDispatchEvent(&msg);
            }
            else
            {
                s_game->run();          // run the game loop
            }
        }
    }
    catch (const std::runtime_error& err)
    {
        SDL_DestroyWindow(s_hwnd);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
            err.what(), NULL);
    }
    catch (...)
    {
        SDL_DestroyWindow(s_hwnd);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
            "Error", "Unknown error occured in game.", NULL);
    }

    SDL_StopTextInput(s_hwnd);
    s_game->shutdown();
    SAFE_DELETE(s_game);          // free memory before exit

    return 0;
}

//=============================================================================
// Window event callback function
//=============================================================================
static void SDLProc(const SDL_Window* hwnd, const SDL_Event* event)
{
    if (s_game != NULL)
    {
        s_game->messageHandler(hwnd, event);
    }
}

//=============================================================================
// Translate virtual key codes
//=============================================================================
void TranslateAndDispatchEvent(const SDL_Event* msg)
{
    if (msg != NULL)
    {
        SDLProc(s_hwnd, msg);
    }
}

//=============================================================================
// Places a window close request
//=============================================================================
void ExitMainWindow()
{
    SDL_Event event = { 0 };

    event.window.type = SDL_EVENT_WINDOW_CLOSE_REQUESTED;
    event.window.timestamp = SDL_GetTicksNS();
    event.window.windowID = SDL_GetWindowID(s_hwnd);

    SDL_PushEvent(&event);
}

//=============================================================================
// Create the window
//=============================================================================
bool CreateMainWindow(SDL_Window** phwnd, int cmdShow)
{
    if (phwnd == NULL)
    {
        return false;
    }

    // set up the screen in windowed or fullscreen mode?
    Uint32 flags = SDL_WINDOW_HIDDEN;

    if (GAME_FULLSCREEN)
    {
        flags |= SDL_WINDOW_FULLSCREEN;
    }

    // Create window
    SDL_Window* hwnd = SDL_CreateWindow(GAME_TITLE, GAME_WIDTH, GAME_HEIGHT, flags);

    // if there was an error creating the window
    if (hwnd == NULL)
    {
        return false;
    }

    // Show the window
    if (cmdShow == 1)
    {
        SDL_ShowWindow(hwnd);
    }

    *phwnd = hwnd;

    return true;
}