#include "input.h"

//=============================================================================
// default constructor
//=============================================================================
Input::Input()
{
    // Keyboard
    for (size_t i = 0; i < inputNS::KEYS_ARRAY_LEN; i++)
        keysDown[i] = false;

    for (size_t i = 0; i < inputNS::KEYS_ARRAY_LEN; i++)
        keysPressed[i] = false;

    newLine = true;
    textIn.clear();
    charIn = 0;

    // Mouse
    mouseX = 0;
    mouseY = 0;
    mouseRawX = 0;
    mouseRawY = 0;
    mouseLButtonPressed = false;
    mouseMButtonPressed = false;
    mouseRButtonPressed = false;
    mouse4ButtonPressed = false;
    mouse5ButtonPressed = false;
    mouseLButton = false;
    mouseMButton = false;
    mouseRButton = false;
    mouse4Button = false;
    mouse5Button = false;

    // Controller
    SDL_memset(controllers, 0, sizeof(ControllerState) * MAX_CONTROLLERS);

    thumbstickDeadzone = GAMEPAD_THUMBSTICK_DEADZONE;
    triggerDeadzone = GAMEPAD_TRIGGER_DEADZONE;
}

//=============================================================================
// destructor
//=============================================================================
Input::~Input()
{
    // Mouse
    if (mouseCaptured)
    {
        SDL_CaptureMouse(SDL_FALSE);
    }

    // Controller
    for (int i = 0; i < MAX_CONTROLLERS; i++)
    {
        if (controllers[i].controller != 0)
        {
            SDL_CloseGamepad(controllers[i].controller);
        }
    }

    SDL_QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD);
}

//=============================================================================
// Initialize mouse and controller input
// Set capture=true to capture mouse
//=============================================================================
bool Input::initialize(SDL_Window* hwnd, bool capture)
{
    if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD) != SDL_TRUE)
    {
        throw(std::runtime_error(SDL_GetError()));
        return false;
    }

    // Mouse
    mouseCaptured = capture;

    if (mouseCaptured)
    {
        SDL_CaptureMouse(SDL_TRUE);
    }

    // Controller
    SDL_memset(controllers, 0, sizeof(ControllerState) * MAX_CONTROLLERS);

    SDL_AddGamepadMappingsFromFile("gamecontrollerdb.txt");

    checkControllers();

    return true;
}

//=============================================================================
// Set true in the keysDown and keysPessed array for this key
//=============================================================================
void Input::keyDown(unsigned int key)
{
    if (key < inputNS::KEYS_ARRAY_LEN)
    {
        keysDown[key] = true;
        keysPressed[key] = true;
    }
}

//=============================================================================
// Set false in the keysDown array for this key
//=============================================================================
void Input::keyUp(unsigned int key)
{
    if (key < inputNS::KEYS_ARRAY_LEN)
        keysDown[key] = false;
}

//=============================================================================
// Save the char just entered in textIn string
//=============================================================================
void Input::keyIn(unsigned int key)
{
    if (newLine)            // if start of new line
    {
        textIn.clear();
        newLine = false;
    }

    if (key == '\b')            // backspace
    {
        if (textIn.length() > 0)
        {
            textIn.erase(textIn.size() - 1);
        }
    }
    else
    {
        // don't process console key
        if (key == '`')
        {
            return;
        }

        if (textIn.length() + 1 < 256)
        {
            textIn.insert(textIn.length(), 1, (key & 0xFF));
        }

        charIn = (key & 0xFF);
    }

    if (key == '\r')            // return
    {
        newLine = true;
    }
}

//=============================================================================
// Returns true if the specified VIRTUAL KEY is down, otherwise false.
//=============================================================================
bool Input::isKeyDown(unsigned char vkey) const
{
    if (vkey < inputNS::KEYS_ARRAY_LEN)
        return keysDown[vkey];
    else
        return false;
}

