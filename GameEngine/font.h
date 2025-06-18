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
#define PRECISION_QUALITY       2

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
typedef struct Font* LP_FONT;

bool Create(Graphics* pGraphics, int Height, bool Bold,
    bool Italic, long Quality, const char* pFaceName, Font** ppFont);

struct Font
{
public:
    Font() noexcept;
    Font(SDL_Renderer* pRenderer, SDL_Surface* pSurface, SDL_Texture* pTexture,
        SDL_Rect* pMetrics, int* pAdvance, int cellWidth, int cellHeight,
        unsigned int tabSize, bool proportional) noexcept;
    ~Font();
    int DrawText(Graphics* pGraphics, const char* pString, int Count,
        rect_t* pRect, unsigned int Format, COLOR_ARGB Color);
    void SetTabSize(unsigned int Size);

private:
    SDL_Renderer* renderer2d;
    SDL_Surface* surface;
    SDL_Texture* texture;
    SDL_Rect* metrics;
    int* advance;
    int cellW;
    int cellH;
    unsigned int tabSize;
    bool proportional;
};
