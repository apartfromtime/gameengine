#pragma once
#include "constants.h"
#include "gameError.h"
#include "graphics.h"

//-----------------------------------------------------------------------------
//
// FONT
//
//-----------------------------------------------------------------------------

#define DEFAULT_QUALITY         0
#define HIGH_QUALITY            1

enum ALIGNMENT
{
    TOP         = 0x0000,
    LEFT        = 0x0000,
    HCENTER     = 0x0001,
    RIGHT       = 0x0002,
    VCENTER     = 0x0004,
    BOTTOM      = 0x0008,
    WORDBOUNDS  = 0x0010,
    SINGLELINE  = 0x0020,
    EXPANDTABS  = 0x0040,
    NOCLIP      = 0x0080,
    CALCRECT    = 0x0100,
    RTLREADING  = 0x0200
};

typedef struct Font Font;
typedef struct Font *LP_FONT;

bool Create(SDL_Renderer* pRenderer2d, int Height, bool Bold,
    bool Italic, long Quality, const char* pFaceName, Font** ppFont);

struct Font
{
    virtual int DrawText(LP_SPRITE pSprite, const char* pString, int Count,
        rect_t* pRect, unsigned int Format, COLOR_ARGB Color) = 0;
    virtual void SetTabSize(unsigned int Size) = 0;
};