//=============================================================================
// Return true if the specified VIRTUAL KEY has been pressed in the most recent
// frame.
//=============================================================================
bool Input::wasKeyPressed(unsigned char vkey) const
{
    if (vkey < inputNS::KEYS_ARRAY_LEN)
        return keysPressed[vkey];
    else
        return false;
}

//=============================================================================
// Return true if any key was pressed in the most recent frame.
//=============================================================================
bool Input::anyKeyPressed() const
{
    for (size_t i = 0; i < inputNS::KEYS_ARRAY_LEN; i++)
        if (keysPressed[i] == true)
            return true;
    return false;
}

//=============================================================================
// Clear the specified key press
//=============================================================================
void Input::clearKeyPress(unsigned char vkey)
{
    if (vkey < inputNS::KEYS_ARRAY_LEN)
        keysPressed[vkey] = false;
}

//=============================================================================
// Clear specified input buffers
//=============================================================================
void Input::clear(unsigned char what)
{
    if (what & inputNS::KEYS_DOWN)
    {
        for (size_t i = 0; i < inputNS::KEYS_ARRAY_LEN; i++)
            keysDown[i] = false;
    }

    if (what & inputNS::KEYS_PRESSED)
    {
        for (size_t i = 0; i < inputNS::KEYS_ARRAY_LEN; i++)
            keysPressed[i] = false;
    }

    if (what & inputNS::MOUSE_BUTTONS_PRESSED)
    {
        mouseLButtonPressed = false;
        mouseMButtonPressed = false;
        mouseRButtonPressed = false;
        mouse4ButtonPressed = false;
        mouse5ButtonPressed = false;
    }

    if (what & inputNS::MOUSE)
    {
        mouseRawX = 0;
        mouseRawY = 0;
        mouseX = 0;
        mouseY = 0;
    }

    if (what & inputNS::TEXT_IN)
    {
        clearTextIn();
    }

    if (what & inputNS::GAMEPAD)
    {
        for (int i = 0; i < MAX_CONTROLLERS; i++)
        {
            if (controllers[i].controller != 0)
            {
                controllers[i].vibrateMotorSpeedLeft = 0;
                controllers[i].vibrateMotorSpeedRight = 0;
                controllers[i].vibrateTimeLeft = 0.0f;
                controllers[i].vibrateTimeRight = 0.0f;
            }
        }
    }
}

//=============================================================================
// Clears key, mouse and text input data
//=============================================================================
void Input::clearAll()
{
    clear(inputNS::KEYS_MOUSE_TEXT_GAMEPAD);
}

//=============================================================================
// Clear text input buffer
//=============================================================================
void Input::clearTextIn()
{
    textIn.clear();
}

//=============================================================================
// Clear last character entered
//=============================================================================
void Input::clearCharIn()
{
    charIn = 0;
}

//=============================================================================
// Return text input as a string
//=============================================================================
std::string Input::getTextIn()
{
    return textIn;
}

//=============================================================================
// Set text input string
//=============================================================================
void Input::setTextIn(std::string str)
{
    textIn = str;
}

//=============================================================================
// Return last character entered
//=============================================================================
char Input::getCharIn()
{
    return charIn;
}


//-----------------------------------------------------------------------------
//
// MOUSE
//
//-----------------------------------------------------------------------------


//=============================================================================
// Reads mouse screen position into mouseX, mouseY
//=============================================================================
void Input::mouseIn(int x, int y)
{
    mouseX = x;
    mouseY = y;
}

//=============================================================================
// Reads raw mouse data into mouseRawX, mouseRawY
//=============================================================================
void Input::mouseRawIn(int x, int y)
{
    mouseRawX = x;
    mouseRawY = y;
}

//=============================================================================
// Reads mouse wheel movement expressed in multiples of WHEEL_DELTA, which
// is 120.
//=============================================================================
void Input::mouseWheelIn(int w)
{
    mouseWheel = w;
}

