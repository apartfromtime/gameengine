#pragma once
#include "constants.h"
#include "gameError.h"
#include "graphics.h"
#include "input.h"
#include "audio.h"
#include "net.h"
#include "image.h"
#include "entity.h"
#include "textSDL.h"
#include "console.h"
#include "messageDialog.h"
#include "inputDialog.h"

namespace gameNS
{
    const char FONT[] = "cour.ttf";
    const int POINT_SIZE = 14;
    const color_t FONT_COLOR = graphicsNS::WHITE;
    const int BUF_SIZE = 32;
}

class Game
{
    // Game properties
protected:
    // System
    SDL_Window* hwnd;
    Graphics* graphics;
    Input* input;
    Audio* audio;
    Console* console;
    MessageDialog* messageDialog;
    InputDialog* inputDialog;
    // Time
    uint64_t timeStart;         // Performance Counter start value
    uint64_t timeEnd;           // Performance Counter end value
    uint64_t timerFreq;         // Performance Counter frequency
    uint32_t sleepTime;         // number of milli-seconds to sleep between frames
    float frameTime;            // time required for last frame
    float   fps;            // frames per second
    // View
    viewport_t viewport3d;
    matrix4_t wrldMatrix;
    matrix4_t viewMatrix;
    matrix4_t projMatrix;
    float frustum[6];
    // Font
    TextSDL  font;          // Game font
    TextSDL  fontBig;           // font for game banners
    TextSDL  fontScore;         // font for score boards
    TextSDL* fontOther;         // font for games
    // Commands
    char    buffer[gameNS::BUF_SIZE];
    bool    fpsOn;         // true to display fps
    bool    paused;
    bool    initialized;

public:
    // Constructor
    Game();
    // Destructor
    virtual ~Game();

    // Member functions

    // Window message handler
    void messageHandler(const SDL_Window* hwnd, const SDL_Event* msg);

    // Initialize the game
    bool initialize(SDL_Window* phwnd);

    // Shutdown all game items
    void shutdown();

    // Call run repeatedly by the main message loop in main
    void run();

    // Call when the graphics device was lost.
    // Release all reserved video memory so graphics device may be reset.
    void releaseAll();

    // Recreate all surfaces and reset all entities.
    void resetAll();

    // Delete all reserved memory.
    void deleteAll();

    // Process console commands.
    void consoleCommand();

    // Render game items.
    void renderGame();

    // Set display mode (fullscreen, window or toggle)
    void setDisplayMode(graphicsNS::DISPLAY_MODE mode = graphicsNS::DISPLAYMODE_TOGGLE);

    // Exit the game
    void exitGame();

    // Return pointer to Console.
    Console* getConsole();

    // Return pointer to Graphics.
    Graphics* getGraphics();

    // Return pointer to Input.
    Input* getInput();

    // Return pointer to Audio.
    Audio* getAudio();

    // Pure virtual function declarations
    // These functions MUST be written in any class that inherits from Game

    // Start up game
    virtual bool init() = 0;

    // Shutdown game
    virtual void destroy() = 0;

    // Update game items.
    virtual void update() = 0;

    // Perform AI calculations.
    virtual void ai() = 0;

    // Check for collisions.
    virtual void collisions() = 0;

    // Render graphics.
    // Call graphics->spriteBegin();
    //   draw sprites
    // Call graphics->spriteEnd();
    //   draw non-sprites
    virtual void render() = 0;
};

class NullGame : public Game
{
public:
    // Constructor
    NullGame() {}
    // Destructor
    virtual ~NullGame() {}
    bool init() { return true; }
    void destroy() {}
    void update() {}      // must override pure virtual from Game
    void ai() {}          // "
    void collisions() {}  // "
    void render() {}      // "
};

