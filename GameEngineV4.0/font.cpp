#include <SDL3\SDL_ttf.h>
#include "font.h"

static const int GRID_C = 16;          // number of columns in font image
static const int GRID_R = 14;          // number of rows in font image
static const int MAX_FONT_HEIGHT = 256;
static const int MIN_CHAR = 0x0020;            // minimum character code
static const int MAX_CHAR = 0x00FF;            // maximum character code
static const int TAB_SIZE = 8;

struct FontBase : public Font
{
public:
    FontBase() noexcept;
    FontBase(SDL_Renderer* pRenderer, SDL_Surface* pSurface, SDL_Texture* pTexture,
        rect_t* pMetrics, int* pAdvance, int cellWidth, int cellHeight,
        unsigned int tabSize, SDL_bool proportional) noexcept;
    ~FontBase();
    int DrawText(LP_SPRITE pSprite, const char* pString, int Count,
        rect_t* pRect, unsigned int Format, COLOR_ARGB Color);
    void SetTabSize(unsigned int Size);

private:
    SDL_Renderer* renderer2d;
    SDL_Surface* surface;
    SDL_Texture* texture;
    rect_t* metrics;
    int* advance;
    int cellW;
    int cellH;
    unsigned int tabSize;
    bool proportional;
};

bool Create(SDL_Renderer* pRenderer2d, int Height, bool Bold,
    bool Italic, long Quality, const char* pFaceName, Font** ppFont)
{
    if (pRenderer2d == NULL || Height > MAX_FONT_HEIGHT ||
        pFaceName == "" || ppFont == NULL)
    {
        return false;
    }

    // Initialize the TTF library
    if (TTF_Init() == SDL_FALSE)
    {
        GameError(gameErrorNS::FATAL_ERROR,
            "Couldn't initialize TTF: %s\n", SDL_GetError());

        return false;
    }

    int weight = TTF_STYLE_NORMAL;

    if (Bold)
    {
        weight |= TTF_STYLE_BOLD;
    }

    if (Italic)
    {
        weight |= TTF_STYLE_ITALIC;
    }

    // create TTF font
    TTF_Font* font = TTF_OpenFont(pFaceName, Height);

    if (font == NULL)
    {
        TTF_Quit();         // Free the TTF library

        return false;
    }

    TTF_SetFontStyle(font, weight);
    SDL_bool proportional = !TTF_FontFaceIsFixedWidth(font);

    rect_t* metrics = (rect_t*)SDL_malloc(GRID_R * GRID_C * sizeof(rect_t));
    int* advance = (int*)SDL_malloc(GRID_R * GRID_C * sizeof(int));

    if (metrics == NULL)
    {
        TTF_Quit();         // Free the TTF library

        return false;
    }

    if (advance == NULL)
    {
        SDL_free(metrics);
        TTF_Quit();         // Free the TTF library

        return false;
    }

    SDL_memset(metrics, 0, GRID_C * GRID_R * sizeof(rect_t));
    SDL_memset(advance, 0, GRID_C * GRID_R * sizeof(int   ));

    int cellW = 0;
    int cellH = TTF_FontHeight(font);

    for (int row = 0; row < GRID_R; row++)
    {
        for (int col = 0; col < GRID_C; col++)
        {
            const char ch = (row * GRID_C) + col + MIN_CHAR;

            if (ch >= MIN_CHAR && ch <= MAX_CHAR)
            {
                int index = row * GRID_C + col;
                if (TTF_GlyphMetrics(font, ch,
                    (int*)&metrics[index].min[0],
                    (int*)&metrics[index].max[0],
                    (int*)&metrics[index].min[1],
                    (int*)&metrics[index].max[1],
                    &advance[index]) == SDL_TRUE)
                {
                    const int w = (metrics[index].max[0] - metrics[index].min[0]);
                    if (cellW < w)
                    {
                        cellW = w;
                    }
                }
            }
        }
    }

    SDL_Surface* surface = SDL_CreateSurface(cellW * GRID_C,
        cellH * GRID_R, SDL_PIXELFORMAT_ARGB8888);

    SDL_Surface* glyph = NULL;
    const SDL_Color  fcolor = {
        255,
        255,
        255,
        255
    };
    const SDL_Color bcolor = {
        128,
        128,
        128,
        255
    };

    uint32_t surfaceRGBA = SDL_MapRGBA(
        SDL_GetPixelFormatDetails(surface->format),
        SDL_GetSurfacePalette(surface), bcolor.r, bcolor.g, bcolor.b, bcolor.a);
    SDL_FillSurfaceRect(surface, NULL, surfaceRGBA);

    SDL_SetSurfaceColorKey(surface, SDL_TRUE,
        SDL_MapRGBA(SDL_GetPixelFormatDetails(surface->format),
            SDL_GetSurfacePalette(surface),
            bcolor.r, bcolor.g, bcolor.b, bcolor.a));

    for (int row = 0; row < GRID_R; row++)
    {
        for (int col = 0; col < GRID_C; col++)
        {
            SDL_Rect rect = {};
            const char ch = (row * GRID_C) + col + MIN_CHAR;

            if (ch >= MIN_CHAR && ch <= MAX_CHAR)
            {
                switch (Quality)
                {
                case HIGH_QUALITY:
                {
                    glyph = TTF_RenderGlyph_LCD(font, ch, fcolor, bcolor);
                } break;
                case DEFAULT_QUALITY:
                default:
                {
                    glyph = TTF_RenderGlyph_Shaded(font, ch, fcolor, bcolor);
                }
                }

                if (glyph != NULL)
                {
                    rect.x = col * cellW;
                    rect.y = row * cellH;
                    rect.w = cellW;
                    rect.h = cellH;

                    SDL_BlitSurface(glyph, NULL, surface, &rect);
                    SDL_DestroySurface(glyph);
                    glyph = NULL;

                }
            }
        }
    }

    if (font != NULL)
    {
        TTF_CloseFont(font);
        font = NULL;
    }

    TTF_Quit();         // Free the TTF library

    SDL_Texture* texture = SDL_CreateTextureFromSurface(pRenderer2d, surface);

    if (texture == NULL)
    {
        if (surface != NULL)
        {
            SDL_DestroySurface(surface);
            surface = NULL;
        }

        SDL_free(metrics);
        SDL_free(advance);

        return false;
    }

    FontBase* pFont = new FontBase(pRenderer2d, surface, texture, metrics,
        advance, cellW, cellH, TAB_SIZE, proportional);
    *ppFont = pFont;

    return true;
}

