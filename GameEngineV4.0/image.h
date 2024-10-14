#pragma once
#include "constants.h"
#include "graphics.h"
#include "textureManager.h"

class Image
{
    // Image properties
protected:
    // System
    Graphics* graphics;
    // Animation
    int     startFrame;     // first frame of current animation
    int     endFrame;       // end frame of current animation
    int     currentFrame;   // current frame of animation
    float   frameDelay;     // how long between frames of animation
    float   animTimer;      // animation timer
    bool    loop;           // true to loop frames
    bool    animComplete;   // true when loop is false and endFrame has finished displaying
    bool    animate;        // true to animate, else static
    // Image
    TextureManager* textureM;   // pointer to texture manager
    // spriteData contains the data required to draw the image by Graphics::drawSprite()
    SpriteData spriteData;  // SpriteData is defined in "sdlgraphics.h"
    color_t colorFilter;    // applied as a color filter (use WHITE for no change)
    int     cols;           // number of cols (1 to n) in multi-frame sprite
    int     rows;           // number of rows (1 to n) in multi-frame sprite
    bool    visible;        // true when visible
    bool    initialized;    // true when successfully initialized

public:
    // Constructor
    Image();

    // Destructor
    ~Image();

    ////////////////////////////////////////
    //           Get functions            //
    ////////////////////////////////////////

    // Return reference to SpriteData structure.
    const SpriteData& getSpriteInfo();          // for backward compatibility
    const SpriteData& getSpriteData();

    // Return visible parameter.
    bool getVisible();

    // Return X position.
    float getX();

    // Return Y position.
    float getY();

    // Return Z position.
    float getZ();

    // Return scale factor.
    float getScale();

    // Return width.
    int getW();

    // Return height.
    int getH();

    // Return center X.
    float getCenterX();

    // Return center Y.
    float getCenterY();

    // Return rotation angle in degrees.
    float getDegrees();

    // Return rotation angle in radians.
    float getRadians();

    // Return delay between frames of animation.
    float getFrameDelay();

    // Return number of starting frame.
    int getStartFrame();

    // Return number of ending frame.
    int getEndFrame();

    // Return number of current frame.
    int getCurrentFrame();

    // Return RECT structure of Image.
    rect_t getSpriteDataRect();

    // Return state of animation complete.
    bool getAnimationComplete();

    // Return colorFilter.
    color_t getColorFilter();

    // Set textureNumber
    TextureManager* getTextureManager();

    // Return animation timer.
    float getAnimTimer();

    ////////////////////////////////////////
    //           Set functions            //
    ////////////////////////////////////////

    // Set X location.
    void setX(float newX);

    // Set Y location.
    void setY(float newY);

    // Set Z location.
    void setZ(float newZ);

    // Set width.
    void setW(int newW);

    // Set height.
    void setH(int newH);

    // Set scale.
    void setScale(float s);

    // Set X location.
    void setCenterX(float newX);

    // Set Y location.
    void setCenterY(float newY);

    // Set rotation angle in degrees.
    // 0 degrees is up. Angles progress clockwise.
    void setDegrees(float deg);

    // Set rotation angle in radians.
    // 0 radians is up. Angles progress clockwise.
    void setRadians(float rad);

    // Set visible.
    void setVisible(bool v);

    // Set delay between frames of animation.
    void setFrameDelay(float d);

    // Set starting and ending frames of animation.
    void setFrames(int s, int e);

    // Set current frame of animation.
    void setCurrentFrame(int c);

    // Set spriteData.rect to draw currentFrame
    void setRect();

    // Set spriteData.rect to r.
    void setSpriteDataRect(rect_t r);

    // Set animation loop. lp = true to loop.
    void setLoop(bool lp);

    // Set animation complete Boolean.
    void setAnimationComplete(bool a);

    // Set color filter. (use WHITE for no change)
    void setColorFilter(color_t color);

    // Set TextureManager
    void setTextureManager(TextureManager* textureM);

    // Set animation timer
    void setAnimTimer(float t);

    ////////////////////////////////////////
    //         Other functions            //
    ////////////////////////////////////////

    // Initialize Image
    bool initialize(Graphics* pGraphics, int width, int height, int ncols, int nrows,
        TextureManager* textureM);

    // Flip image horizontally (mirror)
    void flipHorizontal(bool flip);

    // Flip image vertically
    void flipVertical(bool flip);

    // Draw Image using color as filter. Default color is WHITE.
    // textureN is number of texture in textureManager 
    void draw(COLOR_ARGB color, unsigned int textureN);

    // Draw Image using color as filter. Default color is WHITE.
    void draw(COLOR_ARGB color = graphicsNS::WHITE);

    // Draw Image using default color filter.
    void draw(unsigned int textureN);

    // Draw this image using the specified SpriteData.
    // The current SpriteData.rect is used to select the texture.
    // textureN is number of texture in textureManager 
    void draw(SpriteData sd, COLOR_ARGB color = graphicsNS::WHITE,
        unsigned int textureN = 0);

    // Update the animation. frameTime is used to regulate the speed.
    void update(float frameTime);
};

