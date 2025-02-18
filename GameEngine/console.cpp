#include "console.h"

//=============================================================================
// Constructor
//=============================================================================
Console::Console()
{
    initialized = false;            // set true when successfully initialized
    graphics = NULL;
    input = NULL;
    visible = false;            // not visible
    fontColor = consoleNS::FONT_COLOR;
    backColor = consoleNS::BACK_COLOR;

    x = consoleNS::X;           // starting console position
    y = consoleNS::Y;
    w = consoleNS::W;
    h = consoleNS::H;
    b = consoleNS::BORDER;
    m = consoleNS::MARGIN;

    textRect.max[1] = (int)((y + h) - m);
    textRect.min[0] = (int)(x + m);
    textRect.max[0] = (int)((x + w) - m);
    textRect.min[1] = (int)(y + m);

    SDL_memset(&vtx, 0, 4 * sizeof(vector4_t));

    textIn = "";
    rows = 0;
    scrollAmount = 0;
}

//=============================================================================
// Destructor
//=============================================================================
Console::~Console()
{
    font.deleteAll();
}

//=============================================================================
// Initialize the console
//=============================================================================
bool Console::initialize(Graphics* pGraphics, Input* pInput)
{
    graphics = pGraphics;
    input = pInput;

    // top left
    vtx[0].x = x;
    vtx[0].y = y;

    // top right
    vtx[1].x = x + w;
    vtx[1].y = y;

    // bottom right
    vtx[2].x = x + w;
    vtx[2].y = y + h;

    // bottom left
    vtx[3].x = x;
    vtx[3].y = y + h;

    // initialize SDL font
    if (font.initialize(graphics, consoleNS::FONT_HEIGHT, false, false,
        consoleNS::FONT) == false)
    {
        return false;
    }

    font.setFontColor(fontColor);

    // sets textRect bottom to height of 1 row
    rowH = font.print("|", textRect, ALIGNMENT::CALCRECT);
    rowH += 2;         // height of 1 row (+2 is row spacing)

    if (rowH <= 0)         // this should never be true
    {
        rowH = 20;
    }

    // number of rows that will fit on console
    rows = (int)((h - (2 * m)) / (float)rowH);
    rows -= 2;          // room for input prompt at bottom

    if (rows <= 0)          // this should never be true
    {
        rows = 5;
    }

    initialized = true;

    return true;
}

//=============================================================================
// draw console
//=============================================================================
void Console::draw()
{
    if (!visible || graphics == NULL || !initialized)
    {
        return;
    }

    graphics->drawQuad(vtx[0], vtx[1], vtx[2], vtx[3], backColor);            // draw backdrop

    if (text.size() == 0)
    {
        return;
    }

    graphics->spriteBegin();

    // display text on console
    textRect.min[0] = 0;
    textRect.min[1] = 0;

    // set text display rect for one row
    textRect.min[0] = (long)(x + m);
    textRect.max[0] = (long)(x + (w - m));
    // -2*rowHeight is room for input prompt
    textRect.max[1] = (long)(y + ((h - (2 * m)) - (2 * rowH)));
    textRect.min[1] = (long)(textRect.max[1] - rowH);

    // for all rows (max text.size()) from bottom to top
    for (int r = scrollAmount; r < rows + scrollAmount && r < (int)(text.size()); r++)
    {
        // set text display rect top for this row
        textRect.min[1] = (long)(textRect.max[1] - rowH);
        // display one row of text
        font.print(text[r], textRect, ALIGNMENT::LEFT);
        // adjust text display rect bottom for next row
        textRect.max[1] -= rowH;
    }

    // display command prompt and current command string
    // set text display rect for prompt
    textRect.max[1] = (long)(y + (h - m));
    textRect.min[1] = (long)(textRect.max[1] - rowH);
    std::string prompt = ">";           // build prompt string
    prompt += textIn;
    font.print(prompt, textRect, ALIGNMENT::LEFT);         // display prompt and command

    graphics->spriteEnd();
}

