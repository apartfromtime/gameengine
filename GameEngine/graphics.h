#pragma once
#include <SDL3\SDL.h>
#include <GEUL\g_geul.h>
#include "constants.h"
#include "gameError.h"

//-----------------------------------------------------------------------------
//
// GRAPHICS
//
//-----------------------------------------------------------------------------

// SDL pointer types
#define LP_DEVICE           SDL_Renderer*
#define LP_TEXTURE          SDL_Texture*
#define LP_VERTEX           SDL_Vertex*

// Color defines
#define COLOR_ARGB vector4_t

#define SETCOLOR_ARGB(a,r,g,b) (Vector4((r & 0xff) / 255.0f, (g & 0xff) / 255.0f, \
    (b & 0xff) / 255.0f, (a & 0xff) / 255.0f))

namespace graphicsNS
{
    // Some common colors
    // ARGB numbers range from 0 through 255
    // A = Alpha channel (transparency where 255 is opaque)
    // R = Red, G = Green, B = Blue
    const COLOR_ARGB ORANGE     = SETCOLOR_ARGB(255, 255, 165,   0);
    const COLOR_ARGB BROWN      = SETCOLOR_ARGB(255, 139,  69,  19);
    const COLOR_ARGB LTGRAY     = SETCOLOR_ARGB(255, 192, 192, 192);
    const COLOR_ARGB GRAY       = SETCOLOR_ARGB(255, 128, 128, 128);
    const COLOR_ARGB OLIVE      = SETCOLOR_ARGB(255, 128, 128,   0);
    const COLOR_ARGB PURPLE     = SETCOLOR_ARGB(255, 128,   0, 128);
    const COLOR_ARGB MAROON     = SETCOLOR_ARGB(255, 128,   0,   0);
    const COLOR_ARGB TEAL       = SETCOLOR_ARGB(255,   0, 128, 128);
    const COLOR_ARGB GREEN      = SETCOLOR_ARGB(255,   0, 128,   0);
    const COLOR_ARGB NAVY       = SETCOLOR_ARGB(255,   0,   0, 128);
    const COLOR_ARGB WHITE      = SETCOLOR_ARGB(255, 255, 255, 255);
    const COLOR_ARGB YELLOW     = SETCOLOR_ARGB(255, 255, 255,   0);
    const COLOR_ARGB MAGENTA    = SETCOLOR_ARGB(255, 255,   0, 255);
    const COLOR_ARGB RED        = SETCOLOR_ARGB(255, 255,   0,   0);
    const COLOR_ARGB CYAN       = SETCOLOR_ARGB(255,   0, 255, 255);
    const COLOR_ARGB LIME       = SETCOLOR_ARGB(255,   0, 255,   0);
    const COLOR_ARGB BLUE       = SETCOLOR_ARGB(255,   0,   0, 255);
    const COLOR_ARGB BLACK      = SETCOLOR_ARGB(255,   0,   0,   0);
    const COLOR_ARGB FILTER     = SETCOLOR_ARGB(  0,   0,   0,   0);            // use to specify drawing with colorFilter
    const COLOR_ARGB ALPHA25    = SETCOLOR_ARGB( 64, 255, 255, 255);            // AND with color to get 25% alpha
    const COLOR_ARGB ALPHA50    = SETCOLOR_ARGB(128, 255, 255, 255);            // AND with color to get 50% alpha
    const COLOR_ARGB ALPHA75    = SETCOLOR_ARGB(192, 255, 255, 255);            // AND with color to get 75% alpha
    const COLOR_ARGB BACK_COLOR = NAVY;          // background color of game
    const COLOR_ARGB TRANSCOLOR = SETCOLOR_ARGB(255, 255,   0, 255);  // transparent color (magenta)

    enum DISPLAY_MODE { DISPLAYMODE_TOGGLE, DISPLAYMODE_FULLSCREEN, DISPLAYMODE_WINDOW };
}

// Texture locked rectangle
typedef struct _LOCKED_RECT
{
    int pitch;
    void* pBits;
} LOCKED_RECT;

typedef struct _VERTEX
{
    vector4_t     position;         // Vertex position
    vector4_t     texcoord;         // Texture coordinates
    vector3_t     normal;           // Vertex normal
    vector4_t     color;            // Vertex color
} VERTEX;

