#pragma once
#include <vector>
#include <string>
#include "constants.h"
#include "graphics.h"

class TextureManager
{
    // TextureManager properties
private:
    // System
    Graphics* graphics;

    // Texture Manager
    std::vector<unsigned int> width;
    std::vector<unsigned int> height;
    std::vector<LP_TEXTURE> texture;
    std::vector<std::string> fileNames;
    bool initialized;

    // (For internal use only. No user serviceable parts inside.)

    // extract characters from stream until end of line
    bool getLine(SDL_IOStream* iostream, std::string& str);

public:
    // Constructor
    TextureManager();

    // Destructor
    ~TextureManager();

    // Returns a pointer to texture n
    LP_TEXTURE getTexture(unsigned int n = 0) const;

    // Returns the width of texture n
    unsigned int getW(unsigned int n = 0) const;

    // Return the height of texture n
    unsigned int getH(unsigned int n = 0) const;

    // Initialize the textureManager
    bool initialize(Graphics* pGraphics, std::string file);

    // Release resources, all texture memory is released.
    void onLostDevice();

    // Restore resourses, all textures are reloaded.
    void onResetDevice();

    // Safely release texture
    void safeReleaseTexture(LP_TEXTURE& ptr);
};

