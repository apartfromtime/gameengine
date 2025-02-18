#pragma once
#include <SDL3\sdl.h>
#include <string>
#include "constants.h"
#include "gameError.h"

//-----------------------------------------------------------------------------
//
// INPUT
//
//-----------------------------------------------------------------------------

const int KEYS_ARRAY_LEN = 512;         // size of key arrays

// what values for clear(), bit flag
const unsigned char CLEAR_KEYS_DOWN = 0x01;
const unsigned char CLEAR_KEYS_PRESSED = 0x02;
const unsigned char CLEAR_MOUSE = 0x04;
const unsigned char CLEAR_MOUSE_BUTTONS_PRESSED = 0x08;
const unsigned char CLEAR_TEXT_IN = 0x10;
const unsigned char CLEAR_GAMEPAD = 0x20;
const unsigned char CLEAR_KEYS_MOUSE_TEXT_GAMEPAD = CLEAR_KEYS_DOWN + CLEAR_KEYS_PRESSED +
    CLEAR_MOUSE + CLEAR_MOUSE_BUTTONS_PRESSED + CLEAR_TEXT_IN + CLEAR_GAMEPAD;

const unsigned int GAMEPAD_THUMBSTICK_DEADZONE = (unsigned int)(0.20f * 0X7FFF);            // default to 20% of range as deadzone
const unsigned int GAMEPAD_TRIGGER_DEADZONE = (unsigned int)(0.08f * 0X7FFF);               // trigger range 0-32767
const unsigned int MAX_CONTROLLERS = 4;         // Maximum number of controllers supported by XInput

// Bit corresponding to gamepad button in gamepadButtons
const unsigned int GAMEPAD_A                = 0x0001;
const unsigned int GAMEPAD_B                = 0x0002;
const unsigned int GAMEPAD_X                = 0x0004;
const unsigned int GAMEPAD_Y                = 0x0008;
const unsigned int GAMEPAD_BACK_BUTTON      = 0x0010;
const unsigned int GAMEPAD_GUIDE_BUTTON     = 0x0020;
const unsigned int GAMEPAD_START_BUTTON     = 0x0040;
const unsigned int GAMEPAD_LEFT_THUMB       = 0x0080;
const unsigned int GAMEPAD_RIGHT_THUMB      = 0x0100;
const unsigned int GAMEPAD_LEFT_SHOULDER    = 0x0200;
const unsigned int GAMEPAD_RIGHT_SHOULDER   = 0x0400;
const unsigned int GAMEPAD_DPAD_UP          = 0x0800;
const unsigned int GAMEPAD_DPAD_DOWN        = 0x1000;
const unsigned int GAMEPAD_DPAD_LEFT        = 0x2000;
const unsigned int GAMEPAD_DPAD_RIGHT       = 0x4000;

const unsigned int MOUSE_L_BUTTON = 0x0001;
const unsigned int MOUSE_M_BUTTON = 0x0002;
const unsigned int MOUSE_R_BUTTON = 0x0004;
const unsigned int MOUSE_4_BUTTON = 0x0008;
const unsigned int MOUSE_5_BUTTON = 0x0010;

struct ControllerState
{
    SDL_Gamepad*        controller;
    unsigned int        gamepadButtons;
    unsigned short      vibrateMotorSpeedLeft;
    unsigned short      vibrateMotorSpeedRight;
    float               vibrateTimeLeft;            // milliseconds
    float               vibrateTimeRight;           // milliseconds
    bool                hasRumble;
    bool                connected;
};

class Input
{
    // Input properties
private:
    // Keyboard
    bool keysDown[KEYS_ARRAY_LEN];
    bool keysPressed[KEYS_ARRAY_LEN];
    std::string textIn;
    char charIn;
    bool newLine;
    // Mouse
    int mouseX, mouseY;         // mouse screen coordinates
    int mouseRawX, mouseRawY;           // high-definition mouse data
    int mouseWheel;            // mouse wheel movement
    bool mouseCaptured;         // true if mouse captured
    bool mouseLButtonPressed;           // true if mouse left button down
    bool mouseMButtonPressed;           // true if mouse middle button down
    bool mouseRButtonPressed;           // true if mouse right button down
    bool mouse4ButtonPressed;           // true if mouse 4th button down
    bool mouse5ButtonPressed;           // true if mouse 5th button down
    bool mouseLButton;          // true if mouse left button down
    bool mouseMButton;          // true if mouse middle button down
    bool mouseRButton;          // true if mouse right button down
    bool mouse4Button;          // true if mouse 4th button down
    bool mouse5Button;          // true if mouse 5th button down
    // Controller
    ControllerState controllers[MAX_CONTROLLERS];           // state of controllers
    short thumbstickDeadzone;
    short triggerDeadzone;

public:
    // Constructor
    Input();

