#include "graphics.h"

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
    sprite = NULL;
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
    viewport3d = Viewport();
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
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

//=============================================================================
// Release all
//=============================================================================
void Graphics::releaseAll()
{
    safeDelete(sprite);

    if (depthStencilBuffer != NULL)
    {
        SDL_DestroyTexture(depthStencilBuffer);
        depthStencilBuffer = NULL;
    }

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

//=============================================================================
// Initialize SDL graphics
//=============================================================================
bool Graphics::initialize(SDL_Window* phwnd, int w, int h, bool full, bool vsync)
{
    if (SDL_Init(SDL_INIT_VIDEO) != SDL_TRUE)
    {
        throw(std::runtime_error(SDL_GetError()));
        return false;
    }

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
    viewport3d = Viewport(viewport2d.x, viewport2d.y, viewport2d.w,
        viewport2d.h, 0.0f, 1.0f);

    SDL_SetRenderDrawBlendMode(renderer2d, SDL_BLENDMODE_BLEND);
    SDL_SetRenderScale(renderer2d, 1.0f, 1.0f);

    matrix3d[0] = Matrix4();
    matrix3d[1] = Matrix4();
    matrix3d[2] = Matrix4();
    transform3d = Matrix4();

    Create(renderer2d, &sprite);

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

    const bool result = SDL_SetWindowFullscreenMode(hwnd, &displayMode) ==
        SDL_TRUE ? true : false;
    SDL_SetRenderVSync(renderer2d, presentationInterval);

    SDL_GetRenderViewport(renderer2d, &viewport2d);
    viewport3d = Viewport(viewport2d.x, viewport2d.y, viewport2d.w,
        viewport2d.h, 0.0f, 1.0f);

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
        SDL_SetWindowFullscreen(hwnd, SDL_TRUE);            // borderless window fullscreen
        windowRect.w = displayMode.w;
        windowRect.h = displayMode.h;
    }
    else            // windowed
    {
        SDL_SetWindowFullscreen(hwnd, SDL_FALSE);
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
    SDL_Surface* image = IMG_Load(filename);

    if (image == NULL)
    {
        return false;
    }

    SDL_SetSurfaceColorKey(image, SDL_TRUE,
        SDL_MapRGBA(SDL_GetPixelFormatDetails(image->format),
            SDL_GetSurfacePalette(image),
            (uint8_t)(transcolor.r * 255),
            (uint8_t)(transcolor.g * 255),
            (uint8_t)(transcolor.b * 255),
            (uint8_t)(transcolor.a * 255)));

    width  = image->w;
    height = image->h;

    // create the new texture
    texture = SDL_CreateTexture(renderer2d, SDL_PIXELFORMAT_UNKNOWN,
        SDL_TEXTUREACCESS_STATIC, image->w, image->h);

    if (texture == NULL)
    {
        return false;
    }

    SDL_PropertiesID properties = SDL_GetTextureProperties(texture);
    SDL_PixelFormat format = (SDL_PixelFormat)SDL_GetNumberProperty(properties,
        SDL_PROP_TEXTURE_FORMAT_NUMBER, SDL_PIXELFORMAT_UNKNOWN);
    SDL_Surface* surface = SDL_ConvertSurface(image, format);
    SDL_DestroySurface(image);

    if (surface == NULL)
    {
        SDL_DestroyTexture(texture);
        
        return false;
    }

    LOCKED_RECT pLockedRect;

    if (SDL_MUSTLOCK(surface))
    {
        if (SDL_LockSurface(surface) == SDL_FALSE)
        {
            SDL_DestroySurface(surface);
            SDL_DestroyTexture(texture);

            return false;
        }
    }
    
    pLockedRect.pBits = surface->pixels;
    pLockedRect.pitch = surface->pitch;

    if (SDL_UpdateTexture(texture, NULL, pLockedRect.pBits, pLockedRect.pitch) ==
        SDL_FALSE)
    {
        SDL_DestroySurface(surface);
        SDL_DestroyTexture(texture);
        
        return false;
    }

    if (SDL_MUSTLOCK(surface))
    {
        SDL_UnlockSurface(surface);
    }

    SDL_DestroySurface(surface);

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
    SDL_Surface* image = IMG_Load(filename);

    if (image == NULL)
    {
        return false;
    }

    SDL_SetSurfaceColorKey(image, SDL_TRUE,
        SDL_MapRGBA(SDL_GetPixelFormatDetails(image->format),
            SDL_GetSurfacePalette(image),
            (uint8_t)(transcolor.r * 255),
            (uint8_t)(transcolor.g * 255),
            (uint8_t)(transcolor.b * 255),
            (uint8_t)(transcolor.a * 255)));

    width = image->w;
    height = image->h;

    // create the new texture
    texture = SDL_CreateTexture(renderer2d, SDL_PIXELFORMAT_UNKNOWN,
        SDL_TEXTUREACCESS_STREAMING, image->w, image->h);

    if (texture == NULL)
    {
        return false;
    }

    SDL_PropertiesID properties = SDL_GetTextureProperties(texture);
    SDL_PixelFormat format = (SDL_PixelFormat)SDL_GetNumberProperty(properties,
        SDL_PROP_TEXTURE_FORMAT_NUMBER, SDL_PIXELFORMAT_UNKNOWN);
    SDL_Surface* surface = SDL_ConvertSurface(image, format);
    SDL_DestroySurface(image);

    if (surface == NULL)
    {
        SDL_DestroyTexture(texture);

        return false;
    }

    LOCKED_RECT pLockedRect;

    if (SDL_MUSTLOCK(surface))
    {
        if (SDL_LockSurface(surface) == SDL_FALSE)
        {
            SDL_DestroySurface(surface);
            SDL_DestroyTexture(texture);

            return false;
        }
    }

    pLockedRect.pBits = surface->pixels;
    pLockedRect.pitch = surface->pitch;

    if (SDL_UpdateTexture(texture, NULL, pLockedRect.pBits, pLockedRect.pitch) ==
        SDL_FALSE)
    {
        SDL_DestroySurface(surface);
        SDL_DestroyTexture(texture);

        return false;
    }

    if (SDL_MUSTLOCK(surface))
    {
        SDL_UnlockSurface(surface);
    }

    SDL_DestroySurface(surface);

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
        rect.x = pRect->min[0];
        rect.y = pRect->min[1];
        rect.w = pRect->max[0] - pRect->min[0];
        rect.h = pRect->max[1] - pRect->min[1];
    }

    if (SDL_LockTexture(texture, &rect, &pLockedRect->pBits,
        &pLockedRect->pitch) == SDL_FALSE)
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
void Graphics::drawPoint(int16_t x, int16_t y, uint8_t width, COLOR_ARGB color)
{
    x -= width >> 1;
    y -= width >> 1;

    SDL_SetRenderDrawColorFloat(renderer2d, color.r, color.g, color.b, color.a);

    for (size_t i = 0; i < width; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            SDL_RenderPoint(renderer2d, (float)x + i, (float)y + j);
        }
    }

    SDL_SetRenderDrawColorFloat(renderer2d, 1.0f, 1.0f, 1.0f, 1.0f);
}