//=============================================================================
// Return true if the specified VIRTUAL BUTTON has been pressed in the most
// recent frame.
//=============================================================================
bool Input::wasMouseButtonPressed(unsigned char vbutton)
{
    bool result = false;

    switch (vbutton)
    {
    case MOUSE_L_BUTTON:
    {
        result = mouseLButtonPressed;
    } break;
    case MOUSE_M_BUTTON:
    {
        result = mouseMButtonPressed;
    } break;
    case MOUSE_R_BUTTON:
    {
        result = mouseRButtonPressed;
    } break;
    case MOUSE_4_BUTTON:
    {
        result = mouse4ButtonPressed;
    } break;
    case MOUSE_5_BUTTON:
    {
        result = mouse5ButtonPressed;
    } break;
    }

    return result;
}

//=============================================================================
// Save state of mouse button
//=============================================================================
void Input::setMouseLButton(bool b)
{
    mouseLButtonPressed = true;
    mouseLButton = b;
}

//=============================================================================
// Save state of mouse button
//=============================================================================
void Input::setMouseMButton(bool b)
{
    mouseMButtonPressed = true;
    mouseMButton = b;
}

//=============================================================================
// Save state of mouse button
//=============================================================================
void Input::setMouseRButton(bool b)
{
    mouseRButtonPressed = true;
    mouseRButton = b;
}

//=============================================================================
// Save state of mouse button
//=============================================================================
void Input::setMouse4Button(bool b)
{
    mouse4ButtonPressed = true;
    mouse4Button = b;
}

//=============================================================================
// Save state of mouse button
//=============================================================================
void Input::setMouse5Button(bool b)
{
    mouse5ButtonPressed = true;
    mouse5Button = b;
}

//=============================================================================
// Return mouse X position
//=============================================================================
int Input::getMouseX() const
{
    return mouseX;
}

//=============================================================================
// Return mouse Y position
//=============================================================================
int Input::getMouseY() const
{
    return mouseY;
}

//=============================================================================
// Return raw mouse X movement. Left is <0, Right is >0
//=============================================================================
int Input::getMouseRawX()
{
    int rawX = mouseRawX;
    mouseRawX = 0;

    return rawX;
}

//=============================================================================
// Return raw mouse Y movement. Up is <0, Down is >0
//=============================================================================
int Input::getMouseRawY()
{
    int rawY = mouseRawY;
    mouseRawY = 0;

    return rawY;
}

//=============================================================================
// Return state of left mouse button.
//=============================================================================
bool Input::getMouseLButton() const
{
    return mouseLButton;
}

//=============================================================================
// Return state of middle mouse button.
//=============================================================================
bool Input::getMouseMButton() const
{
    return mouseMButton;
}

//=============================================================================
// Return state of right mouse button.
//=============================================================================
bool Input::getMouseRButton() const
{
    return mouseRButton;
}

//=============================================================================
// Return state of X1 mouse button.
//=============================================================================
bool Input::getMouse4Button() const
{
    return mouse4Button;
}

//=============================================================================
// Return state of X2 mouse button.
//=============================================================================
bool Input::getMouse5Button() const
{
    return mouse5Button;
}


//-----------------------------------------------------------------------------
//
// CONTROLLER
//
//-----------------------------------------------------------------------------


//=============================================================================
// Check for connected controllers
//=============================================================================
void Input::checkControllers()
{
    int joystickCount = 0;
    SDL_JoystickID* joysticks = SDL_GetJoysticks(&joystickCount);

    for (int i = 0, n = 0; i < joystickCount; ++i)
    {
        if (SDL_IsGamepad(joysticks[i]) == SDL_TRUE && n < MAX_CONTROLLERS)
        {
            SDL_Gamepad* controller = SDL_OpenGamepad(joysticks[i]);

            if (controller != NULL)
            {
                SDL_PropertiesID properties = SDL_GetGamepadProperties(controller);
                SDL_bool hasRumble = SDL_GetBooleanProperty(properties,
                    SDL_PROP_GAMEPAD_CAP_RUMBLE_BOOLEAN, SDL_FALSE);
                controllers[n].controller = controller;
                controllers[n].hasRumble = hasRumble;
                controllers[n].connected = true;
                n++;
            }
        }
    }
}

