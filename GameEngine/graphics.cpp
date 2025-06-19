#include "graphics.h"

vector3_t Graphics::vertex0 = { 0 };
vector3_t Graphics::vertex1 = { 0 };
vector3_t Graphics::vertex2 = { 0 };
vector3_t Graphics::vertex3 = { 0 };
SDL_FColor Graphics::colour0 = { 1, 1, 1, 1 };
const int Graphics::ibuffer[6] = { 0, 1, 2, 2, 3, 0 };
SDL_Vertex Graphics::vbuffer[4] = { 0 };

static bool BlitBW(image_t* tex) noexcept
{
    void* pixels = NULL;
    uint8_t* dst = NULL;
    int src, src_pitch, dst_pitch, dst_bytes, src_bytes;
    if (tex == NULL) {
        return false;
    }
    if (tex->format != GEUL_LUMINANCE || tex->format != GEUL_LUMINANCE_ALPHA) {
        return false;
    }
    src_bytes = (tex->format == GEUL_LUMINANCE ? 1 : 2);
    dst_bytes = (tex->format == GEUL_LUMINANCE ? 3 : 4);
    src_pitch = (tex->width * src_bytes);
    dst_pitch = (tex->width * dst_bytes);
    pixels = malloc(tex->height * dst_pitch);
    if (pixels == NULL) {
        return false;
    }
    switch (dst_bytes)
    {
    case 3:
    {
        for (uint32_t y = 0; y < tex->height; y++) {
            src = y * src_pitch;
            dst = ((uint8_t*)pixels + (y * dst_pitch));
            for (uint32_t x = 0; x < tex->width; x++) {
                dst[x*3+0] = tex->pixels[src];
                dst[x*3+1] = tex->pixels[src];
                dst[x*3+2] = tex->pixels[src];
                src += src_bytes;
            }
        }
    } break;
    case 4:
    {
        for (uint32_t y = 0; y < tex->height; y++) {
            src = y * src_pitch;
            dst = ((uint8_t*)pixels + (y * dst_pitch));
            for (uint32_t x = 0; x < tex->width; x++) {
                dst[x*4+0] = tex->pixels[src+0];
                dst[x*4+1] = tex->pixels[src+0];
                dst[x*4+2] = tex->pixels[src+0];
                dst[x*4+3] = tex->pixels[src+1];
                src += src_bytes;
            }
        }
    } break;
    }
    free(tex->pixels);
    tex->pixels = (uint8_t*)pixels;
    tex->depth = dst_bytes * 8;
    tex->format = (tex->format == GEUL_LUMINANCE) ? GEUL_RGB : GEUL_RGBA;
    return true;
}

//=============================================================================
// Constructor
//=============================================================================
Graphics::Graphics()
{
    // SDL
    hwnd = NULL;
    renderer2d = NULL;
    SDL_memset(&displayMode, 0, sizeof(SDL_DisplayMode));
    numberOfPixelsColliding = 0;
    depthStencilBuffer = NULL;
    stencilSupport = false;
    windowRect.x = 0;
    windowRect.y = 0;
    windowRect.w = 0;
    windowRect.h = 0;
    viewport2d.x = 0;
    viewport2d.y = 0;
    viewport2d.w = 0;
    viewport2d.h = 0;
    // Presentation parameters
    backBufferWidth = 0;
    backBufferHeight = 0;
    backBufferFormat = SDL_PIXELFORMAT_UNKNOWN;
    backBufferCount = 0;
    enableAutoDepthStencil = false;
    autoDepthStencilFormat = SDL_PIXELFORMAT_UNKNOWN;
    fullScreenRefreshRateInHz = 0;
    presentationInterval = 0;
    // View
    viewport3d = Viewport3d();
    matrix3d[0] = Matrix4();
    matrix3d[1] = Matrix4();
    matrix3d[2] = Matrix4();
    transform3d = Matrix4();
    // Window
    vsync = false;
    fullscreen = false;
    width = 0;         // width & height are replaced in initialize()
    height = 0;
    backColor = graphicsNS::BACK_COLOR;
}

//=============================================================================
// Destructor
//=============================================================================
Graphics::~Graphics()
{
    releaseAll();
}