//-----------------------------------------------------------------------------
//
// SPRITE
//
//-----------------------------------------------------------------------------

#define SPRITE_DONOTSAVESTATE               (1 << 0)
#define SPRITE_DONOTMODIFY_RENDERSTATE      (1 << 1)
#define SPRITE_OBJECTSPACE                  (1 << 2)
#define SPRITE_ALPHABLEND                   (1 << 3)

typedef enum
{
    SPRITEEFFECT_NONE       = 0x00,
    SPRITEEFFECT_FLIPH      = 0x01,
    SPRITEEFFECT_FLIPV      = 0x02
} SPRITEEFFECT;

// SpriteData: The properties required by Graphics::drawSprite to draw a sprite
struct SpriteData
{
    int         w;          // width of sprite in pixels
    int         h;          // height of sprite in pixels
    float       x;          // screen location (top left corner of sprite)
    float       y;
    float       z;
    float       depth;      // depth scale distance
    float       scale;      // <1 smaller, >1 bigger
    float       angle;      // rotation angle in radians
    rect_t      rect;       // used to select an image from a larger texture
    LP_TEXTURE  texture;    // pointer to texture
    uint32_t    effect;     // flip x, y
};

typedef enum
{
    TRANSFORMTYPE_WORLD         = 0x00,
    TRANSFORMTYPE_VIEW          = 0x01,
    TRANSFORMTYPE_PROJECTION    = 0x02,
    TRANSFORMTYPE_TRANSFORM     = 0x03
} TRANSFORMTYPE;

class Graphics
{
    // Graphics properties
private:
    // SDL
    SDL_Window* hwnd;
    SDL_Renderer* renderer2d;
    SDL_DisplayMode displayMode;
    unsigned long numberOfPixelsColliding;          // for pixel perfect collision detection
    SDL_Texture* depthStencilBuffer;
    bool stencilSupport;
    SDL_Rect windowRect;
    SDL_Rect viewport2d;
    SDL_BlendMode prevBlendMode;
    long flags;

    // Presentation parameters
    int backBufferWidth;
    int backBufferHeight;
    SDL_PixelFormat backBufferFormat;
    int backBufferCount;
    bool enableAutoDepthStencil;
    SDL_PixelFormat autoDepthStencilFormat;
    float fullScreenRefreshRateInHz;
    int presentationInterval;

    // View
    viewport_t viewport3d;
    matrix4_t matrix3d[3];            // world = 0, view = 1, projection = 2
    matrix4_t transform3d;

    // Window
    bool        vsync;
    bool        fullscreen;
    int         width;
    int         height;
    COLOR_ARGB  backColor;          // background color

    // (For internal use only. No user serviceable parts inside.)

    // Initialize SDL presentation parameters
    void initSDLpp();

public:
    // Constructor
    Graphics();

    // Destructor
    ~Graphics();

    // Releases direct3d and renderer3d.
    void releaseAll();

    // Initialize SDL graphics
    bool initialize(SDL_Window* hw, int width, int height, bool fullscreen, bool vsync);

    // Checks the adapter to see if it is compatible with the BackBuffer height,
    // width and refresh rate specified in sdlpp. Fills in the pMode structure with
    // the format of the compatible mode, if found.
    bool isAdapterCompatible();

    // Reset the graphics device.
    bool reset();

    // Toggle, fullscreen or window display mode
    void changeDisplayMode(graphicsNS::DISPLAY_MODE mode = graphicsNS::DISPLAYMODE_TOGGLE);

    // Display the offscreen backbuffer to the screen.
    bool showBackbuffer();

    // Locks a rectangle on a texture resource.
    bool lockRect(LP_TEXTURE& texture, LOCKED_RECT* pLockedRect, const rect_t* pRect = NULL);

    // Unlocks a rectangle on a texture resource.
    bool unlockRect(LP_TEXTURE& texture);

    // Frees the data associated with this texture
    void freeTexture(LP_TEXTURE texture);

    // Load the texture into default SDL memory (normal texture use)
    bool loadTexture(const char* filename, COLOR_ARGB transcolor,
        unsigned int& width, unsigned int& height, LP_TEXTURE& texture);

