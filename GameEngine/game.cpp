#include "game.h"

// The primary class should inherit from Game class

//=============================================================================
// Constructor
//=============================================================================
Game::Game()
{
    // System
    hwnd = 0;
    graphics = 0;
    input = 0;
    audio = 0;
    console = 0;
    messageDialog = 0;
    inputDialog = 0;
    // Time
    fps = 100;
    // View
    viewport3d = Viewport();
    wrldMatrix = Matrix4();
    viewMatrix = Matrix4();
    projMatrix = Matrix4();
    frustum[0] = -1.0f;
    frustum[1] =  1.0f;
    frustum[2] =  1.0f;
    frustum[3] = -1.0f;
    frustum[4] =  0.0f;
    frustum[5] =  1.0f;
    // Font
    fontOther = 0;
    // Commands
    SDL_memset(buffer, 0, sizeof(char) * gameNS::BUF_SIZE);
    fpsOn = false;
    paused = false;
    initialized = false;
}

//=============================================================================
// Destructor
//=============================================================================
Game::~Game()
{

}

//=============================================================================
// Release all game items
//=============================================================================
void Game::shutdown()
{
    deleteAll();            // free all reserved memory
}

//=============================================================================
// Window message handler
//=============================================================================
void Game::messageHandler(const SDL_Window* hwnd, const SDL_Event* msg)
{
    if (initialized == true)
    {
        switch (msg->type)
        {
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:          // exit this program
        {
            SDL_Event event = {};

            event.quit.timestamp = SDL_GetTicksNS();
            event.quit.type = SDL_EVENT_QUIT;

            SDL_PushEvent(&event);

            return;
        }
        case SDL_EVENT_KEY_DOWN:            // key down
        {
            input->keyDown(msg->key.scancode);
            return;
        }
        case SDL_EVENT_KEY_UP:          // key up
        {
            input->keyUp(msg->key.scancode);
            return;
        }
        case SDL_EVENT_TEXT_INPUT:          // character entered
        {
            if (console->getVisible())
            {
                console->keyIn(msg->text.text[0]);
            }
            else if (inputDialog->getVisible())
            {
                inputDialog->keyIn(msg->text.text[0]);
            }
            else
            {
                input->keyIn(msg->text.text[0]);
            }

            return;
        }
        case SDL_EVENT_MOUSE_MOTION:            // mouse moved
        {
            int xrel = static_cast<int>(msg->motion.xrel);
            int yrel = static_cast<int>(msg->motion.yrel);
            int x = static_cast<int>(msg->motion.x);
            int y = static_cast<int>(msg->motion.y);
            input->mouseRawIn(xrel, yrel);
            input->mouseIn(x, y);
            return;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        {
            switch (msg->button.button)
            {
            case SDL_BUTTON_LEFT:
            {
                input->setMouseLButton(true);
            } break;
            case SDL_BUTTON_MIDDLE:
            {
                input->setMouseMButton(true);
            } break;
            case SDL_BUTTON_RIGHT:
            {
                input->setMouseRButton(true);
            } break;
            case SDL_BUTTON_X1:
            {
                input->setMouse4Button(true);
            } break;
            case SDL_BUTTON_X2:
            {
                input->setMouse5Button(true);
            } break;
            }

            int x = static_cast<int>(msg->button.x);
            int y = static_cast<int>(msg->button.y);
            input->mouseIn(x, y);

            return;
        }
        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            switch (msg->button.button)
            {
            case SDL_BUTTON_LEFT:
            {
                input->setMouseLButton(false);
            } break;
            case SDL_BUTTON_MIDDLE:
            {
                input->setMouseMButton(false);
            } break;
            case SDL_BUTTON_RIGHT:
            {
                input->setMouseRButton(false);
            } break;
            case SDL_BUTTON_X1:
            {
                input->setMouse4Button(false);
            } break;
            case SDL_BUTTON_X2:
            {
                input->setMouse5Button(false);
            } break;
            }

            int x = static_cast<int>(msg->button.x);
            int y = static_cast<int>(msg->button.y);
            input->mouseIn(x, y);

            return;
        }
        case SDL_EVENT_MOUSE_WHEEL:
        {
            int wheely = static_cast<int>(msg->wheel.y);
            input->mouseWheelIn(wheely);
        } break;
        case SDL_EVENT_GAMEPAD_ADDED:           // check for controller insert
        case SDL_EVENT_GAMEPAD_REMOVED:
        {
            input->checkControllers();
            return;
        }
        }
    }
}