//=============================================================================
// Release all
//=============================================================================
void Graphics::releaseAll()
{
    if (depthStencilBuffer != NULL)
    {
        SDL_DestroyTexture(depthStencilBuffer);
        depthStencilBuffer = NULL;
    }
}

//=============================================================================
// Initialize SDL graphics
//=============================================================================
bool Graphics::initialize(SDL_Window* phwnd, int w, int h, bool full, bool vsync)
{
    this->hwnd = phwnd;
    this->vsync = vsync;

    SDL_GetWindowSize(hwnd, &width, &height);

    if (SDL_GetWindowFlags(hwnd) & SDL_WINDOW_FULLSCREEN)
    {
        fullscreen = true;
    }

    // create renderer
    const char* name = SDL_GetRenderDriver(0);
    if ((renderer2d = SDL_CreateRenderer(hwnd, name)) == NULL)
    {
        GameError(gameErrorNS::FATAL_ERROR,
            "Error creating SDL device: %s\n", SDL_GetError());
        return false;
    }

    initSDLpp();

    windowRect.x = 0;
    windowRect.y = 0;
    windowRect.w = 0;
    windowRect.h = 0;

    if (fullscreen)
    {
        if (isAdapterCompatible())
        {
            fullScreenRefreshRateInHz = displayMode.refresh_rate;
        }
        else
        {
            GameError(gameErrorNS::WARNING,
                "The graphics device does not support the specified resolution and/or format.\n");
            return false;
        }

        windowRect.w = displayMode.w;
        windowRect.h = displayMode.h;
    }
    else
    {
        SDL_GetWindowSizeInPixels(hwnd, &windowRect.w, &windowRect.h);
    }

    displayMode.w = backBufferWidth;
    displayMode.h = backBufferHeight;
    displayMode.refresh_rate = fullScreenRefreshRateInHz;
    displayMode.format = backBufferFormat;

    SDL_SetWindowFullscreenMode(hwnd, &displayMode);
    SDL_SetRenderVSync(renderer2d, presentationInterval);

    if (enableAutoDepthStencil == true)
    {
        depthStencilBuffer = SDL_CreateTexture(renderer2d,
            autoDepthStencilFormat, SDL_TEXTUREACCESS_TARGET, displayMode.w,
            displayMode.h);

        if (depthStencilBuffer != NULL)
        {
            stencilSupport = true;
        }
    }

    SDL_GetRenderViewport(renderer2d, &viewport2d);
    viewport3d = Viewport3d((float)viewport2d.x, (float)viewport2d.y, (float)viewport2d.w,
        (float)viewport2d.h, 0.0f, 1.0f);

    SDL_SetRenderDrawBlendMode(renderer2d, SDL_BLENDMODE_BLEND);
    SDL_SetRenderScale(renderer2d, 1.0f, 1.0f);

    matrix3d[0] = Matrix4();
    matrix3d[1] = Matrix4();
    matrix3d[2] = Matrix4();
    transform3d = Matrix4();

    // Sprite
    prevBlendMode = SDL_BLENDMODE_NONE;
    flags = 0;

    return true;
}

//=============================================================================
// Initialize SDL presentation parameters
//=============================================================================
void Graphics::initSDLpp()
{
    backBufferWidth = width;
    backBufferHeight = height;
    backBufferCount = 1;
    fullScreenRefreshRateInHz = 0;

    if (fullscreen)
    {
        backBufferFormat = SDL_PIXELFORMAT_XRGB8888;            // 24-bit color
    }
    else
    {
        const SDL_DisplayMode* dMode =
            SDL_GetDesktopDisplayMode(SDL_GetDisplayForWindow(hwnd));

        if (dMode == NULL)
        {
            backBufferFormat = SDL_PIXELFORMAT_UNKNOWN;
        }
        else
        {
            backBufferFormat = dMode->format;          // use desktop setting
        }
    }

    if (vsync == true)
    {
        presentationInterval = 1;
    }
    else
    {
        presentationInterval = 0;
    }

    enableAutoDepthStencil = false;         // sdl doesn't support depth stenciling
    autoDepthStencilFormat = SDL_PIXELFORMAT_RGBA8888;          // depth 24-bit, stencil 8-bit
}