//=============================================================================
// Draw pixels from list
//=============================================================================
void Graphics::drawPoint(const vector2_t* pointList,
    unsigned long pointListCount, uint8_t width, COLOR_ARGB color)
{
    if (pointList == NULL || pointListCount == 0)
    {
        return;
    }

    int16_t x = 0;
    int16_t y = 0;

    SDL_SetRenderDrawColorFloat(renderer2d, color.r, color.g, color.b, color.a);

    for (size_t i = 0; i < pointListCount - 1; i++)
    {
        x = (int16_t)pointList[i + 0].x - (width >> 1);
        y = (int16_t)pointList[i + 0].y - (width >> 1);

        for (size_t j = 0; j < width; j++)
        {
            for (size_t k = 0; k < width; k++)
            {
                SDL_RenderPoint(renderer2d, (float)x + j, (float)y + k);
            }
        }
    }

    SDL_SetRenderDrawColorFloat(renderer2d, 1.0f, 1.0f, 1.0f, 1.0f);
}

//=============================================================================
// Draw line from X1,Y1 to X2,Y2.
//=============================================================================
void Graphics::drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2,
    uint8_t width, COLOR_ARGB color)
{
    const float dy = (float)(y2 - y1);
    const float dx = (float)(x2 - x1);
    const float hw = width / 2.0f;
    const vector2_t wn = NormalizeVector2(Vector2(dy, dx));

    // world, view and projection transform
    vector3_t p1 = TransformVector3Coord(Vector3(x1, y1, 1.0f), transform3d);
    vector3_t p2 = TransformVector3Coord(Vector3(x2, y2, 1.0f), transform3d);

    vector3_t v0 = Vector3(p1.x - (hw * wn.x), p1.y + (hw * wn.y), p1.z);
    vector3_t v1 = Vector3(p2.x - (hw * wn.x), p2.y + (hw * wn.y), p2.z);
    vector3_t v2 = Vector3(p2.x + (hw * wn.x), p2.y - (hw * wn.y), p2.z);
    vector3_t v3 = Vector3(p1.x + (hw * wn.x), p1.y - (hw * wn.y), p1.z);

    SDL_FColor c = {
        color.r,
        color.g,
        color.b,
        color.a
    };

    SDL_Vertex vertex0 = {
        { v0.x, v0.y },
        { c }
    };

    SDL_Vertex vertex1 = {
        { v1.x, v1.y },
        { c }
    };

    SDL_Vertex vertex2 = {
        { v2.x, v2.y },
        { c }
    };

    SDL_Vertex vertex3 = {
        { v3.x, v3.y },
        { c }
    };

    const SDL_Vertex vertice[4] = { vertex0, vertex1, vertex2, vertex3 };
    static const int indices[6] = { 0, 1, 2, 2, 3, 0 };
    
    SDL_RenderGeometry(renderer2d, NULL,
        vertice, sizeof(vertice) / sizeof(vertice[0]),
        indices, sizeof(indices) / sizeof(indices[0]));
}

