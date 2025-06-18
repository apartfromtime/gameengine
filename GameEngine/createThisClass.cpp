#include "createThisClass.h"

const char MENU_IMAGE[] = "pictures\\menu.png";      // menu texture
const char BACKGROUND_IMAGE[] = "pictures\\background.png";  // background

//=============================================================================
// Constructor
//=============================================================================
CreateThis::CreateThis() : Game()
{
    sdlFont = new TextSDL();  // DirectX font
    messageY = 0;
}

//=============================================================================
// Destructor
//=============================================================================
CreateThis::~CreateThis()
{
}

//=============================================================================
// Initializes the game
// Throws GameError on error
//=============================================================================
bool CreateThis::init()
{
    // background texture
    if (!backgroundTexture.initialize(graphics, BACKGROUND_IMAGE))
    {
        GameError(gameErrorNS::FATAL_ERROR, "Error initializing background texture");
        return false;
    }

    // menu texture
    if (!menuTexture.initialize(graphics, MENU_IMAGE))
    {
        GameError(gameErrorNS::FATAL_ERROR, "Error initializing menu texture");
        return false;
    }

    // background image
    if (!background.initialize(graphics, 0, 0, 0, 0, &backgroundTexture))
    {
        GameError(gameErrorNS::FATAL_ERROR, "Error initializing background");
        return false;
    }

    // menu image
    if (!menu.initialize(graphics, 0, 0, 0, 0, &menuTexture))
    {
        GameError(gameErrorNS::FATAL_ERROR, "Error initializing menu");
        return false;
    }

    // initialize DirectX font
    // 18 pixel high Arial
    if (sdlFont->initialize(graphics, 15, true, false, "arial.ttf") == false)
    {
        GameError(gameErrorNS::FATAL_ERROR, "Error initializing DirectX font");
        return false;
    }

    menu.setDegrees(300);
    menu.setScale(0.002861f);

    message = "\n\n\nUtilizes Object Oriented C++ and DirectX\n\n";
    message += "Sprites with Transparency\n\n";
    message += "Collision Detection Supported:\n";
    message += "     - Circular (Distance)\n";
    message += "     - Axis-aligned bounding box\n";
    message += "     - Rotated bounding box\n";
    message += "     - Rotated bounding box and circle\n";
    message += "     - Pixel Perfect\n\n";
    message += "XACT Audio\n\n";
    message += "Sprite and DirectX Text\n\n";
    message += "Tile Based Graphics\n\n";
    message += "Xbox 360 Controller Input\n\n";
    message += "TCP/IP and UDP/IP Network Support\n\n";
    messageY = GAME_HEIGHT;

    return true;
}

//=============================================================================
// Update all game items
//=============================================================================
void CreateThis::update(float frameTime)
{
    static float delay = 0;
    delay += frameTime;

    if (menu.getDegrees() > 0)
    {
        menu.setDegrees(menu.getDegrees() - frameTime * 120);
        menu.setScale(menu.getScale() + frameTime * 0.4f);
    }
    else
        menu.setDegrees(0);

    if (delay > 15)           // start over
    {
        menu.setDegrees(300);
        menu.setScale(0.002861f);
        menu.setY(0);
        delay = 0;
        messageY = GAME_HEIGHT;
    }
    else if (delay > 5)
    {
        menu.setY(menu.getY() - frameTime * 300);
        if (messageY > 10)
            messageY -= frameTime * 300;
    }
}

//=============================================================================
// Artificial Intelligence
//=============================================================================
void CreateThis::ai(float frameTime)
{}

//=============================================================================
// Handle collisions
//=============================================================================
void CreateThis::collisions(float frameTime)
{}

//=============================================================================
// Render game items
//=============================================================================
void CreateThis::render()
{
    graphics->spriteBegin(SPRITE_ALPHABLEND);                // begin drawing sprites
    background.draw(graphicsNS::ALPHA50);
    menu.draw();
    sdlFont->setFontColor(graphicsNS::ORANGE);
    sdlFont->print(message, 20, (int)messageY);

    graphics->spriteEnd();                  // end drawing sprites
}