//=============================================================================
// Initializes the game
//=============================================================================
bool Game::initialize(SDL_Window* phwnd)
{
    hwnd = phwnd;

    // initialise graphics
    graphics = new Graphics();

    if (graphics->initialize(hwnd, 0, 0, 0, VSYNC) == false)
    {
        throw(std::runtime_error("Failed to initialize graphics system."));
    }

    // initialise input
    input = new Input();            // initialize keyboard input immediately

    // initialize input, do not capture mouse
    if (input->initialize(hwnd, false) == false)
    {
        throw(std::runtime_error("Failed to initialize input system."));
    }

    // initialise audio
    audio = new Audio();

    if (audio->initialize(XGS_FILE) == false)
    {
        throw(std::runtime_error("Failed to initialize audio system."));
    }

    // initialize SDL font
    if (font.initialize(graphics, gameNS::POINT_SIZE, false, false,
        gameNS::FONT) == false)
    {
        throw(std::runtime_error("Failed to initialize game font."));
    }

    font.setFontColor(gameNS::FONT_COLOR);

    // initialize console
    console = new Console();

    console->initialize(graphics, input);
    console->print("---Console---");

    // initialize messageDialog
    messageDialog = new MessageDialog();
    messageDialog->initialize(graphics, input);

    // initialize inputDialog
    inputDialog = new InputDialog();
    inputDialog->initialize(graphics, input);

    viewport3d = graphics->get3DViewport();

    if (init() == false)            // call init() in derived object
    {
        return false;
    }

    initialized = true;

    return true;
}

//=============================================================================
// Render game items
//=============================================================================
void Game::renderGame()
{
    // start rendering
    if (graphics->beginScene())
    {
        render();           // call render() in derived object

        graphics->spriteBegin(SPRITE_ALPHABLEND);

        if (fpsOn == true)          // fps display requested
        {
            // convert fps to string
            _snprintf_s(buffer, gameNS::BUF_SIZE, "fps %d ", (int)fps);
            font.print(buffer, (viewport3d.w - viewport3d.x) - 100,
                (viewport3d.h - viewport3d.y) - 28);
        }

        graphics->spriteEnd();

        messageDialog->draw();          // drawn on top
        inputDialog->draw();            // drawn on top
        console->draw();            // drawn on top

        // stop rendering
        graphics->endScene();
    }

    // display the back buffer on the screen
    graphics->showBackbuffer();
}

//=============================================================================
// Toggle window or fullscreen mode
//=============================================================================
void Game::setDisplayMode(graphicsNS::DISPLAY_MODE mode)
{
    graphics->changeDisplayMode(mode);
    viewport3d = graphics->get3DViewport();
}

//=============================================================================
// Exit the game
//=============================================================================
void Game::exitGame()
{
    ExitMainWindow();
}

//=============================================================================
// Return pointer to Graphics.
//=============================================================================
Console* Game::getConsole()
{
    return console;
}

//=============================================================================
// Return pointer to Graphics.
//=============================================================================
Graphics* Game::getGraphics()
{
    return graphics;
}

//=============================================================================
// Return pointer to Input.
//=============================================================================
Input* Game::getInput()
{
    return input;
}

//=============================================================================
// Return pointer to Audio.
//=============================================================================
Audio* Game::getAudio()
{
    return audio;
}