//=============================================================================
// Draw lines, from vertex list
//=============================================================================
void Graphics::drawLine(const vector2_t* vertexList,
    unsigned long vertexListCount, uint8_t width, COLOR_ARGB color)
{
    if (vertexList == NULL || vertexListCount == 0)
    {
        return;
    }

    int16_t x1 = 0;
    int16_t y1 = 0;
    int16_t x2 = 0;
    int16_t y2 = 0;

    for (size_t i = 0; i < vertexListCount - 1; i++)
    {
        x1 = (int16_t)vertexList[i + 0].x;
        y1 = (int16_t)vertexList[i + 0].y;
        x2 = (int16_t)vertexList[i + 1].x;
        y2 = (int16_t)vertexList[i + 1].y;

        drawLine(x1, y1, x2, y2, width, color);
    }
}

//=============================================================================
// Display a quad (rectangle) with alpha transparency.
//=============================================================================
void Graphics::drawQuad(const vector4_t v0, const vector4_t v1,
    const vector4_t v2, const vector4_t v3, COLOR_ARGB color)
{
    // world, view and projection transform
    vector4_t p0 = TransformVector4(v0, transform3d);
    vector4_t p1 = TransformVector4(v1, transform3d);
    vector4_t p2 = TransformVector4(v2, transform3d);
    vector4_t p3 = TransformVector4(v3, transform3d);

    SDL_FColor c = {
        color.r,
        color.g,
        color.b,
        color.a
    };

    SDL_Vertex vertex0 = {
        { p0.x, p0.y },
        { c }
    };

    SDL_Vertex vertex1 = {
        { p1.x, p1.y },
        { c }
    };

    SDL_Vertex vertex2 = {
        { p2.x, p2.y },
        { c }
    };

    SDL_Vertex vertex3 = {
        { p3.x, p3.y },
        { c }
    };

    const SDL_Vertex vertice[4] = { vertex0, vertex1, vertex2, vertex3 };
    static const int indices[6] = { 0, 1, 2, 2, 3, 0 };
    
    SDL_RenderGeometry(renderer2d, NULL,
        vertice, sizeof(vertice) / sizeof(vertice[0]),
        indices, sizeof(indices) / sizeof(indices[0]));
}