//=============================================================================
// Checks the adapter to see if it is compatible with the BackBuffer height,
// width and refresh rate specified in sdlpp.
//=============================================================================
bool Graphics::isAdapterCompatible()
{
    SDL_DisplayID displayIndex = SDL_GetDisplayForWindow(hwnd);
    int modeCount = 0;
    SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes(displayIndex,
        &modeCount);

    for (int i = 0; i < modeCount; i++)
    {
        if ((modes[i]->format == backBufferFormat ||
            backBufferFormat == SDL_PIXELFORMAT_UNKNOWN) &&
            modes[i]->h == backBufferHeight &&
            modes[i]->w == backBufferWidth &&
            modes[i]->refresh_rate >= fullScreenRefreshRateInHz)
        {
            displayMode = *modes[i];

            return true;
        }
    }

    return false;
}

//=============================================================================
// Reset the graphics device
//=============================================================================
bool Graphics::reset()
{
    initSDLpp();

    if (fullscreen)
    {
        if (isAdapterCompatible())
        {
            fullScreenRefreshRateInHz = displayMode.refresh_rate;
        }
        else
        {
            GameError(gameErrorNS::WARNING,
                "The graphics device does not support the specified resolution and/or format.\n");
            return false;
        }
    }

    displayMode.format = backBufferFormat;
    displayMode.w = backBufferWidth;
    displayMode.h = backBufferHeight;
    displayMode.refresh_rate = fullScreenRefreshRateInHz;

    const bool result = SDL_SetWindowFullscreenMode(hwnd, &displayMode);
    SDL_SetRenderVSync(renderer2d, presentationInterval);

    SDL_GetRenderViewport(renderer2d, &viewport2d);
    viewport3d = Viewport3d((float)viewport2d.x, (float)viewport2d.y, (float)viewport2d.w,
        (float)viewport2d.h, 0.0f, 1.0f);

    SDL_SetRenderDrawBlendMode(renderer2d, SDL_BLENDMODE_BLEND);

    return result;
}

//=============================================================================
// Toggle window or fullscreen mode
//=============================================================================
void Graphics::changeDisplayMode(graphicsNS::DISPLAY_MODE mode)
{
    switch (mode)
    {
    case graphicsNS::DISPLAYMODE_FULLSCREEN:
    {
        if (fullscreen)         // if already in fullscreen mode
        {
            return;
        }

        fullscreen = true;
    } break;
    case graphicsNS::DISPLAYMODE_WINDOW:
    {
        if (fullscreen == false)            // if already in window mode
        {
            return;
        }

        fullscreen = false;
    } break;
    case graphicsNS::DISPLAYMODE_TOGGLE:
    {
        fullscreen = !fullscreen;
    } break;
    default:            // default to window
    {
        fullscreen = false;
    }
    }

    windowRect.x = 0;
    windowRect.y = 0;
    windowRect.w = 0;
    windowRect.h = 0;

    if (fullscreen)         // fullscreen
    {
        SDL_SetWindowFullscreen(hwnd, true);            // borderless window fullscreen
        windowRect.w = displayMode.w;
        windowRect.h = displayMode.h;
    }
    else            // windowed
    {
        SDL_SetWindowFullscreen(hwnd, true);
        SDL_GetWindowSizeInPixels(hwnd, &windowRect.w, &windowRect.h);
    }

    reset();
}

//=============================================================================
// Display the backbuffer
//=============================================================================
bool Graphics::showBackbuffer()
{
    // Display backbuffer to screen
    SDL_RenderPresent(renderer2d);

    return true;
}

//=============================================================================
// Frees the data associated with this texture
//=============================================================================
void Graphics::freeTexture(LP_TEXTURE texture)
{
    SDL_DestroyTexture(texture);
}

