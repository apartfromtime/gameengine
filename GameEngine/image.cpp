#include "image.h"

//=============================================================================
// default constructor
//=============================================================================
Image::Image()
{
    // System
    graphics = NULL;
    // Animation
    startFrame = 0;
    endFrame = 0;
    currentFrame = 0;
    frameDelay = 1.0f;
    animTimer = 0.0f;
    loop = true;
    animComplete = false;
    animate = true;
    // Image
    textureM = NULL;
    initialized = false;
    spriteData.effect = SPRITEEFFECT_NONE;
    spriteData.w = 2;
    spriteData.h = 2;
    spriteData.x = 0.0f;
    spriteData.y = 0.0f;
    spriteData.z = 1.0f;
    spriteData.depth = 1.0f;
    spriteData.scale = 1.0f;
    spriteData.angle = 0.0f;
    spriteData.rect.min.x = 0;
    spriteData.rect.min.y = 0;
    spriteData.rect.max.y = (float)spriteData.w;
    spriteData.rect.max.y = (float)spriteData.h;
    spriteData.texture = NULL;
    cols = 1;
    rows = 1;
    visible = true;
    colorFilter = graphicsNS::WHITE;
}

//=============================================================================
// destructor
//=============================================================================
Image::~Image()
{
}

////////////////////////////////////////
//           Get functions            //
////////////////////////////////////////

//=============================================================================
// Return reference to SpriteData structure.
//=============================================================================
const SpriteData& Image::getSpriteInfo()
{
    return spriteData;
}

const SpriteData& Image::getSpriteData()
{
    return spriteData;
}

//=============================================================================
// Return visible parameter.
//=============================================================================
bool Image::getVisible() const
{
    return visible;
}

//=============================================================================
// Return X position.
//=============================================================================
float Image::getX() const
{
    return spriteData.x;
}

//=============================================================================
// Return Y position.
//=============================================================================
float Image::getY() const
{
    return spriteData.y;
}

//=============================================================================
// Return Z position.
//=============================================================================
float Image::getZ() const
{
    return spriteData.z;
}

//=============================================================================
// Return scale factor.
//=============================================================================
float Image::getScale() const
{
    return spriteData.scale;
}

//=============================================================================
// Return width.
//=============================================================================
int Image::getW() const
{
    return spriteData.w;
}

//=============================================================================
// Return height.
//=============================================================================
int Image::getH() const
{
    return spriteData.h;
}

//=============================================================================
// Return center X.
//=============================================================================
float Image::getCenterX() const
{
    return (float)(spriteData.w / 2) * getScale();
}

//=============================================================================
// Return center Y.
//=============================================================================
float Image::getCenterY() const
{
    return (float)(spriteData.h / 2) * getScale();
}

//=============================================================================
// Return rotation angle in degrees.
//=============================================================================
float Image::getDegrees() const
{
    return spriteData.angle * (180.0f / (float)M_PI);
}

//=============================================================================
// Return rotation angle in radians.
//=============================================================================
float Image::getRadians() const
{
    return spriteData.angle;
}

//=============================================================================
// Return delay between frames of animation.
//=============================================================================
float Image::getFrameDelay() const
{
    return frameDelay;
}

//=============================================================================
// Return number of starting frame.
//=============================================================================
int Image::getStartFrame() const
{
    return startFrame;
}

//=============================================================================
// Return number of ending frame.
//=============================================================================
int Image::getEndFrame() const
{
    return endFrame;
}

//=============================================================================
// Return number of current frame.
//=============================================================================
int Image::getCurrentFrame() const
{
    return currentFrame;
}

//=============================================================================
// Return RECT structure of Image.
//=============================================================================
rect_t Image::getSpriteDataRect() const
{
    return spriteData.rect;
}

//=============================================================================
// Return state of animation complete.
//=============================================================================
bool Image::getAnimationComplete() const
{
    return animComplete;
}

