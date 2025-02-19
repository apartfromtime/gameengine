#pragma once
#include <string>
#include <sstream>
#include "game.h"
#include "textureManager.h"
#include "image.h"
#include "textSDL.h"

//=============================================================================
// This class is the core of the game
//=============================================================================
class CreateThis : public Game
{
private:
    // game items
    TextureManager menuTexture, backgroundTexture; // textures
    Image       menu;               // menu image
    Image       background;         // background image
    TextSDL*    sdlFont;            // SDL font
    std::string  message;
    float messageY;

public:
    // Constructor
    CreateThis();
    // Destructor
    virtual ~CreateThis();
    // Start up game
    bool init();
    // Shutdown game
    void destroy() {
        safeDelete(sdlFont);
    };
    void update();      // must override pure virtual from Game
    void ai();          // "
    void collisions();  // "
    void render();      // "
};