//=============================================================================
// Read state of connected controllers
//=============================================================================
void Input::readControllers()
{
    SDL_UpdateGamepads();

    for (size_t i = 0; i < MAX_CONTROLLERS; i++)
    {
        if (controllers[i].connected == true)
        {
            // bitmask of the buttons
            controllers[i].gamepadButtons = 0;

            for (size_t button = 0; button < SDL_GAMEPAD_BUTTON_COUNT; button++)
            {
                controllers[i].gamepadButtons |=
                    SDL_GetGamepadButton(controllers[i].controller,
                        (SDL_GamepadButton)button) << button;
            }
        }
    }
}

//=============================================================================
// Set thumbstick deadzone
//=============================================================================
void Input::setThumbstickDeadzone(short dz)
{
    thumbstickDeadzone = abs(dz);
}

//=============================================================================
// Set trigger deadzone
//=============================================================================
void Input::setTriggerDeadzone(unsigned char dz)
{
    triggerDeadzone = dz;
}

//=============================================================================
// Get thumbstick deadzone
//=============================================================================
short Input::getThumbstickDeadzone()
{
    return thumbstickDeadzone;
}

//=============================================================================
// Get trigger deadzone
//=============================================================================
unsigned char Input::getTriggerDeadzone()
{
    return static_cast<unsigned char>(triggerDeadzone);
}

//=============================================================================
// Return state of specified game controller.
//=============================================================================
const ControllerState* Input::getControllerState(unsigned int n)
{
    if (n > MAX_CONTROLLERS - 1)
        n = MAX_CONTROLLERS - 1;
    return &controllers[n];
}

//=============================================================================
// Return connection state of specified game controller
//=============================================================================
bool Input::getGamepadConnected(unsigned int n)
{
    if (n > MAX_CONTROLLERS - 1)
        n = MAX_CONTROLLERS - 1;
    return controllers[n].connected;
}

//=============================================================================
// Return state of controller n buttons.
//=============================================================================
const unsigned short Input::getGamepadButtons(unsigned int n)
{
    if (n > MAX_CONTROLLERS - 1)
        n = MAX_CONTROLLERS - 1;
    return controllers[n].gamepadButtons;
}

//=============================================================================
// Return state of controller n D-pad Up
//=============================================================================
bool Input::getGamepadDPadUp(unsigned int n)
{
    if (n > MAX_CONTROLLERS - 1)
        n = MAX_CONTROLLERS - 1;
    return SDL_GetGamepadButton(controllers[n].controller,
        SDL_GAMEPAD_BUTTON_DPAD_UP);
}

//=============================================================================
// Return state of controller n D-pad Down.
//=============================================================================
bool Input::getGamepadDPadDown(unsigned int n)
{
    if (n > MAX_CONTROLLERS - 1)
        n = MAX_CONTROLLERS - 1;
    return SDL_GetGamepadButton(controllers[n].controller,
        SDL_GAMEPAD_BUTTON_DPAD_DOWN);
}

//=============================================================================
// Return state of controller n D-pad Left.
//=============================================================================
bool Input::getGamepadDPadLeft(unsigned int n)
{
    if (n > MAX_CONTROLLERS - 1)
        n = MAX_CONTROLLERS - 1;
    return SDL_GetGamepadButton(controllers[n].controller,
        SDL_GAMEPAD_BUTTON_DPAD_LEFT);
}

//=============================================================================
// Return state of controller n D-pad Right.
//=============================================================================
bool Input::getGamepadDPadRight(unsigned int n)
{
    if (n > MAX_CONTROLLERS - 1)
        n = MAX_CONTROLLERS - 1;
    return SDL_GetGamepadButton(controllers[n].controller,
        SDL_GAMEPAD_BUTTON_DPAD_RIGHT);
}

