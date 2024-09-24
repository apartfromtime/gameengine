#include "sprite.h"

struct SpriteBase : public Sprite
{
public:
    SpriteBase() noexcept;
    SpriteBase(SDL_Renderer* pRenderer2d) noexcept;
    bool Begin(long Flags);
    bool Draw(SDL_Texture* pTexture, const rect_t* pSrcRect,
        const vector3_t* pCenter, const vector3_t* pPosition,
        color_t Color);
    bool End();
    bool Flush();
    bool GetDevice(SDL_Renderer** ppRenderer2d);
    bool GetTransform(matrix4_t* pTransform);
    bool SetTransform(const matrix4_t* pTransform);

private:
    SDL_Renderer* renderer2d;
    SDL_BlendMode prevBlendMode;
    matrix4_t transform3d;
    long flags;
};

bool Create(SDL_Renderer* pRenderer, Sprite** ppSprite)
{
    if (pRenderer == NULL || ppSprite == NULL)
    {
        return false;
    }

    SpriteBase* pSprite = new SpriteBase(pRenderer);
    *ppSprite = pSprite;

    return true;
}

SpriteBase::SpriteBase() noexcept
{
    renderer2d = NULL;
    prevBlendMode = SDL_BLENDMODE_NONE;
    transform3d = Matrix4();
    flags = 0;
}

SpriteBase::SpriteBase(SDL_Renderer* pRenderer) noexcept : renderer2d(pRenderer)
{
    prevBlendMode = SDL_BLENDMODE_NONE;
    transform3d = Matrix4();
}

bool SpriteBase::Begin(long Flags)
{
    transform3d = Matrix4();

    if ((Flags & SPRITE_DONOTSAVESTATE) != SPRITE_DONOTSAVESTATE)
    {
        SDL_GetRenderDrawBlendMode(renderer2d, &prevBlendMode);
    }

    if ((Flags & SPRITE_DONOTMODIFY_RENDERSTATE) !=
        SPRITE_DONOTMODIFY_RENDERSTATE)
    {
        if ((Flags & SPRITE_ALPHABLEND) == SPRITE_ALPHABLEND)
        {
            SDL_SetRenderDrawBlendMode(renderer2d, SDL_BLENDMODE_BLEND);
        }
    }

    flags = Flags;

    return true;
}

bool SpriteBase::Draw(SDL_Texture* pTexture, const rect_t* pSrcRect,
    const vector3_t* pCenter, const vector3_t* pPosition, color_t Color)
{
    rect_t rect = Rectangle();
    float tw = 0.0f;
    float th = 0.0f;
    float x0 = 0.0f;
    float y0 = 0.0f;
    float z0 = 0.0f;
    float x1 = 0.0f;
    float y1 = 0.0f;

    if (pTexture == NULL)
    {
        return false;
    }

    SDL_GetTextureSize(pTexture, &tw, &th);

    if (pSrcRect != NULL)
    {
        rect = Rectangle(pSrcRect->min[0], pSrcRect->min[1],
            pSrcRect->max[0], pSrcRect->max[1]);
    }
    else
    {
        rect = Rectangle(0, 0, (long)tw, (long)th);
    }

    if (pPosition != NULL)
    {
        x0 = pPosition->x;
        y0 = pPosition->y;
        z0 = pPosition->z;
    }

    if (pCenter != NULL)
    {
        x0 = x0 - pCenter->x;
        y0 = y0 - pCenter->y;
        z0 = z0 - pCenter->z;
    }

    x1 = x0 + (float)(rect.max[0] - rect.min[0]);
    y1 = y0 + (float)(rect.max[1] - rect.min[1]);

    // world, view and projection transform
    vector4_t v0 = Vector4(x0, y0, z0, 1.0f);
    vector4_t v1 = Vector4(x1, y0, z0, 1.0f);
    vector4_t v2 = Vector4(x1, y1, z0, 1.0f);
    vector4_t v3 = Vector4(x0, y1, z0, 1.0f);

    if ((flags & SPRITE_OBJECTSPACE) != SPRITE_OBJECTSPACE)
    {
        v0 = TransformVector4(v0, transform3d);
        v1 = TransformVector4(v1, transform3d);
        v2 = TransformVector4(v2, transform3d);
        v3 = TransformVector4(v3, transform3d);
    }

    const float s = 1.0f / tw;
    const float t = 1.0f / th;
    const float s1 = s * (float)(rect.max[0]);
    const float t1 = t * (float)(rect.max[1]);
    const float s0 = s * (float)(rect.min[0]);
    const float t0 = t * (float)(rect.min[1]);

    SDL_FColor c = {
        Color.r,
        Color.g,
        Color.b,
        Color.a
    };

    SDL_Vertex vertex0 = {
        { v0.x, v0.y },
        { c          },
        { s0, t0     }
    };

    SDL_Vertex vertex1 = {
        { v1.x, v1.y },
        { c          },
        { s1, t0     }
    };

    SDL_Vertex vertex2 = {
        { v2.x, v2.y },
        { c          },
        { s1, t1     }
    };

    SDL_Vertex vertex3 = {
        { v3.x, v3.y },
        { c          },
        { s0, t1     }
    };

    const SDL_Vertex vertice[4] = { vertex0, vertex1, vertex2, vertex3 };
    static const int indices[6] = { 0, 1, 2, 2, 3, 0 };

    SDL_RenderGeometry(renderer2d, pTexture,
        vertice, sizeof(vertice) / sizeof(vertice[0]),
        indices, sizeof(indices) / sizeof(indices[0]));

    return true;
}

bool SpriteBase::End()
{
    bool result = Flush();

    if ((flags & SPRITE_DONOTSAVESTATE) != SPRITE_DONOTSAVESTATE)
    {
        SDL_SetRenderDrawBlendMode(renderer2d, prevBlendMode);
    }

    return result;
}

bool SpriteBase::Flush()
{
    return SDL_FlushRenderer(renderer2d);
}

bool SpriteBase::GetDevice(SDL_Renderer** ppRenderer2d)
{
    if (ppRenderer2d != NULL)
    {
        *ppRenderer2d = renderer2d;
    }

    return true;
}

bool SpriteBase::GetTransform(matrix4_t* pTransform)
{
    if (pTransform != NULL)
    {
        *pTransform = transform3d;
    }

    return true;
}

bool SpriteBase::SetTransform(const matrix4_t* pTransform)
{
    if (pTransform != NULL)
    {
        transform3d = *pTransform;
    }

    return true;
}