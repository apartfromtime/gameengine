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
    float fps;          // frames per second
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
    void run(float frameTime);

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
    virtual void update(float frameTime) = 0;

    // Perform AI calculations.
    virtual void ai(float frameTime) = 0;

    // Check for collisions.
    virtual void collisions(float frameTime) = 0;

    // Render graphics.
    virtual void render() = 0;
};

class NullGame : public Game
{
    float currentTime;
    float r, g, b, a;

public:
    // Constructor
    NullGame() {}
    // Destructor
    virtual ~NullGame() {}
    bool init() { currentTime = 0.0f; return true; }
    void destroy() {}
    void update(float frameTime)
    {
        currentTime += frameTime;

        r = (float)(0.5f + 0.5f * SDL_sin(currentTime));
        g = (float)(0.5f + 0.5f * SDL_sin(currentTime + M_PI * 2 / 3));
        b = (float)(0.5f + 0.5f * SDL_sin(currentTime + M_PI * 4 / 3));
        a = 1.0f;

        graphics->setBackColor(Color(r, g, b, a));
    }      // must override pure virtual from Game
    void ai(float frameTime) {}          // "
    void collisions(float frameTime) {}  // "
    void render() {}      // "
};