//=============================================================================
// Call repeatedly by the main message loop in main
//=============================================================================
void Game::run(float frameTime)
{
    if (graphics == NULL) {
        return;
    }

    // game timer
    if (frameTime > 0.0) {
        fps = (fps * 0.99f) + (0.01f / frameTime);          // average fps
    }

    // update(), ai(), and collisions() are pure virtual functions.
    // These functions must be provided in the class that inherits from Game.
    if (!paused) {

        update(frameTime);                   // update all game items
        ai(frameTime);                       // artificial intelligence
        collisions(frameTime);               // handle collisions
        input->vibrateControllers(frameTime);           // handle controller vibration
    }

    renderGame();           // draw all game items

    // toggle pause
    if (input->wasKeyPressed(PAUSE_KEY)) {
        paused = !paused;
    }

    console->update(frameTime);         // update console

    consoleCommand();           // process user entered console command

    input->readControllers();           // read state of controllers

    messageDialog->update();
    inputDialog->update();

    audio->run();           // perform periodic sound engine tasks

    // if Alt+Enter toggle fullscreen/window
    if ((input->isKeyDown(LALT_KEY) || input->isKeyDown(RALT_KEY))
        && input->wasKeyPressed(ENTER_KEY)) {
        setDisplayMode(graphicsNS::DISPLAYMODE_TOGGLE);         // toggle fullscreen/window
    }

    // if Alt+F4 exit game
    if ((input->isKeyDown(LALT_KEY) || input->isKeyDown(RALT_KEY))
        && input->wasKeyPressed(F4_KEY)) {
        exitGame();
    }

    // Clear input
    // Call this after all key checks are done
    input->clear(CLEAR_KEYS_PRESSED | CLEAR_MOUSE_BUTTONS_PRESSED);
}

//=============================================================================
// Process console commands
//=============================================================================
void Game::consoleCommand()
{
    std::string command = console->getCommand();            // get command from console
    std::vector<std::string> argv;
    uint32_t argc = 0;
    size_t offset = 0;
    size_t extent = 0;

    if (command == "")
    {
        return;
    }

    while (offset < command.length())
    {
        // regular words
        while (extent < command.length() &&
            command.at(extent) != '\b' && command.at(extent) != '\t' &&
            command.at(extent) != '\n' && command.at(extent) != '\v' &&
            command.at(extent) != '\r' && command.at(extent) != ' ')
        {
            extent++;
        }

        std::string token = command.substr(offset, (extent - offset));

        argv.push_back(token);
        argc++;

        if (extent < command.length())
        {
            // special characters
            while (extent < command.length() &&
                command.at(extent) == '\b' || command.at(extent) == '\t' ||
                command.at(extent) == '\n' || command.at(extent) == '\v' ||
                command.at(extent) == '\r' || command.at(extent) == ' ')
            {
                extent++;
            }
        }

        offset = extent;
    }

    if (argc == 0)         // if no command
    {
        return;
    }

    if (argv[0] == "help")         // if "help" command
    {
        console->print("Console Commands:");
        console->print("fps - toggle display of frames per second");

        return;
    }

    if (argc == 1)
    {
        if (argv[0] == "fps")
        {
            fpsOn = !fpsOn;
        }
    }
    else
    {
        if (argv[1] == "0" || argv[1] == "1")
        {
            if (argv[0] == "fps")
            {
                fpsOn = static_cast<bool>(atoi(argv[1].c_str()));
            }
            else
            {
                console->print("unknown command");
            }
        }
        else
        {
            std::string name = argv[0].c_str();
            console->print(name + " - " + "invalid value");
        }
    }
}

//=============================================================================
// Delete all reserved memory
//=============================================================================
void Game::deleteAll()
{
    destroy();          // call destroy() in derived object

    font.deleteAll();
    fontBig.deleteAll();
    fontScore.deleteAll();
    SAFE_DELETE(fontOther);
    SAFE_DELETE(console);
    SAFE_DELETE(messageDialog);
    SAFE_DELETE(inputDialog);
    SAFE_DELETE(audio);
    SAFE_DELETE(input);
    SAFE_DELETE(graphics);

    initialized = false;
}