//=============================================================================
// Display a quads (rectangle) from quad list with alpha transparency.
//=============================================================================
void Graphics::drawQuad(const vector4_t* quadList[4], uint32_t quadListCount,
    COLOR_ARGB color)
{
    if (quadList == NULL || quadListCount == 0)
    {
        return;
    }

    for (size_t i = 0; i < quadListCount; i++)
    {
        drawQuad(quadList[i][0], quadList[i][1], quadList[i][2], quadList[i][3],
            color);
    }
}

//=============================================================================
// Draw the sprite described in SpriteData structure
// Color is optional, it is applied like a filter, WHITE is default (no change)
//=============================================================================
void Graphics::drawSprite(const SpriteData& spriteData, COLOR_ARGB color)
{
    if (spriteData.texture == NULL)
    {
        return;
    }

    // Find center of sprite
    vector2_t spriteCenter = Vector2(
        ((float)(spriteData.w) / 2.0f) * spriteData.scale,
        ((float)(spriteData.h) / 2.0f) * spriteData.scale
    );

    // Screen position of the sprite
    vector2_t translate = Vector2(spriteData.x, spriteData.y);
    
    // Scaling X,Y
    vector2_t scaling = Vector2(spriteData.scale, spriteData.scale);

    if ((spriteData.effect & SPRITEEFFECT_FLIPH) == SPRITEEFFECT_FLIPH)
    {
        scaling.x *= -1;            // negative X scale to flip
        // Get center of flipped image.
        spriteCenter.x -= (float)(spriteData.w * spriteData.scale);
        // Flip occurs around left edge, translate right to put
        // Flipped image in same location as original.
        translate.x += (float)(spriteData.w * spriteData.scale);
    }

    if ((spriteData.effect & SPRITEEFFECT_FLIPV) == SPRITEEFFECT_FLIPV)
    {
        scaling.y *= -1;            // negative Y scale to flip
        // Get center of flipped image
        spriteCenter.y -= (float)(spriteData.h * spriteData.scale);
        // Flip occurs around top edge, translate down to put
        // Flipped image in same location as original.
        translate.y += (float)(spriteData.h * spriteData.scale);
    }

    // Create a matrix to rotate, scale and position our sprite
    matrix4_t matrix = Transformation2DMatrix4(
        Vector2(),          // keep origin at top left when scaling
        scaling,            // scale amount
        spriteCenter,           // rotation center
        spriteData.angle,           // rotation angle
        translate);         // X,Y location

    sprite->SetTransform(&matrix);
    sprite->Draw(spriteData.texture, &spriteData.rect, NULL, NULL, color);
}

//=============================================================================
// Draw sprites from sprite list.
//=============================================================================
void Graphics::drawSprite(const SpriteData* spriteList,
    unsigned long spriteListCount, COLOR_ARGB color)
{
    if (spriteList == NULL || spriteListCount == 0)
    {
        return;
    }

    for (size_t i = 0; i < spriteListCount; i++)
    {
        drawSprite(spriteList[i], color);
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
SDL_Rect Graphics::getWindowRect()
{
    return windowRect;
}

//=============================================================================
// Return viewport.
//=============================================================================
viewport_t Graphics::get3DViewport()
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
// Return sprite.
//=============================================================================
LP_SPRITE Graphics::getSprite()
{
    return sprite;
}

//=============================================================================
// return width
//=============================================================================
int Graphics::getWidth()
{
    return width;
}

//=============================================================================
// return height
//=============================================================================
int Graphics::getHeight()
{
    return height;
}

//=============================================================================
// Return fullscreen
//=============================================================================
bool Graphics::getFullscreen()
{
    return fullscreen;
}

//=============================================================================
// Returns true if the graphics card supports a stencil buffer
//=============================================================================
bool Graphics::getStencilSupport()
{
    return stencilSupport;
}

//=============================================================================
// Returns transform
//=============================================================================
void Graphics::getTransform(matrix4_t& matrix, TRANSFORMTYPE type)
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
void Graphics::spriteBegin()
{
    sprite->Begin(SPRITE_ALPHABLEND);
}

//=============================================================================
// Sprite End
//=============================================================================
void Graphics::spriteEnd()
{
    sprite->End();
}