//=============================================================================
// Return state of controller n Start button.
//=============================================================================
bool Input::getGamepadStart(unsigned int n)
{
    if (n > MAX_CONTROLLERS - 1)
        n = MAX_CONTROLLERS - 1;
    return SDL_GetGamepadButton(controllers[n].controller,
        SDL_GAMEPAD_BUTTON_START);
}

//=============================================================================
// Return state of controller n Back button.
//=============================================================================
bool Input::getGamepadBack(unsigned int n)
{
    if (n > MAX_CONTROLLERS - 1)
        n = MAX_CONTROLLERS - 1;
    return SDL_GetGamepadButton(controllers[n].controller,
        SDL_GAMEPAD_BUTTON_BACK);
}

//=============================================================================
// Return state of controller n Left Thumb button.
//=============================================================================
bool Input::getGamepadLeftThumb(unsigned int n)
{
    if (n > MAX_CONTROLLERS - 1)
        n = MAX_CONTROLLERS - 1;
    return SDL_GetGamepadButton(controllers[n].controller,
        SDL_GAMEPAD_BUTTON_LEFT_STICK);
}

//=============================================================================
// Return state of controller n Right Thumb button.
//=============================================================================
bool Input::getGamepadRightThumb(unsigned int n)
{
    if (n > MAX_CONTROLLERS - 1)
        n = MAX_CONTROLLERS - 1;
    return SDL_GetGamepadButton(controllers[n].controller,
        SDL_GAMEPAD_BUTTON_RIGHT_STICK);
}

//=============================================================================
// Return state of controller n Left Shoulder button.
//=============================================================================
bool Input::getGamepadLeftShoulder(unsigned int n)
{
    if (n > MAX_CONTROLLERS - 1)
        n = MAX_CONTROLLERS - 1;
    return SDL_GetGamepadButton(controllers[n].controller,
        SDL_GAMEPAD_BUTTON_LEFT_SHOULDER);
}

//=============================================================================
// Return state of controller n Right Shoulder button.
//=============================================================================
bool Input::getGamepadRightShoulder(unsigned int n)
{
    if (n > MAX_CONTROLLERS - 1)
        n = MAX_CONTROLLERS - 1;
    return SDL_GetGamepadButton(controllers[n].controller,
        SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER);
}

//=============================================================================
// Return state of controller n A button.
//=============================================================================
bool Input::getGamepadA(unsigned int n)
{
    if (n > MAX_CONTROLLERS - 1)
        n = MAX_CONTROLLERS - 1;
    return SDL_GetGamepadButton(controllers[n].controller,
        SDL_GAMEPAD_BUTTON_SOUTH);
}

//=============================================================================
// Return state of controller n B button.
//=============================================================================
bool Input::getGamepadB(unsigned int n)
{
    if (n > MAX_CONTROLLERS - 1)
        n = MAX_CONTROLLERS - 1;
    return SDL_GetGamepadButton(controllers[n].controller,
        SDL_GAMEPAD_BUTTON_EAST);
}

//=============================================================================
// Return state of controller n X button.
//=============================================================================
bool Input::getGamepadX(unsigned int n)
{
    if (n > MAX_CONTROLLERS - 1)
        n = MAX_CONTROLLERS - 1;
    return SDL_GetGamepadButton(controllers[n].controller,
        SDL_GAMEPAD_BUTTON_WEST);
}

//=============================================================================
// Return state of controller n Y button.
//=============================================================================
bool Input::getGamepadY(unsigned int n)
{
    if (n > MAX_CONTROLLERS - 1)
        n = MAX_CONTROLLERS - 1;
    return SDL_GetGamepadButton(controllers[n].controller,
        SDL_GAMEPAD_BUTTON_NORTH);
}