    // Destructor
    ~Input();

    // Initialize mouse and controller input.
    bool initialize(SDL_Window* hwnd, bool capture);

    // Save key down state
    void keyDown(unsigned int key);

    // Save key up state
    void keyUp(unsigned int key);

    // Save the char just entered in textIn string
    void keyIn(unsigned int key);

    // Returns true if the specified VIRTUAL KEY is down, otherwise false.
    bool isKeyDown(unsigned char vkey) const;

    // Return true if the specified VIRTUAL KEY has been pressed in the most recent frame.
    // Key presses are erased at the end of each frame.
    bool wasKeyPressed(unsigned char vkey) const;

    // Return true if any key was pressed in the most recent frame.
    // Key presses are erased at the end of each frame.
    bool anyKeyPressed() const;

    // Clear the specified key press
    void clearKeyPress(unsigned char vkey);

    // Clear specified input buffers where what is any combination of
    // KEYS_DOWN, KEYS_PRESSED, MOUSE, TEXT_IN or KEYS_MOUSE_TEXT.
    // Use OR '|' operator to combine parmeters.
    void clear(unsigned char what);

    // Clears key, mouse and text input data
    void clearAll();

    // Clear text input buffer
    void clearTextIn();

    // Clear last character entered
    void clearCharIn();

    // Return text input as a string
    std::string getTextIn();

    // Set text input string
    void setTextIn(std::string str);

    // Return last character entered
    char getCharIn() const;

    // Reads mouse screen position into mouseX, mouseY
    void mouseIn(int x, int y);

    // Reads raw mouse data into mouseRawX, mouseRawY
    // This routine is compatible with a high-definition mouse
    void mouseRawIn(int x, int y);

    // Reads mouse wheel movement.
    void mouseWheelIn(int w);

    // Return true if the specified VIRTUAL BUTTON has been pressed in the most recent frame.
    bool wasMouseButtonPressed(unsigned char vbutton) const;

    // Save state of mouse button
    void setMouseLButton(bool b);

    // Save state of mouse button
    void setMouseMButton(bool b);

    // Save state of mouse button
    void setMouseRButton(bool b);

    // Save state of mouse button
    void setMouse4Button(bool b);
    void setMouse5Button(bool b);

    // Return mouse X position
    int  getMouseX() const;

    // Return mouse Y position
    int  getMouseY() const;

    // Return raw mouse X movement. Left is <0, Right is >0
    // Compatible with high-definition mouse.
    int  getMouseRawX();

    // Return raw mouse Y movement. Up is <0, Down is >0
    // Compatible with high-definition mouse.
    int  getMouseRawY();

    // Return state of left mouse button.
    bool getMouseLButton() const;

    // Return state of middle mouse button.
    bool getMouseMButton() const;

    // Return state of right mouse button.
    bool getMouseRButton() const;

    // Return state of X1 mouse button.
    bool getMouse4Button() const;

    // Return state of X2 mouse button.
    bool getMouse5Button() const;

    // Update connection status of game controllers.
    void checkControllers();

    // Save input from connected game controllers.
    void readControllers();

    // Set thumbstick deadzone
    void setThumbstickDeadzone(short dz);

    // Set trigger deadzone
    void setTriggerDeadzone(unsigned char dz);

    // Get thumbstick deadzone
    short getThumbstickDeadzone() const;

    // Get trigger deadzone
    unsigned char getTriggerDeadzone() const;

    // Return state of specified game controller.
    const ControllerState* getControllerState(unsigned int n);

    // Return connection state of specified game controller
    bool getGamepadConnected(unsigned int n) const;