//=============================================================================
// Load the texture into default SDL memory (normal texture use)
// For internal engine use only. Use the TextureManager class to load game
// textures.
//=============================================================================
bool Graphics::loadTexture(const char* filename, COLOR_ARGB transcolor,
    unsigned int& width, unsigned int& height, LP_TEXTURE& texture)
{
    // create surface
    image_t image = { 0 };
    bool result = false;

    result = GetImageInfoFromFile(&image, filename);

    if (result == false)
    {
        texture = NULL;
        return false;
    }

    width = image.width;
    height = image.height;

    uint8_t colorkey[4] = {
        (uint8_t)(transcolor.b * 255.0f),
        (uint8_t)(transcolor.g * 255.0f),
        (uint8_t)(transcolor.r * 255.0f),
        (uint8_t)(transcolor.a * 255.0f)
    };

    result = LoadImageFromFile(&image, NULL, colorkey, filename);
    if (result == false)
    {
        texture = NULL;
        return false;
    }

    SDL_PixelFormat pixelformat = SDL_PIXELFORMAT_UNKNOWN;
    switch (image.format)
    {
    case GEUL_COLOUR_INDEX:
    {
        pixelformat = SDL_PIXELFORMAT_INDEX8;
    } break;
    case GEUL_RGB:
    {
        pixelformat = SDL_PIXELFORMAT_RGB24;
    } break;
    case GEUL_RGBA:
    {
        pixelformat = SDL_PIXELFORMAT_RGBA32;
    } break;
    case GEUL_BGR:
    {
        pixelformat = SDL_PIXELFORMAT_BGR24;
    } break;
    case GEUL_BGRA:
    {
        pixelformat = SDL_PIXELFORMAT_BGRA32;
    } break;
    case GEUL_LUMINANCE:
    {
        if (BlitBW(&image) == false) {
            free(image.pixels);
            return false;
        }
        pixelformat = SDL_PIXELFORMAT_RGB24;
    } break;
    case GEUL_LUMINANCE_ALPHA:
    {
        if (BlitBW(&image) == false) {
            free(image.pixels);
            return false;
        }
        pixelformat = SDL_PIXELFORMAT_RGBA32;
    } break;
    default:
    {
        free(image.pixels);
        return false;
    } break;
    }

    // create the new texture
    texture = SDL_CreateTexture(renderer2d, pixelformat, SDL_TEXTUREACCESS_STATIC,
        width, height);
    if (texture == NULL)
    {
        return false;
    }

    LOCKED_RECT pLockedRect = { 0 };

    pLockedRect.pBits = image.pixels;
    pLockedRect.pitch = image.width * (image.depth >> 3);

    if (SDL_UpdateTexture(texture, NULL, pLockedRect.pBits, pLockedRect.pitch) ==
        false)
    {
        SDL_DestroyTexture(texture);

        return false;
    }

    free(image.pixels);

    return true;
}

//=============================================================================
// Load the texture into system memory (system memory is lockable)
// Provides direct access to pixel data. Use the TextureManager class to load
// textures for display.
//=============================================================================
bool Graphics::loadTextureSystemMem(const char* filename, COLOR_ARGB transcolor,
    unsigned int& width, unsigned int& height, LP_TEXTURE& texture)
{
    // create surface
    image_t image = { 0 };
    bool result = false;

    result = GetImageInfoFromFile(&image, filename);

    if (result == false)
    {
        texture = NULL;
        return false;
    }

    width = image.width;
    height = image.height;

    uint8_t colorkey[4] = {
        (uint8_t)(transcolor.b * 255.0f),
        (uint8_t)(transcolor.g * 255.0f),
        (uint8_t)(transcolor.r * 255.0f),
        (uint8_t)(transcolor.a * 255.0f)
    };

    result = LoadImageFromFile(&image, NULL, colorkey, filename);
    if (result == false)
    {
        texture = NULL;
        return false;
    }

    SDL_PixelFormat pixelformat = SDL_PIXELFORMAT_UNKNOWN;
    switch (image.format)
    {
    case GEUL_COLOUR_INDEX:
    {
        pixelformat = SDL_PIXELFORMAT_INDEX8;
    } break;
    case GEUL_RGB:
    {
        pixelformat = SDL_PIXELFORMAT_RGB24;
    } break;
    case GEUL_RGBA:
    {
        pixelformat = SDL_PIXELFORMAT_RGBA32;
    } break;
    case GEUL_BGR:
    {
        pixelformat = SDL_PIXELFORMAT_BGR24;
    } break;
    case GEUL_BGRA:
    {
        pixelformat = SDL_PIXELFORMAT_BGRA32;
    } break;
    case GEUL_LUMINANCE:
    {
        if (BlitBW(&image) == false) {
            free(image.pixels);
            return false;
        }
        pixelformat = SDL_PIXELFORMAT_RGB24;
    } break;
    case GEUL_LUMINANCE_ALPHA:
    {
        if (BlitBW(&image) == false) {
            free(image.pixels);
            return false;
        }
        pixelformat = SDL_PIXELFORMAT_RGBA32;
    } break;
    default:
    {
        free(image.pixels);
        return false;
    } break;
    }

    // create the new texture
    texture = SDL_CreateTexture(renderer2d, pixelformat, SDL_TEXTUREACCESS_STREAMING,
        width, height);

    if (texture == NULL)
    {
        return false;
    }

    LOCKED_RECT pLockedRect = { 0 };

    pLockedRect.pBits = image.pixels;
    pLockedRect.pitch = image.width * (image.depth >> 3);

    if (SDL_UpdateTexture(texture, NULL, pLockedRect.pBits, pLockedRect.pitch) ==
        false)
    {
        SDL_DestroyTexture(texture);

        return false;
    }

    return true;
}