//=============================================================================
// update
//=============================================================================
void Console::update(float frameTime)
{
    if (!initialized)
    {
        return;
    }

    // check for console key
    if (input->wasKeyPressed(CONSOLE_KEY))
    {
        input->clearCharIn();       // clear last char
        showHide();
        return;
    }

    // check for Esc key
    if (input->wasKeyPressed(ESC_KEY))
    {
        textIn.clear();
    }

    // check for scroll
    if (input->wasKeyPressed(UPARROW_KEY))
    {
        scrollAmount++;
    }
    else if (input->wasKeyPressed(DNARROW_KEY))
    {
        scrollAmount--;
    }
    else if (input->wasKeyPressed(PGUP_KEY))
    {
        scrollAmount += rows;
    }
    else if (input->wasKeyPressed(PGDN_KEY))
    {
        scrollAmount -= rows;
    }

    if (scrollAmount < 0)
    {
        scrollAmount = 0;
    }

    if (scrollAmount > consoleNS::MAX_LINES - 1)
    {
        scrollAmount = consoleNS::MAX_LINES - 1;
    }

    if (scrollAmount > (int)(text.size()) - 1)
    {
        scrollAmount = (int)(text.size()) - 1;
    }

    if (input->wasKeyPressed(BACKSPACE_KEY) == true)            // backspace
    {
        if (textIn.size() > 0)
        {
            textIn.erase(textIn.size() - 1);
        }
    }

    if (input->wasKeyPressed(ENTER_KEY) == true)           // if 'Enter' key not pressed
    {
        // do not pass keys through to game
        input->clear(CLEAR_KEYS_DOWN | CLEAR_KEYS_PRESSED
            | CLEAR_MOUSE);

        inputStr = textIn;
        textIn.clear();
    }
}

//=============================================================================
// show/hide console
//=============================================================================
void Console::showHide()
{
    if (!initialized)
    {
        return;
    }

    visible = !visible;
    textIn.clear();
}

//=============================================================================
// Return visible.
//=============================================================================
bool Console::getVisible() const
{
    return visible;
}

//=============================================================================
// Set visible = true;
//=============================================================================
void Console::show()
{
    visible = true;
}

//=============================================================================
// Set visible = false;
//=============================================================================
void Console::hide()
{
    visible = false;
}

//=============================================================================
// print variable format string
//=============================================================================
void Console::print(const char* fmt, ...)
{
    if (!initialized)
    {
        return;
    }

    va_list args;
    va_start(args, fmt);
    char str[256];

    vsnprintf(str, 256, fmt, args);
    va_end(args);

    text.push_front(str);           // add str to deque of text

    if (text.size() > consoleNS::MAX_LINES)
    {
        text.pop_back();            // delete oldest line
    }
}

//=============================================================================
// Add text to console
// Only the first line of text in str will be displayed.
//=============================================================================
void Console::print(const std::string& str)
{
    if (!initialized)
    {
        return;
    }

    text.push_front(str);           // add str to deque of text

    if (text.size() > consoleNS::MAX_LINES)
    {
        text.pop_back();            // delete oldest line
    }
}

//=============================================================================
// Save the char just entered in textIn string
//=============================================================================
void Console::keyIn(unsigned int key)
{
    if (input->wasKeyPressed(BACKSPACE_KEY) == true)            // backspace
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
    }
}

//=============================================================================
// Return console command
// Handles console single key commands.
// Returns all other commands to game.
//=============================================================================
std::string Console::getCommand()
{
    // if console not initialized or not visible
    if (!initialized || !visible)
    {
        return "";
    }

    std::string str = inputStr;

    inputStr = "";

    return str;
}

//=============================================================================
// Return Console Input text
//=============================================================================
std::string Console::getInput()
{
    return textIn;
}

//=============================================================================
// Clear Input text
//=============================================================================
void Console::clearInput()
{
    textIn = "";
}
