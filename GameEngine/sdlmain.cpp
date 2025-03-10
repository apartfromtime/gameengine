#include <SDL3\sdl.h>
#include "game.h"

void TranslateAndDispatchEvent(const SDL_Event* msg);
bool CreateMainWindow(SDL_Window** window, int show);

static Game* s_game = NULL;
static SDL_Window* s_hwnd = NULL;

// Timer
uint64_t timeStart = 0;         // Performance Counter start value
uint64_t timeEnd = 0;           // Performance Counter end value
uint64_t timerFreq = 0;         // Performance Counter frequency
uint32_t sleepTime = 0;         // number of milli-seconds to sleep between frames
float frameTime = 0.0f;         // time required for last frame

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
        const SDL_InitFlags initFlags = SDL_INIT_AUDIO | SDL_INIT_VIDEO |
            SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS;
        
        if (SDL_Init(initFlags) == false)
        {
            throw(std::runtime_error(SDL_GetError()));
            return 2;
        }

        // attempt to set up high resolution timer
        if ((timerFreq = SDL_GetPerformanceFrequency()) == 0)
        {
            throw(std::runtime_error(SDL_GetError()));
            return 4;
        }

        timeStart = SDL_GetPerformanceCounter();            // get starting time

        s_game->initialize(s_hwnd);

        SDL_StartTextInput(s_hwnd);

        // main message loop
        int done = 0;

        while (!done)
        {
            // real timer
            // calculate elapsed time of last frame, save in frameTime
            timeEnd = SDL_GetPerformanceCounter();
            frameTime = (float)(timeEnd - timeStart) / (float)timerFreq;

            // not enough time has elapsed for desired frame rate
            if (frameTime < MIN_FRAME_TIME) {
                sleepTime = (uint32_t)((MIN_FRAME_TIME - frameTime) * 1000000);
                SDL_DelayNS(sleepTime);         // release cpu for sleepTime

                continue;
            }

            if (frameTime > MAX_FRAME_TIME) {           // frame rate is very slow
                frameTime = MAX_FRAME_TIME;         // limit maximum frameTime
            }

            timeStart = timeEnd;

            SDL_PumpEvents();

            while (SDL_PeepEvents(&msg, 1, SDL_GETEVENT, SDL_EVENT_FIRST,
                SDL_EVENT_LAST) > 0) {

                if (msg.type == SDL_EVENT_QUIT) {
                    done = 1;
                }

                TranslateAndDispatchEvent(&msg);
            }
            
            s_game->run(frameTime);         // run the game loop
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
    SDL_QuitSubSystem(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_HAPTIC |
        SDL_INIT_GAMEPAD | SDL_INIT_EVENTS);
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