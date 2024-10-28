#pragma once
#include <SDL3\SDL.h>
#include <stdexcept>
#include <string.h>

//-----------------------------------------------
// Useful macros
//-----------------------------------------------

//=============================================================================
// Function templates for safely dealing with pointer referenced items.
// The functions defined by these templates may be called using a normal
// function call syntax. The compiler will create a function that replaces T
// with the type of the calling parameter.
//=============================================================================

// Safe free pointer
template <typename T>
inline void safeFree(T& ptr)
{
    if (ptr)
    {
        free(ptr);
        ptr = 0;
    }
}
#define SAFE_FREE safeFree

// Safe free pointer array
template <typename T>
inline void safeFreeArray(T& ptr, uint32_t& length)
{
    for (size_t i = 0; i < length; i++)
    {
        if (ptr[i])
        {
            free(ptr[i]);
            ptr[i] = 0;
        }
    }

    length = 0;
}
#define SAFE_FREE_ARRAY safeFreeArray

// Safely delete pointer referenced item
template <typename T>
inline void safeDelete(T& ptr)
{
    if (ptr)
    {
        delete ptr;
        ptr = NULL;
    }
}
#define SAFE_DELETE safeDelete              // for backward compatiblility

// Safely delete pointer referenced array
template <typename T>
inline void safeDeleteArray(T& ptr)
{
    if (ptr)
    {
        delete[](T*)ptr;
        ptr = NULL;
    }
}
#define SAFE_DELETE_ARRAY safeDeleteArray   // for backward compatiblility

// Safely call deleteAll
template <typename T>
inline void safeDeleteAll(T& ptr)
{
    if (ptr)
        ptr->deleteAll();
}
#define SAFE_DELETE_ALL safeDeleteAll    // for backward compatiblility


//-----------------------------------------------
//                  Constants
//-----------------------------------------------


// window

// Function prototypes
void ExitMainWindow();

const char CLASS_NAME[] = "Game Engine";
const char GAME_TITLE[] = "Game Engine";
const bool GAME_FULLSCREEN = false;              // windowed or fullscreen
const int GAME_WIDTH = 640;              // width of game in pixels
const int GAME_HEIGHT = 480;               // height of game in pixels

// game
const bool VSYNC = false;                   // true locks display to vertical sync rate
const float FRAME_RATE = 240.0f;                // the target frame rate (frames/sec)
const float MIN_FRAME_RATE = 10.0f;             // the minimum frame rate
const float MIN_FRAME_TIME = 1.0f / FRAME_RATE;   // minimum desired time for 1 frame
const float MAX_FRAME_TIME = 1.0f / MIN_FRAME_RATE; // maximum time used in calculations

// XGS_FILE must be location of .xgs file.
const char XGS_FILE[] = "audio\\Win\\Engine.xgs";