//=============================================================================
// Locks a rectangle on a texture resource.
//=============================================================================
bool Graphics::lockRect(LP_TEXTURE& texture, LOCKED_RECT* pLockedRect,
    const rect_t* pRect)
{
    SDL_Rect rect = { 0 };

    if (pRect != NULL)
    {
        rect.x = (int)pRect->min.x;
        rect.y = (int)pRect->min.y;
        rect.w = (int)pRect->max.x - (int)pRect->min.x;
        rect.h = (int)pRect->max.y - (int)pRect->min.y;
    }

    if (SDL_LockTexture(texture, &rect, &pLockedRect->pBits,
        &pLockedRect->pitch) == false)
    {
        return false;
    }

    return true;
}

//=============================================================================
// Unlocks a rectangle on a texture resource.
//=============================================================================
bool Graphics::unlockRect(LP_TEXTURE& texture)
{
    SDL_UnlockTexture(texture);

    return true;
}

//=============================================================================
// Draw pixel at X,Y
//=============================================================================
void Graphics::drawPoint(float x, float y, uint8_t width, COLOR_ARGB color)
{
    const vector3_t p0 = { x - (width >> 1), y - (width >> 1), 1 };
    const vector3_t p1 = { x + (width >> 1), y - (width >> 1), 1 };
    const vector3_t p2 = { x + (width >> 1), y + (width >> 1), 1 };
    const vector3_t p3 = { x - (width >> 1), y + (width >> 1), 1 };
    drawSprite(NULL, NULL, p0, p1, p2, p3, color);
}

//=============================================================================
// Draw pixels from list
//=============================================================================
void Graphics::drawPoint(const vector2_t* pointList, unsigned long pointListCount,
    uint8_t width, COLOR_ARGB color)
{
    if (pointList == NULL || pointListCount == 0)
    {
        return;
    }
    for (unsigned long i = 0; i < pointListCount - 1; i++)
    {
        drawPoint(pointList[i].x, pointList[i].y, width, color);
    }
}

//=============================================================================
// Draw line from X1,Y1 to X2,Y2.
//=============================================================================
void Graphics::drawLine(float x1, float y1, float x2, float y2, uint8_t width,
    COLOR_ARGB color)
{
    const float hw = width / 2.0f;
    const vector2_t dt = Vector2(y2 - y1, x2 - x1);
    const vector2_t dn = NormalizeVector2(dt);
    const vector3_t p0 = Vector3(x1 - (hw * dn.x), y1 + (hw * dn.y), 1);
    const vector3_t p1 = Vector3(x2 - (hw * dn.x), y2 + (hw * dn.y), 1);
    const vector3_t p2 = Vector3(x2 + (hw * dn.x), y2 - (hw * dn.y), 1);
    const vector3_t p3 = Vector3(x1 + (hw * dn.x), y1 - (hw * dn.y), 1);
    drawSprite(NULL, NULL, p0, p1, p2, p3, color);
}

//=============================================================================
// Draw lines, from vertex list
//=============================================================================
void Graphics::drawLine(const vector2_t* vertexList, unsigned long vertexListCount,
    uint8_t width, COLOR_ARGB color)
{
    if (vertexList == NULL || vertexListCount == 0)
    {
        return;
    }
    float x1 = 0;
    float y1 = 0;
    float x2 = 0;
    float y2 = 0;
    for (unsigned long i = 0; i < vertexListCount - 1; i++)
    {
        x1 = vertexList[i+0].x;
        y1 = vertexList[i+0].y;
        x2 = vertexList[i+1].x;
        y2 = vertexList[i+1].y;
        drawLine(x1, y1, x2, y2, width, color);
    }
}