    // Return state of controller n buttons.
    const unsigned short getGamepadButtons(unsigned int n) const;

    // Return state of controller n D-pad Up
    bool getGamepadDPadUp(unsigned int n) const;

    // Return state of controller n D-pad Down.
    bool getGamepadDPadDown(unsigned int n) const;

    // Return state of controller n D-pad Left.
    bool getGamepadDPadLeft(unsigned int n) const;

    // Return state of controller n D-pad Right.
    bool getGamepadDPadRight(unsigned int n) const;

    // Return state of controller n Start button.
    bool getGamepadStart(unsigned int n) const;

    // Return state of controller n Back button.
    bool getGamepadBack(unsigned int n) const;

    // Return state of controller n Left Thumb button.
    bool getGamepadLeftThumb(unsigned int n) const;

    // Return state of controller n Right Thumb button.
    bool getGamepadRightThumb(unsigned int n) const;

    // Return state of controller n Left Shoulder button.
    bool getGamepadLeftShoulder(unsigned int n) const;

    // Return state of controller n Right Shoulder button.
    bool getGamepadRightShoulder(unsigned int n) const;

    // Return state of controller n A button.
    bool getGamepadA(unsigned int n) const;

    // Return state of controller n B button.
    bool getGamepadB(unsigned int n) const;

    // Return state of controller n X button.
    bool getGamepadX(unsigned int n) const;

    // Return state of controller n Y button.
    bool getGamepadY(unsigned int n) const;

    // Return value of controller n Left Trigger.
    // Deadzone is not applied.
    // Deadzone is not applied.
    int getGamepadLeftTriggerUndead(unsigned int n) const;

    // Return value of controller n Left Trigger (0 through 32767).
    // Trigger movement less than triggerDeadzone returns 0.
    // Return value is scaled to 0 through 32767
    int getGamepadLeftTrigger(unsigned int n) const;

    // Return value of controller n Right Trigger.
    int getGamepadRightTriggerUndead(unsigned int n) const;


    // Return value of controller n Right Trigger (0 through 32767).
    // Trigger movement less than triggerDeadzone returns 0.
    // Return value is scaled to 0 through 32767
    int getGamepadRightTrigger(unsigned int n) const;

    // Return value of controller n Left Thumbstick X.
    int getGamepadThumbLXUndead(unsigned int n) const;

    // Return value of controller n Left Thumbstick X (-32767 through 32767).
    // Stick movement less than thumbstickDeadzone returns 0.
    // Return value is scaled to -32768 through 32767
    int getGamepadThumbLX(unsigned int n) const;

    // Return value of controller n Left Thumbstick Y.
    int getGamepadThumbLYUndead(unsigned int n) const;

    // Return value of controller n Left Thumbstick Y (-32768 through 32767).
    // Stick movement less than thumbstickDeadzone returns 0.
    // Return value is scaled to -32768 through 32767
    int getGamepadThumbLY(unsigned int n) const;

    // Return value of controller n Right Thumbstick X.
    int getGamepadThumbRXUndead(unsigned int n) const;

    // Return value of controller n Right Thumbstick X (-32768 through 32767).
    // Stick movement less than thumbstickDeadzone returns 0.
    // Return value is scaled to -32768 through 32767
    int getGamepadThumbRX(unsigned int n) const;

    // Return value of controller n Right Thumbstick Y.
    int getGamepadThumbRYUndead(unsigned int n) const;

    // Return value of controller n Right Thumbstick Y (-32768 through 32767).
    // Stick movement less than thumbstickDeadzone returns 0.
    // Return value is scaled to -32768 through 32767
    int getGamepadThumbRY(unsigned int n) const;

    // Vibrate controller n left motor.
    // Left is low frequency vibration.
    // speed 0=off, 65536=100 percent
    // sec is time to vibrate in seconds
    void gamePadVibrateLeft(unsigned int n, unsigned short speed, float sec);

    // Vibrate controller n right motor.
    // Right is high frequency vibration.
    // speed 0=off, 65536=100 percent
    // sec is time to vibrate in seconds
    void gamePadVibrateRight(unsigned int n, unsigned short speed, float sec);

    // Vibrates the connected controllers for the desired time.
    void vibrateControllers(float frameTime);
};