// key mappings
// In this game simple constants are used for key mappings. If variables were used
// it would be possible to save and restore key mappings from a data file.
const unsigned int TAB_KEY          = SDL_SCANCODE_TAB;         // Tab key
const unsigned int ENTER_KEY        = SDL_SCANCODE_RETURN;          // Enter key
const unsigned int ESC_KEY          = SDL_SCANCODE_ESCAPE;          // Escape key
const unsigned int SPACE_KEY        = SDL_SCANCODE_SPACE;           // Space key
const unsigned int BACKSPACE_KEY    = SDL_SCANCODE_BACKSPACE;           // Backspace key
const unsigned int CAPSLOCK_KEY     = SDL_SCANCODE_CAPSLOCK;            // Caps lock key
const unsigned int SCROLL_KEY       = SDL_SCANCODE_SCROLLLOCK;			// Scroll lock key
const unsigned int PAUSE_KEY        = SDL_SCANCODE_PAUSE;			// Pause key
const unsigned int UPARROW_KEY      = SDL_SCANCODE_UP;			// Up arrow key
const unsigned int DNARROW_KEY      = SDL_SCANCODE_DOWN;			// Down arrow key
const unsigned int LTARROW_KEY      = SDL_SCANCODE_LEFT;			// Left arrow key
const unsigned int RTARROW_KEY      = SDL_SCANCODE_RIGHT;			// Right arrow key
const unsigned int LWIN_KEY         = SDL_SCANCODE_LGUI;			// Left windows key
const unsigned int RWIN_KEY         = SDL_SCANCODE_RGUI;			// Right windows key
const unsigned int LALT_KEY         = SDL_SCANCODE_LALT;			// Left alt key
const unsigned int RALT_KEY         = SDL_SCANCODE_RALT;			// Right alt key
const unsigned int LCTRL_KEY        = SDL_SCANCODE_LCTRL;			// Left control key
const unsigned int RCTRL_KEY        = SDL_SCANCODE_RCTRL;			// Right control key
const unsigned int LSHIFT_KEY       = SDL_SCANCODE_LSHIFT;			// Left shift key
const unsigned int RSHIFT_KEY       = SDL_SCANCODE_RSHIFT;			// Right shift key
const unsigned int ADD_KEY          = SDL_SCANCODE_EQUALS;
const unsigned int SUB_KEY          = SDL_SCANCODE_MINUS;
const unsigned int INSERT_KEY       = SDL_SCANCODE_INSERT;			// Insert key
const unsigned int DELETE_KEY       = SDL_SCANCODE_DELETE;			// Delete key
const unsigned int PGUP_KEY         = SDL_SCANCODE_PAGEUP;			// Page up key
const unsigned int PGDN_KEY         = SDL_SCANCODE_PAGEDOWN;			// Page down key
const unsigned int HOME_KEY         = SDL_SCANCODE_HOME;			// Home key
const unsigned int END_KEY          = SDL_SCANCODE_END;			// End key
const unsigned int N1_KEY           = SDL_SCANCODE_1;			// 1 key
const unsigned int N2_KEY           = SDL_SCANCODE_2;			// 2 key
const unsigned int N3_KEY           = SDL_SCANCODE_3;			// 3 key
const unsigned int N4_KEY           = SDL_SCANCODE_4;			// 4 key
const unsigned int N5_KEY           = SDL_SCANCODE_5;			// 5 key
const unsigned int N6_KEY           = SDL_SCANCODE_6;			// 6 key
const unsigned int N7_KEY           = SDL_SCANCODE_7;			// 7 key
const unsigned int N8_KEY           = SDL_SCANCODE_8;			// 8 key
const unsigned int N9_KEY           = SDL_SCANCODE_9;			// 9 key
const unsigned int N0_KEY           = SDL_SCANCODE_0;			// 0 key
const unsigned int F1_KEY           = SDL_SCANCODE_F1;
const unsigned int F2_KEY           = SDL_SCANCODE_F2;
const unsigned int F3_KEY           = SDL_SCANCODE_F3;
const unsigned int F4_KEY           = SDL_SCANCODE_F4;
const unsigned int F5_KEY           = SDL_SCANCODE_F5;
const unsigned int F6_KEY           = SDL_SCANCODE_F6;
const unsigned int F7_KEY           = SDL_SCANCODE_F7;
const unsigned int F8_KEY           = SDL_SCANCODE_F8;
const unsigned int F9_KEY           = SDL_SCANCODE_F9;
const unsigned int F10_KEY          = SDL_SCANCODE_F10;
const unsigned int F11_KEY          = SDL_SCANCODE_F11;
const unsigned int F12_KEY          = SDL_SCANCODE_F12;
const unsigned int F13_KEY          = SDL_SCANCODE_F13;
const unsigned int F14_KEY          = SDL_SCANCODE_F14;
const unsigned int F15_KEY          = SDL_SCANCODE_F15;
const unsigned int PRINT_SCR_KEY    = SDL_SCANCODE_PRINTSCREEN;
const unsigned int MOUSE1_KEY       = 0x122 + SDL_BUTTON_LEFT;			// Left mouse key
const unsigned int MOUSE2_KEY       = 0x122 + SDL_BUTTON_RIGHT;			// Right mouse key
const unsigned int MOUSE3_KEY       = 0x122 + SDL_BUTTON_MIDDLE;		// Middle mouse key
const unsigned int MOUSE4_KEY       = 0x122 + SDL_BUTTON_X1;			// Mouse button 4 key
const unsigned int MOUSE5_KEY       = 0x122 + SDL_BUTTON_X2;			// Mouse button 5 key
const unsigned int MWHEELUP_KEY     = 0x122 + 0x07;         // Mouse wheel up
const unsigned int MWHEELDN_KEY     = MWHEELUP_KEY + 1;         // Mouse wheel down
const unsigned int CONSOLE_KEY      = SDL_SCANCODE_GRAVE;           // `~ key for U.S.