//=============================================================================
// Return value of controller n Left Trigger.
// Deadzone is not applied.
//=============================================================================
int Input::getGamepadLeftTriggerUndead(unsigned int n)
{
    if (n > MAX_CONTROLLERS - 1)
        n = MAX_CONTROLLERS - 1;
    return SDL_GetGamepadAxis(controllers[n].controller,
        SDL_GAMEPAD_AXIS_LEFT_TRIGGER);
}

//=============================================================================
// Return value of controller n Left Trigger (0 through 32767).
//=============================================================================
int Input::getGamepadLeftTrigger(unsigned int n)
{
    int value = getGamepadLeftTriggerUndead(n);
    if (value > triggerDeadzone)
        // adjust magnitude relative to the end of the dead zone
        value = (value - triggerDeadzone) * 32767 /
        (32767 - triggerDeadzone);
    else
        value = 0;
    return value;
}

//=============================================================================
// Return value of controller n Right Trigger.
//=============================================================================
int Input::getGamepadRightTriggerUndead(unsigned int n)
{
    if (n > MAX_CONTROLLERS - 1)
        n = MAX_CONTROLLERS - 1;
    return SDL_GetGamepadAxis(controllers[n].controller,
        SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);
}

//=============================================================================
// Return value of controller n Right Trigger (0 through 32767).
// Trigger movement less than triggerDeadzone returns 0.
// Return value is scaled to 0 through 32767
//=============================================================================
int Input::getGamepadRightTrigger(unsigned int n)
{
    int value = getGamepadRightTriggerUndead(n);
    if (value > triggerDeadzone)
        // adjust magnitude relative to the end of the dead zone
        value = (value - triggerDeadzone) * 32767 /
        (32767 - triggerDeadzone);
    else
        value = 0;
    return value;
}

//=============================================================================
// Return value of controller n Left Thumbstick X.
//=============================================================================
int Input::getGamepadThumbLXUndead(unsigned int n)
{
    if (n > MAX_CONTROLLERS - 1)
        n = MAX_CONTROLLERS - 1;
    return SDL_GetGamepadAxis(controllers[n].controller,
        SDL_GAMEPAD_AXIS_LEFTX);
}

//=============================================================================
// Return value of controller n Left Thumbstick X (-32767 through 32767).
//=============================================================================
int Input::getGamepadThumbLX(unsigned int n)
{
    int x = getGamepadThumbLXUndead(n);
    if (x > thumbstickDeadzone)
        // adjust x relative to the deadzone and scale to 0 through 32767
        x = (x - thumbstickDeadzone) * 32767 /
        (32767 - thumbstickDeadzone);
    else if (x < -thumbstickDeadzone)
        // adjust y relative to the deadzone and scale to 0 through -32767
        x = (x + thumbstickDeadzone) * 32767 /
        (32767 - thumbstickDeadzone);
    else
        x = 0;
    return x;
}

//=============================================================================
// Return value of controller n Left Thumbstick Y.
//=============================================================================
int Input::getGamepadThumbLYUndead(unsigned int n)
{
    if (n > MAX_CONTROLLERS - 1)
        n = MAX_CONTROLLERS - 1;
    return SDL_GetGamepadAxis(controllers[n].controller,
        SDL_GAMEPAD_AXIS_LEFTY);
}

//=============================================================================
// Return value of controller n Left Thumbstick Y (-32768 through 32767).
//=============================================================================
int Input::getGamepadThumbLY(unsigned int n)
{
    int y = getGamepadThumbLYUndead(n);
    if (y > thumbstickDeadzone)
        // adjust magnitude relative to the end of the dead zone
        y = (y - thumbstickDeadzone) * 32767 /
        (32767 - thumbstickDeadzone);
    else if (y < -thumbstickDeadzone)
        // adjust magnitude relative to the end of the dead zone
        y = (y + thumbstickDeadzone) * 32767 /
        (32767 - thumbstickDeadzone);
    else
        y = 0;
    return y;
}