//=============================================================================
// Return colorFilter.
//=============================================================================
color_t Image::getColorFilter() const
{
    return colorFilter;
}

//=============================================================================
// Return texture.
//=============================================================================
TextureManager* Image::getTextureManager()
{
    return textureM;
}

//=============================================================================
// Return animation timer.
//=============================================================================
float Image::getAnimTimer() const
{
    return animTimer;
}

////////////////////////////////////////
//           Set functions            //
////////////////////////////////////////

//=============================================================================
// Set width.
//=============================================================================
void Image::setW(int newW)
{
    spriteData.w = newW;
}

//=============================================================================
// Set height.
//=============================================================================
void Image::setH(int newH)
{
    spriteData.h = newH;
}

//=============================================================================
// Set X location.
//=============================================================================
void Image::setX(float newX)
{
    spriteData.x = newX;
}

//=============================================================================
// Set Y location.
//=============================================================================
void Image::setY(float newY)
{
    spriteData.y = newY;
}

//=============================================================================
// Set Z location.
//=============================================================================
void Image::setZ(float newZ)
{
    spriteData.z = newZ;
}

//=============================================================================
// Set scale.
//=============================================================================
void Image::setScale(float s)
{
    spriteData.scale = s;
}

//=============================================================================
// Set X location.
//=============================================================================
void Image::setCenterX(float newX)
{
    spriteData.x = newX - ((float)(spriteData.w / 2) * getScale());
}

//=============================================================================
// Set Y location.
//=============================================================================
void Image::setCenterY(float newY)
{
    spriteData.y = newY - ((float)(spriteData.h / 2) * getScale());
}


//=============================================================================
// Set rotation angle in degrees.
//=============================================================================
void Image::setDegrees(float deg)
{
    spriteData.angle = deg * ((float)M_PI / 180.0f);
}

//=============================================================================
// Set rotation angle in radians.
// 0 radians is up. Angles progress clockwise.
//=============================================================================
void Image::setRadians(float rad)
{
    spriteData.angle = rad;
}

//=============================================================================
// Set visible.
//=============================================================================
void Image::setVisible(bool v)
{
    visible = v;
}

//=============================================================================
// Set delay between frames of animation.
//=============================================================================
void Image::setFrameDelay(float d)
{
    frameDelay = d;
}

//=============================================================================
// Set starting and ending frames of animation.
//=============================================================================
void Image::setFrames(int s, int e)
{
    startFrame = s; endFrame = e;
}

//=============================================================================
// Set the current frame of the image
//=============================================================================
void Image::setCurrentFrame(int c)
{
    if (c >= 0)
    {
        currentFrame = c;
        animComplete = false;
        animTimer = 0;
        setRect();
    }
}

//=============================================================================
// Set spriteData.rect to draw currentFrame
//=============================================================================
inline void Image::setRect()
{
    spriteData.rect.min.x = (float)(currentFrame % cols) * spriteData.w;
    spriteData.rect.max.x = (float)spriteData.w;
    spriteData.rect.min.y = (float)(currentFrame / rows) * spriteData.h;
    spriteData.rect.max.y = (float)spriteData.h;
}

//=============================================================================
// Set spriteData.rect to r.
//=============================================================================
void Image::setSpriteDataRect(rect_t r)
{
    spriteData.rect = r;
}

//=============================================================================
// Set animation loop. lp = true to loop.
//=============================================================================
void Image::setLoop(bool lp)
{
    loop = lp;
}

//=============================================================================
// Set animation complete Boolean.
//=============================================================================
void Image::setAnimationComplete(bool a)
{
    animComplete = a;
};

//=============================================================================
// Set color filter. (use WHITE for no change)
//=============================================================================
void Image::setColorFilter(color_t color)
{
    colorFilter = color;
}

//=============================================================================
// Set TextureManager
//=============================================================================
void Image::setTextureManager(TextureManager* pTextureM)
{
    textureM = pTextureM;
}