//=============================================================================
// Display a quad (rectangle) with alpha transparency.
//=============================================================================
void Graphics::drawQuad(const vector3_t p0, const vector3_t p1,
    const vector3_t p2, const vector3_t p3, COLOR_ARGB color)
{
    drawSprite(NULL, NULL, p0, p1, p2, p3, color);
}

//=============================================================================
// Display a quads (rectangle) from quad list with alpha transparency.
//=============================================================================
void Graphics::drawQuad(const vector3_t* quadList[4], unsigned long quadListCount,
    COLOR_ARGB color)
{
    if (quadList == NULL || quadListCount == 0)
    {
        return;
    }
    for (unsigned long i = 0; i < quadListCount; i++)
    {
        drawQuad(quadList[i][0], quadList[i][1], quadList[i][2], quadList[i][3],
            color);
    }
}

//=============================================================================
// Display a sprite textured (rectangle) with alpha transparency.
//=============================================================================
void Graphics::drawSprite(LP_TEXTURE texture, const rect_t* psrcrect,
    const vector3_t p0, const vector3_t p1,
    const vector3_t p2, const vector3_t p3,
    COLOR_ARGB color)
{
    rect_t rect = { 0 };
    float width = 1.0f;
    float height = 1.0f;
    if (texture != NULL) {
        SDL_GetTextureSize(texture, &width, &height);
    }
    if (psrcrect != NULL) {
        rect = {
            psrcrect->min.x, psrcrect->min.y,
            psrcrect->max.x, psrcrect->max.y
        };
    } else {
        rect = { 0, 0, width, height };
    }
    const float s = 1.0f / width;
    const float t = 1.0f / height;
    const float s0 = s * rect.min.x;
    const float t0 = t * rect.min.y;
    const float s1 = s * rect.max.x;
    const float t1 = t * rect.max.y;
    vertex0 = TransformVector3Coord(p0, transform3d);
    vertex1 = TransformVector3Coord(p1, transform3d);
    vertex2 = TransformVector3Coord(p2, transform3d);
    vertex3 = TransformVector3Coord(p3, transform3d);
    colour0 = {
        color.r,
        color.g,
        color.b,
        color.a
    };
    vbuffer[0] = {
        { vertex0.x, vertex0.y },
        { colour0 },
        { s0, t0  }
    };
    vbuffer[1] = {
        { vertex1.x, vertex1.y },
        { colour0 },
        { s1, t0  }
    };
    vbuffer[2] = {
        { vertex2.x, vertex2.y },
        { colour0 },
        { s1, t1  }
    };
    vbuffer[3] = {
        { vertex3.x, vertex3.y },
        { colour0 },
        { s0, t1  }
    };
    SDL_RenderGeometry(renderer2d, texture,
        vbuffer, sizeof(vbuffer) / sizeof(vbuffer[0]),
        ibuffer, sizeof(ibuffer) / sizeof(ibuffer[0])
    );
}

//=============================================================================
// Draw the sprite described in SpriteData structure
// Color is optional, it is applied like a filter, WHITE is default (no change)
//=============================================================================
void Graphics::drawSprite(LP_TEXTURE texture, const rect_t** psrcrect,
    unsigned long rectListCount, const vector3_t* spriteList[4],
    unsigned long spriteListCount, COLOR_ARGB color)
{
    if (spriteList == NULL || spriteListCount == 0)
    {
        return;
    }
    for (unsigned long i = 0; i < spriteListCount; i++)
    {
        drawSprite(texture, psrcrect[i],
            spriteList[i][0], spriteList[i][1],
            spriteList[i][2], spriteList[i][3],
            color);
    }
}

//=============================================================================
// Test for lost device
//=============================================================================
bool Graphics::getDeviceState()
{
    return false;
}

//=============================================================================
// Return the number of pixels colliding between the two sprites.
//=============================================================================
unsigned long Graphics::pixelCollision(const SpriteData& sprite1,
    const SpriteData& sprite2)
{
    if (!stencilSupport)            // if no stencil buffer support
    {
        return 0;
    }

    // TODO:
    numberOfPixelsColliding = 0;

    return numberOfPixelsColliding;
}

