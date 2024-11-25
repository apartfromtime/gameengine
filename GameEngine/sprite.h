#pragma once
#include <SDL3\sdl.h>
#include <GEUL\g_math.h>
#include "constants.h"
#include "gameError.h"

//-----------------------------------------------------------------------------
//
// SPRITE
//
//-----------------------------------------------------------------------------

#define SPRITE_DONOTSAVESTATE               (1 << 0)
#define SPRITE_DONOTMODIFY_RENDERSTATE      (1 << 1)
#define SPRITE_OBJECTSPACE                  (1 << 2)
#define SPRITE_ALPHABLEND                   (1 << 3)

typedef struct Sprite Sprite;
typedef struct Sprite *LP_SPRITE;

bool Create(SDL_Renderer* pRenderer2d, Sprite** ppSprite);

struct Sprite
{
    virtual bool Begin(long Flags) = 0;
    virtual bool Draw(SDL_Texture* pTexture, const rect_t* pSrcRect,
        const vector3_t* pCenter, const vector3_t* pPosition,
        color_t Color) = 0;
    virtual bool End() = 0;
    virtual bool Flush() = 0;
    virtual bool GetDevice(SDL_Renderer** ppRenderer2d) = 0;
    virtual bool GetTransform(matrix4_t* pTransform) = 0;
    virtual bool SetTransform(const matrix4_t* pTransform) = 0;
};