FontBase::FontBase() noexcept
{
    renderer2d = NULL;
    surface = NULL;
    texture = NULL;
    metrics = NULL;
    advance = NULL;
    cellW = 0;
    cellH = 0;
    proportional = false;
    tabSize = TAB_SIZE;
}

FontBase::FontBase(SDL_Renderer* pRenderer2d, SDL_Surface* pSurface,
    SDL_Texture* pTexture, rect_t* pMetrics, int* pAdvance, int cellWidth,
    int cellHeight, unsigned int tabSize, SDL_bool proportional) noexcept
    : renderer2d(pRenderer2d), surface(pSurface), texture(pTexture),
    metrics(pMetrics), advance(pAdvance), cellW(cellWidth), cellH(cellHeight),
    tabSize(tabSize), proportional(proportional)
{
}

FontBase::~FontBase()
{
    if (texture != NULL)
    {
        SDL_DestroyTexture(texture);
        texture = NULL;
    }

    if (surface != NULL)
    {
        SDL_DestroySurface(surface);
        surface = NULL;
    }

    SDL_free(metrics);
    SDL_free(advance);
}

void FontBase::SetTabSize(unsigned int Size)
{
    if (Size == 0)
    {
        return;
    }

    tabSize = Size;
}

int FontBase::DrawText(LP_SPRITE pSprite, const char* pString, int Count,
    rect_t* pRect, unsigned int Format, COLOR_ARGB Color)
{
    if (pSprite == NULL || pString[0] == '\0' || Count == 0)
    {
        return 0;
    }

    std::string str2 = "";
    std::string str = std::string(pString, Count);
    rect_t sprRect = {};
    int offset = 0;
    int extent = 0;
    int lineNum = (str != "") ? 1 : 0;
    int rectW = pRect->max[0] - pRect->min[0];
    int rectH = pRect->max[1] - pRect->min[1];
    int lineW = 0;
    int lineH = cellH;
    int wordW = 0;
    int charW = cellW;
    int charH = cellH;
    int strW = 0;
    int strH = 0;
    float l = 0.0f;
    float t = 0.0f;
    float x = 0.0f;
    float y = 0.0f;
    unsigned char ch = 0, chN = 0;
    unsigned char spaceW = 0;

    // TODO: RTLREADING

    if (proportional)
    {
        chN = ' ' - MIN_CHAR;
        spaceW = advance[((chN >> 4) * GRID_C) + (chN % GRID_C)] + 1;
    }
    else            // fixed pitch
    {
        spaceW = cellW;
    }

    // newline count
    for (std::string::const_iterator iter = str.begin(); iter != str.end();
        iter++)
    {
        if (*iter == '\n') lineNum++;
    }

    if (lineNum > 1)
    {
        strW = rectW;
    }

    // format string
    while (offset < str.length())
    {
        // regular words
        while (extent < str.length() &&
            (str.at(extent) > MIN_CHAR && str.at(extent) <= MAX_CHAR))
        {
            extent++;
        }

        // measure width in pixels
        wordW = 0;
        std::string token = str.substr(offset, (extent - offset));
        for (unsigned int i = 0; i < token.length(); i++)
        {
            ch = token.at(i);
            if (ch > MIN_CHAR && ch <= MAX_CHAR)            // displayable character
            {
                if (proportional)
                {
                    chN = ch - MIN_CHAR;            // make min_char index 0
                    charW = advance[((chN >> 4) * GRID_C) + (chN % GRID_C)] + 1;
                }
                else            // fixed pitch
                {
                    charW = cellW;
                }

                wordW += charW;
            }
        }

        // wordbound
        if ((Format & WORDBOUNDS) == WORDBOUNDS)
        {
            if (lineW + wordW > rectW && rectW > 0)
            {
                if ((Format & BOTTOM) != BOTTOM &&
                    (Format & SINGLELINE) != SINGLELINE)
                {
                    strW = rectW;
                    if (str2.at(str2.length()-1) != '\n')
                    {
                        if (str2.at(str2.length()-1) == ' ')
                        {
                            str2.erase(str2.end()-1);
                        }
                        str2 += '\n';
                    }
                    lineW = 0;
                    lineH += charH;
                }

                if (wordW > rectW)          // word is bigger than the rectangle
                {
                    wordW = 0;
                    for (unsigned int i = 0; i < token.length(); i++)
                    {
                        ch = token.at(i);
                        if (ch > MIN_CHAR && ch <= MAX_CHAR)
                        {
                            if (proportional)
                            {
                                chN = ch - MIN_CHAR;
                                charW = advance[((chN >> 4) * GRID_C) + (chN % GRID_C)] + 1;
                            }
                            else            // fixed pitch
                            {
                                charW = cellW;
                            }

                            // rectangle bounds
                            if ((lineW + wordW + charW) > rectW && rectW > 0)
                            {
                                if ((Format & BOTTOM) != BOTTOM &&
                                    (Format & SINGLELINE) != SINGLELINE)
                                {
                                    str2 += '\n';
                                    str2 += ch;
                                    wordW = charW;
                                    lineW = 0;
                                    lineH += charH;
                                    continue;
                                }
                            }

                            wordW += charW;
                            str2 += ch;
                        }
                    }
                }
                else
                {
                    extent = offset;
                    continue;
                }
            }
            else
            {
                str2 += token;
            }
        }
        else            // everything else
        {
            if ((lineNum == 1 || (wordW > rectW && rectW > 0)) && (Format & CALCRECT) == CALCRECT)
            {
                if ((lineW + wordW) > rectW && rectW > 0)
                {
                    rectW = lineW + wordW;
                    strW = rectW;
                }
            }

            str2 += token;
        }

        lineW += wordW;
        offset = extent;

        // special characters
        if (extent < str.length())
        {
            while (extent < str.length() && str.at(extent) <= MIN_CHAR)
            {
                extent++;
            }
        }

        wordW = 0;
        token = str.substr(offset, (extent - offset));
        for (unsigned int i = 0; i < token.length(); i++)
        {
            ch = token.at(i);
            switch (ch)
            {
            case '\b':          // backspace
            {
                ch = str2.at(str2.length()-1);
                if (ch >= MIN_CHAR && ch <= MAX_CHAR)            // displayable character
                {
                    if (proportional)
                    {
                        chN = ch - MIN_CHAR;            // make min_char index 0
                        charW = advance[((chN >> 4) * GRID_C) + (chN % GRID_C)] + 1;
                    }
                    else            // fixed pitch
                    {
                        charW = cellW;
                    }
                }
                else
                {
                    charW = spaceW;
                }

                str2.erase(str2.end()-1);
                lineW -= charW;
            } break;
            case '\t':          // horizontal tab
            {
                if ((Format & EXPANDTABS) == EXPANDTABS)
                {
                    charW = spaceW;

                    int tabX = (lineW + wordW) / (charW * tabSize);
                    tabX = (tabX + 1) * charW * tabSize;
                    int tabW = tabX - (lineW + wordW);

                    while (tabW > 0)
                    {
                        if (tabW >= charW)
                        {
                            str2 += ' ';
                            lineW += charW;
                        }
                        else
                        {
                            // fractional part of character to align with tab stop
                            str2 += ' ';
                            charW = tabW;
                            lineW += tabW;
                        }
                        tabW -= charW;
                    }
                }
                else            // space
                {
                    str2 += ' ';

                    charW = spaceW;
                    lineW += charW;
                }
            } break;
            case '\n':          // linefeed
            {
                str2 += ch;
                if ((Format & BOTTOM) != BOTTOM &&
                    (Format & SINGLELINE) != SINGLELINE)
                {
                    lineW = 0;
                    lineH += charH;
                }
            } break;
            case '\v':          // vertical tab
            {
                if ((Format & EXPANDTABS) == EXPANDTABS)
                {
                    str2 += ch;
                    if ((Format & BOTTOM) != BOTTOM &&
                        (Format & SINGLELINE) != SINGLELINE)
                    {
                        lineH += charH;
                    }
                }
                else            // space
                {
                    str2 += ' ';

                    charW = spaceW;
                    lineW += charW;
                }
            } break;
            case '\r':          // carriage return
            {
                str2 += ch;
                if ((Format & BOTTOM) != BOTTOM &&
                    (Format & SINGLELINE) != SINGLELINE)
                {
                    lineW = 0;
                }
            } break;
            case ' ':           // space
            {
                charW = spaceW;
                if (lineW != 0)
                {
                    str2 += ch;
                    lineW += charW;
                }
            } break;
            }
        }

        if (strW < lineW)
        {
            strW = lineW;
        }
        if (strH < lineH)
        {
            strH = lineH;
        }
        offset = extent;
    }

    // calculate rectangle
    if ((Format & CALCRECT) == CALCRECT)
    {
        pRect->max[0] = pRect->min[0] + strW;
        pRect->max[1] = pRect->min[1] + strH;

        return strH;
    }

    // alignment and draw
    offset = 0;
    extent = 0;

    t = static_cast<float>(pRect->min[1]);

    if ((Format & VCENTER) == VCENTER)
    {
        t = (float)((pRect->min[1] + pRect->max[1]) - lineH) / 2.0f;
        t = ceilf(t);
    }
    else if ((Format & BOTTOM) == BOTTOM)
    {
        t = (float)(pRect->max[1] - lineH);
    }

    while (offset < str2.length())
    {
        while (extent < str2.length() && (str2.at(extent) != '\n' &&
            str2.at(extent) != '\r'))
        {
            extent++;
        }

        if (extent < str2.length())
        {
            while (extent < str2.length() && (str2.at(extent) == '\n' ||
                str2.at(extent) == '\r'))
            {
                extent++;
            }
        }

        std::string line = str2.substr(offset, (extent - offset));
        lineW = 0;
        lineH = cellH;

        // alignment
        for (unsigned int i = 0; i < line.length(); i++)
        {
            ch = line.at(i);
            if (ch > MIN_CHAR && ch <= MAX_CHAR)            // displayable character
            {
                if (proportional)
                {
                    chN = ch - MIN_CHAR;            // make min_char index 0
                    charW = advance[((chN >> 4) * GRID_C) + (chN % GRID_C)] + 1;
                }
                else            // fixed pitch
                {
                    charW = cellW;
                }
                lineW += charW;
            }
            else
            {
                switch (ch)
                {
                case '\n':          // linefeed
                {
                    if ((Format & BOTTOM) == BOTTOM &&
                        (Format & SINGLELINE) == SINGLELINE)
                    {
                        charW = spaceW;
                        lineW += charW;
                    }
                } break;
                case '\v':           // vertical tab
                {
                    if ((Format & EXPANDTABS) == EXPANDTABS && ((Format & BOTTOM) != BOTTOM &&
                        (Format & SINGLELINE) != SINGLELINE))
                    {
                        lineH += charH;
                    }
                    else            // space
                    {
                        charW = spaceW;
                        lineW += charW;
                    }
                } break;
                case '\r':          // carriage return
                {
                    if ((Format & BOTTOM) == BOTTOM &&
                        (Format & SINGLELINE) == SINGLELINE)
                    {
                        charW = spaceW;
                        lineW += charW;
                    }
                } break;
                case ' ':           // space
                {
                    charW = spaceW;
                    lineW += charW;
                } break;
                }
            }
        }

        l = static_cast<float>(pRect->min[0]);

        if ((Format & HCENTER) == HCENTER)
        {
            l = (float)((pRect->min[0] + pRect->max[0]) - lineW) / 2.0f;
            l = ceilf(l);
        }
        else if ((Format & RIGHT) == RIGHT)
        {
            l = (float)(pRect->max[0] - lineW);
            l = ceilf(l);
        }

        // draw
        for (unsigned int i = 0; i < line.length(); i++)
        {
            ch = line.at(i);
            if (ch > MIN_CHAR && ch <= MAX_CHAR)            // displayable character
            {
                chN = ch - MIN_CHAR;            // make min_char index 0
                sprRect.min[1] = (chN >> 4) * cellH;
                sprRect.max[1] = sprRect.min[1] + cellH;

                if (proportional)
                {
                    sprRect.min[0] = ((chN % GRID_C) * cellW) +
                        metrics[((chN >> 4) * GRID_C) + (chN % GRID_C)].min[0];
                    sprRect.max[0] = ((chN % GRID_C) * cellW) +
                        metrics[((chN >> 4) * GRID_C) + (chN % GRID_C)].max[0] + 1;
                    charW = advance[((chN >> 4) * GRID_C) + (chN % GRID_C)] + 1;
                }
                else            // fixed pitch
                {
                    charW = cellW;
                    sprRect.min[0] = (chN % GRID_C) * cellW;
                    sprRect.max[0] = sprRect.min[0] + cellW;
                }

                if (l + x >= pRect->min[0] && l + x < pRect->max[0] &&
                    t + y >= pRect->min[1] && t + y < pRect->max[1])
                {
                    pSprite->Draw(texture, &sprRect, NULL,
                        &Vector3(l + x, t + y, 1.0f), Color);
                }
                x += charW;
            }
            else
            {
                switch (ch)
                {
                case '\n':          // linefeed
                {
                    if ((Format & BOTTOM) != BOTTOM &&
                        (Format & SINGLELINE) != SINGLELINE)
                    {
                        x = 0;
                        y += charH;
                    }
                    else
                    {
                        x += spaceW;
                    }
                } break;
                case '\v':           // vertical tab
                {
                    if ((Format & EXPANDTABS) == EXPANDTABS && ((Format & BOTTOM) != BOTTOM &&
                        (Format & SINGLELINE) != SINGLELINE))
                    {
                        y += charH;
                    }
                    else
                    {
                        x += spaceW;
                    }
                } break;
                case '\r':          // carriage return
                {
                    if ((Format & BOTTOM) != BOTTOM &&
                        (Format & SINGLELINE) != SINGLELINE)
                    {
                        x = 0;
                    }
                    else
                    {
                        x += spaceW;
                    }
                } break;
                case ' ':           // space
                {
                    x += spaceW;
                } break;
                }
            }
        }
        offset = extent;
    }

    return lineH;
}