    // Load the texture into system memory (system memory is lockable)
    // Provides direct access to pixel data.
    bool loadTextureSystemMem(const char* filename, COLOR_ARGB transcolor,
        unsigned int& width, unsigned int& height, LP_TEXTURE& texture);

    // Draw pixel at (x, y). 
    //      color defaults to graphicsNS::WHITE.
    void drawPoint(int16_t x, int16_t y, uint8_t width = 1,
        COLOR_ARGB color = graphicsNS::WHITE);

    // Draw pixels from list
    //      color defaults to graphicsNS::WHITE.
    void drawPoint(const vector2_t* pointList, unsigned long pointListCount,
        uint8_t width, COLOR_ARGB color = graphicsNS::WHITE);

    // Draw line from (x1, y1) to (x2, y2). 
    //      width defauts to 1. 
    //      color defaults to graphicsNS::WHITE.
    void drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t width = 1,
        COLOR_ARGB color = graphicsNS::WHITE);

    // Draw lines, from vertex list
    //      width defaults to 1
    //      color defaults to graphicsNS::WHITE
    void drawLine(const vector2_t* vertexList, unsigned long vertexListCount,
        uint8_t width = 1, COLOR_ARGB color = graphicsNS::WHITE);

    // Display quad (rectangle) with alpha transparency.
    void drawQuad(const vector4_t v0, const vector4_t v1, const vector4_t v2,
        const vector4_t v3, COLOR_ARGB color = graphicsNS::WHITE);

    // Display quads (rectangle) from quad list with alpha transparency.
    void drawQuad(const vector4_t* quadList[4], uint32_t quadListCount,
        COLOR_ARGB color = graphicsNS::WHITE);

    // Display a quad (rectangle) textured.
    bool drawTetxuredQuad(SDL_Texture* pTexture, const rect_t* pSrcRect,
        const vector3_t* pCenter, const vector3_t* pPosition,
        COLOR_ARGB Color = graphicsNS::WHITE);

    // Draw the sprite described in SpriteData structure. (SDL_RenderGeometry)
    // color is optional, it is applied as a filter, WHITE is default (no change).
    void drawSprite(const SpriteData& spriteData,
        COLOR_ARGB color = graphicsNS::WHITE);

    // Draw sprites from sprite list.
    void drawSprite(const SpriteData* spriteList, unsigned long spriteListCount,
        COLOR_ARGB color = graphicsNS::WHITE);

    // Sprite Begin
    bool spriteBegin(long Flags);
    
    // Sprite End
    bool spriteEnd();

    // Return the number of pixels colliding between the two sprites.
    // Pre: The device supports a stencil buffer and pOcclusionQuery points to
    // a valid occlusionQuery object.
    // Post: Returns the number of pixels of overlap
    // This function waits for the graphics card to render the last frame and return
    // the collision query pixel count. To avoid slowing down your game, use a
    // simple collison test first to eliminate entities that are not colliding.
    unsigned long pixelCollision(const SpriteData& sprite1,
        const SpriteData& sprite2);

    // Multiplies world, view, or projection matrices
    void multiplyTransform();

    // get functions

    // return the window client area in pixels
    SDL_Rect getWindowRect() const;

    // return 3d viewport
    viewport_t get3DViewport() const;

    // Return renderer3d.
    SDL_Renderer* get2DRenderer();

    // Test for lost device
    bool getDeviceState();

    // return width and height
    int getWidth() const;
    int getHeight() const;

    // Return fullscreen
    bool getFullscreen() const;

    // Returns true if the graphics card supports a stencil buffer
    bool getStencilSupport() const;

    // Returns transform
    void getTransform(matrix4_t& matrix, TRANSFORMTYPE type) const;

    // Set VSync
    void setVSync(bool vsync);

    // Set color used to clear screen
    void setBackColor(COLOR_ARGB c);

    // Set transform
    void setTransform(const matrix4_t& matrix, TRANSFORMTYPE type);

    // Clear backbuffer and BeginScene()
    bool beginScene();

    // Forces all batched vertices to be submitted to the device.
    bool flush();

    // EndScene()
    bool endScene();
};