//=============================================================================
// Set Animation Timer
//=============================================================================
void Image::setAnimTimer(float t)
{
    animTimer = t;
}

//=============================================================================
// Initialize the Image.
//=============================================================================
bool Image::initialize(Graphics* pGraphics, int w, int h, int ncols, int nrows,
    TextureManager* pTextureM)
{
    graphics = pGraphics;
    textureM = pTextureM;

    if (w == 0)
    {
        w = textureM->getW();
    }

    if (h == 0)
    {
        h = textureM->getH();
    }

    spriteData.texture = textureM->getTexture();
    spriteData.w = w;
    spriteData.h = h;
    cols = ncols;
    rows = nrows;

    if (cols == 0)
    {
        cols = 1;
    }

    if (rows == 0)
    {
        rows = 1;
    }

    setRect();

    initialized = true;

    return true;
}

//=============================================================================
// Flip image horizontally (mirror)
//=============================================================================
void Image::flipHorizontal(bool flip)
{
    if (flip == false)
    {
        spriteData.effect &= ~SPRITEEFFECT_FLIPH;
    }
    else
    {
        spriteData.effect |= SPRITEEFFECT_FLIPH;
    }
}

//=============================================================================
// Flip image vertically
//=============================================================================
void Image::flipVertical(bool flip)
{
    if (flip == false)
    {
        spriteData.effect &= ~SPRITEEFFECT_FLIPV;
    }
    else
    {
        spriteData.effect |= SPRITEEFFECT_FLIPV;
    }
}

//=============================================================================
// Draw Image using color as filter.
//=============================================================================
void Image::draw(COLOR_ARGB color, unsigned int textureN)
{
    if (!visible || graphics == NULL)
    {
        return;
    }

    // set texture to draw
    spriteData.texture = textureM->getTexture(textureN);
    
    if (color.r == graphicsNS::FILTER.r &&
        color.g == graphicsNS::FILTER.g &&
        color.b == graphicsNS::FILTER.b &&
        color.a == graphicsNS::FILTER.a)
    {
        graphics->drawSprite(spriteData, colorFilter);
    }
    else
    {
        graphics->drawSprite(spriteData, color);
    }
}

//=============================================================================
// Draw the image using color as filter
//=============================================================================
void Image::draw(COLOR_ARGB color)
{
    draw(color, 0);
}

//=============================================================================
// Draw Image using default color filter.
//=============================================================================
void Image::draw(unsigned int textureN)
{
    draw(graphicsNS::WHITE, textureN);
}


//=============================================================================
// Draw this image using the specified SpriteData.
//=============================================================================
void Image::draw(SpriteData sd, COLOR_ARGB color, unsigned int textureN)
{
    if (!visible || graphics == NULL)
    {
        return;
    }

    sd.texture = textureM->getTexture(textureN);
    sd.rect = spriteData.rect;          // use this Images rect to select texture

    if (color.r == graphicsNS::FILTER.r &&
        color.g == graphicsNS::FILTER.g &&
        color.b == graphicsNS::FILTER.b &&
        color.a == graphicsNS::FILTER.a)
    {
        graphics->drawSprite(sd, colorFilter);
    }
    else
    {
        graphics->drawSprite(sd, color);
    }
}

//=============================================================================
// update
//=============================================================================
void Image::update(float frameTime)
{
    if (animate == true && endFrame - startFrame > 0)
    {
        animTimer += frameTime;         // total elapsed time

        if (animTimer > frameDelay)
        {
            animTimer -= frameDelay;
            currentFrame++;

            if (currentFrame < startFrame || currentFrame > endFrame)
            {
                if (loop == true)
                {
                    currentFrame = startFrame;
                }
                else
                {
                    currentFrame = endFrame;
                    animComplete = true;
                }
            }
            setRect();
        }
    }
}
