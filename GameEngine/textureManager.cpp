#include "textureManager.h"

//=============================================================================
// default constructor
//=============================================================================
TextureManager::TextureManager()
{
    graphics = NULL;
    width.clear();
    height.clear();
    texture.clear();
    fileNames.clear();
    initialized = false;            // set true when successfully initialized
}

//=============================================================================
// destructor
//=============================================================================
TextureManager::~TextureManager()
{
    for (unsigned int i = 0; i < texture.size(); i++)
        safeReleaseTexture(texture[i]);
}

//=============================================================================
// Reads a line from io stream
//=============================================================================
bool TextureManager::getLine(SDL_IOStream* iostream, std::string& str)
{
    if (iostream == NULL || SDL_GetIOStatus(iostream) != SDL_IO_STATUS_READY)
    {
        return false;
    }

    unsigned char c = 0;
    SDL_ReadU8(iostream, &c);
    while (c == '\n' || c == '\r') { SDL_ReadU8(iostream, &c); };
    do {
        str += c;
        SDL_ReadU8(iostream, &c);
    } while (SDL_GetIOStatus(iostream) == SDL_IO_STATUS_READY &&
        (c != '\n' && c != '\r'));

    return true;
}

//=============================================================================
// Returns a pointer to texture n
//=============================================================================
LP_TEXTURE TextureManager::getTexture(unsigned int n) const
{
    if (n >= texture.size())
    {
        return NULL;
    }

    return texture[n];
}

//=============================================================================
// Returns the width of texture n
//=============================================================================
unsigned int TextureManager::getW(unsigned int n) const
{
    if (n >= texture.size())
    {
        return 0;
    }

    return width[n];
}

//=============================================================================
// Return the height of texture n
//=============================================================================
unsigned int TextureManager::getH(unsigned int n) const
{
    if (n >= texture.size())
    {
        return 0;
    }

    return height[n];
}

//=============================================================================
// Loads the texture file(s) from disk.
//=============================================================================
bool TextureManager::initialize(Graphics* pGraphics, std::string file)
{
    bool success = true;

    graphics = pGraphics;

    for (unsigned int i = 0; i < file.size(); i++)
    {
        file.at(i) = tolower(file.at(i));
    }

    if (file.rfind(".txt") == file.size() - 4)          // .txt extension
    {
        // open file containing individual texture names
        SDL_IOStream* infile = SDL_IOFromFile(file.c_str(), "rb");
        if (infile == NULL)
        {
            return false;
        }

        std::string name = "";
        while (getLine(infile, name))
        {
            fileNames.push_back(name);
            width.push_back(0);
            height.push_back(0);
            texture.push_back(NULL);
            name.clear();
        }
        SDL_CloseIO(infile);
    }
    else            // not .txt file so file contains name of one texture
    {
        fileNames.push_back(file);
        width.push_back(0);
        height.push_back(0);
        texture.push_back(NULL);
    }

    // load texture files
    for (unsigned int i = 0; i < fileNames.size(); i++)
    {
        bool result = graphics->loadTexture(fileNames[i].c_str(),
            graphicsNS::TRANSCOLOR, width[i], height[i], texture[i]);
        if (result == false)
        {
            success = false;            // at least one texture failed to load
        }
    }

    initialized = true;
    
    return success;
}

//=============================================================================
// called when graphics device is lost
//=============================================================================
void TextureManager::onLostDevice()
{
    if (!initialized)
    {
        return;
    }

    for (unsigned int i = 0; i < texture.size(); i++)
    {
        safeReleaseTexture(texture[i]);
    }
}

//=============================================================================
// called when graphics device is reset
//=============================================================================
void TextureManager::onResetDevice()
{
    if (!initialized)
        return;

    // load texture files
    for (unsigned int i = 0; i < fileNames.size(); i++)
    {
        bool result = graphics->loadTexture(fileNames[i].c_str(),
            graphicsNS::TRANSCOLOR, width[i], height[i], texture[i]);
        if (result == false)
        {
            safeReleaseTexture(texture[i]);
        }
    }
}

//=============================================================================
// Safely release texture
//=============================================================================
void TextureManager::safeReleaseTexture(LP_TEXTURE& ptr)
{
    if (ptr)
    {
        graphics->freeTexture(ptr);
        ptr = NULL;
    }
}