//=============================================================================
// Multiplies world, view, or projection matrices.
//=============================================================================
void Graphics::multiplyTransform()
{
    // multiply world, view, projection
    transform3d = MultiplyMatrix4(matrix3d[2],
        MultiplyMatrix4(matrix3d[1], matrix3d[0]));
}

//=============================================================================
// Return the windows client area in pixels.
//=============================================================================
SDL_Rect Graphics::getWindowRect() const
{
    return windowRect;
}

//=============================================================================
// Return viewport.
//=============================================================================
viewport_t Graphics::get3DViewport() const
{
    return viewport3d;
}

//=============================================================================
// Return renderer3d.
//=============================================================================
SDL_Renderer* Graphics::get2DRenderer()
{
    return renderer2d;
}

//=============================================================================
// return width
//=============================================================================
int Graphics::getWidth() const
{
    return width;
}

//=============================================================================
// return height
//=============================================================================
int Graphics::getHeight() const
{
    return height;
}

//=============================================================================
// Return fullscreen
//=============================================================================
bool Graphics::getFullscreen() const
{
    return fullscreen;
}

//=============================================================================
// Returns true if the graphics card supports a stencil buffer
//=============================================================================
bool Graphics::getStencilSupport() const
{
    return stencilSupport;
}

//=============================================================================
// Returns transform
//=============================================================================
void Graphics::getTransform(matrix4_t& matrix, TRANSFORMTYPE type) const
{
    if (type == TRANSFORMTYPE_WORLD)
    {
        matrix = matrix3d[0];
    }

    if (type == TRANSFORMTYPE_VIEW)
    {
        matrix = matrix3d[1];
    }

    if (type == TRANSFORMTYPE_PROJECTION)
    {
        matrix = matrix3d[2];
    }

    if (type == TRANSFORMTYPE_TRANSFORM)
    {
        matrix = transform3d;
    }
}

//=============================================================================
// Set VSync
//=============================================================================
void Graphics::setVSync(bool vs)
{
    vsync = vs;
}

//=============================================================================
// Set color used to clear screen
//=============================================================================
void Graphics::setBackColor(COLOR_ARGB c)
{
    backColor = c;
}

//=============================================================================
// Clear backbuffer and BeginScene()
//=============================================================================
bool Graphics::beginScene()
{
    if (renderer2d == 0)
    {
        return false;
    }

    // clear backbuffer to backColor
    SDL_SetRenderDrawColorFloat(renderer2d, backColor.r, backColor.g,
        backColor.b, backColor.a);
    SDL_RenderClear(renderer2d);

    return true;
}

//=============================================================================
// EndScene()
//=============================================================================
bool Graphics::endScene()
{
    SDL_SetRenderScale(renderer2d, 1.0f, 1.0f);
    matrix3d[0] = Matrix4();
    matrix3d[1] = Matrix4();
    matrix3d[2] = Matrix4();
    transform3d = Matrix4();

    return true;
}

//=============================================================================
// Set tranform
//=============================================================================
void Graphics::setTransform(const matrix4_t& matrix, TRANSFORMTYPE type)
{
    if (type == TRANSFORMTYPE_WORLD)
    {
        matrix3d[0] = matrix;
    }

    if (type == TRANSFORMTYPE_VIEW)
    {
        matrix3d[1] = matrix;
    }

    if (type == TRANSFORMTYPE_PROJECTION)
    {
        matrix3d[2] = matrix;
    }

    if (type == TRANSFORMTYPE_TRANSFORM)
    {
        transform3d = matrix;
    }
}

//=============================================================================
// Sprite Begin
//=============================================================================
bool Graphics::spriteBegin(long Flags)
{
    setTransform(Matrix4(), TRANSFORMTYPE_TRANSFORM);

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

//=============================================================================
// Sprite End
//=============================================================================
bool Graphics::spriteEnd()
{
    bool result = Flush();

    if ((flags & SPRITE_DONOTSAVESTATE) != SPRITE_DONOTSAVESTATE)
    {
        SDL_SetRenderDrawBlendMode(renderer2d, prevBlendMode);
    }

    return result;
}

//=============================================================================
// Flush renderer
//=============================================================================
bool Graphics::Flush()
{
    return SDL_FlushRenderer(renderer2d);
}