//=============================================================================
// Return value of controller n Right Thumbstick X.
//=============================================================================
int Input::getGamepadThumbRXUndead(unsigned int n)
{
    if (n > MAX_CONTROLLERS - 1)
        n = MAX_CONTROLLERS - 1;
    return SDL_GetGamepadAxis(controllers[n].controller,
        SDL_GAMEPAD_AXIS_RIGHTX);
}

//=============================================================================
// Return value of controller n Right Thumbstick X (-32768 through 32767).
//=============================================================================
int Input::getGamepadThumbRX(unsigned int n)
{
    int x = getGamepadThumbRXUndead(n);
    if (x > thumbstickDeadzone)
        // adjust magnitude relative to the end of the dead zone
        x = (x - thumbstickDeadzone) * 32767 /
        (32767 - thumbstickDeadzone);
    else if (x < -thumbstickDeadzone)
        // adjust magnitude relative to the end of the dead zone
        x = (x + thumbstickDeadzone) * 32767 /
        (32767 - thumbstickDeadzone);
    else
        x = 0;
    return x;
}

//=============================================================================
// Return value of controller n Right Thumbstick Y.
//=============================================================================
int Input::getGamepadThumbRYUndead(unsigned int n)
{
    if (n > MAX_CONTROLLERS - 1)
        n = MAX_CONTROLLERS - 1;
    return SDL_GetGamepadAxis(controllers[n].controller,
        SDL_GAMEPAD_AXIS_RIGHTY);
}

//=============================================================================
// Return value of controller n Right Thumbstick Y (-32768 through 32767).
//=============================================================================
int Input::getGamepadThumbRY(unsigned int n)
{
    int y = getGamepadThumbRYUndead(n);
    if (y > thumbstickDeadzone)
        // adjust magnitude relative to the end of the dead zone
        y = (y - thumbstickDeadzone) * 32767 /
        (32767 - thumbstickDeadzone);
    else if (y < -thumbstickDeadzone)
        // adjust magnitude relative to the end of the dead zone
        y = (y + thumbstickDeadzone) * 32767 /
        (32767 - thumbstickDeadzone);
    else
        y = 0;
    return y;
}

//=============================================================================
// Vibrate controller n left motor.
//=============================================================================
void Input::gamePadVibrateLeft(unsigned int n, unsigned short speed, float sec)
{
    if (n > MAX_CONTROLLERS - 1)
        n = MAX_CONTROLLERS - 1;
    controllers[n].vibrateMotorSpeedLeft = speed;
    controllers[n].vibrateTimeLeft = sec;
}

//=============================================================================
// Vibrate controller n right motor.
//=============================================================================
void Input::gamePadVibrateRight(unsigned int n, unsigned short speed, float sec)
{
    if (n > MAX_CONTROLLERS - 1)
        n = MAX_CONTROLLERS - 1;
    controllers[n].vibrateMotorSpeedRight = speed;
    controllers[n].vibrateTimeRight = sec;
}

//=============================================================================
// Vibrate connected controllers
//=============================================================================
void Input::vibrateControllers(float frameTime)
{
    for (int i = 0; i < MAX_CONTROLLERS; i++)
    {

        if (controllers[i].connected && controllers[i].hasRumble)
        {
            controllers[i].vibrateTimeLeft -= frameTime;

            if (controllers[i].vibrateTimeLeft < 0)
            {
                controllers[i].vibrateTimeLeft = 0;
                controllers[i].vibrateMotorSpeedLeft = 0;
            }

            controllers[i].vibrateTimeRight -= frameTime;

            if (controllers[i].vibrateTimeRight < 0)
            {
                controllers[i].vibrateTimeRight = 0;
                controllers[i].vibrateMotorSpeedRight = 0;
            }

            // FIXME: SDL doesn't allow independant motor rumble
            SDL_RumbleGamepad(controllers[i].controller,
                controllers[i].vibrateMotorSpeedLeft,
                controllers[i].vibrateMotorSpeedRight,
                (uint32_t)(controllers[i].vibrateTimeRight * 1000));
        }
    }
}
