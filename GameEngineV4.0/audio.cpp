#include "audio.h"

typedef uint16_t FACTCATEGORY;
typedef uint16_t FACTINDEX;

//=============================================================================
// default constructor
//=============================================================================
Audio::Audio()
{
    audioEngine = NULL;
    waveBank = NULL;
    soundBank = NULL;
    mapWaveBank = NULL;
    soundBankData = NULL;
}

//=============================================================================
// destructor
//=============================================================================
Audio::~Audio()
{
    // Shutdown FACT
    if (audioEngine)
    {
        FACTAudioEngine_ShutDown(audioEngine);          // shut down FACT engine and free resources
        FACTAudioEngine_Release(audioEngine);
        audioEngine = NULL;
    }

    // After FACTAudioEngine_ShutDown() returns, release memory mapped files
    SAFE_DELETE_ARRAY(soundBankData);
    SAFE_DELETE_ARRAY(mapWaveBank);
}

//=============================================================================
// initialize
//=============================================================================
bool Audio::initialize(const char* globalSettingsFile)
{
    SDL_IOStream* hFile = 0;
    uint32_t fileSize = 0;
    uint32_t result = 0;

    result = FACTCreateEngine(0, &audioEngine);

    if (result != 0 || audioEngine == NULL)
    {
        GameError(gameErrorNS::WARNING, SDL_GetError());
        return false;
    }

    // Load the global settings file and pass it into FACTInitialize
    void* pGlobalSettingsData = 0;
    bool bSuccess = false;

    hFile = SDL_IOFromFile(globalSettingsFile, "rb");

    if (hFile)
    {
        fileSize = (uint32_t)SDL_GetIOSize(hFile);

        if (fileSize != -1)
        {
            pGlobalSettingsData = new uint8_t[fileSize];

            if (pGlobalSettingsData)
            {
                if (SDL_ReadIO(hFile, pGlobalSettingsData, fileSize) != -1)
                {
                    bSuccess = true;
                }
            }
        }

        SDL_CloseIO(hFile);
    }

    if (!bSuccess)
    {
        if (pGlobalSettingsData)
        {
            safeDeleteArray(pGlobalSettingsData);
        }

        pGlobalSettingsData = 0;
        fileSize = 0;
    }

    // Initialize & create the FACT runtime 
    FACTRuntimeParameters factParams = { 0 };
    factParams.pGlobalSettingsBuffer = pGlobalSettingsData;
    factParams.globalSettingsBufferSize = fileSize;

    // delete the buffer when not needed
    factParams.globalSettingsFlags = FACT_FLAG_MANAGEDATA;
    factParams.lookAheadTime = FACT_ENGINE_LOOKAHEAD_DEFAULT;
    result = FACTAudioEngine_Initialize(audioEngine, &factParams);

    if (result != 0)
    {
        GameError(gameErrorNS::WARNING, "Couldn't intialise audio engine.\n");

        return false;
    }

    return true;
}

//=============================================================================
// perform periodic sound engine tasks
//=============================================================================
void Audio::run()
{
    if (audioEngine == NULL)
    {
        return;
    }

    FACTAudioEngine_DoWork(audioEngine);
}

//=============================================================================
// play sound specified by cue from sound bank
//=============================================================================
void Audio::playCue(const char cue[])
{
    if (soundBank == NULL)
    {
        return;
    }

    FACTINDEX cueI = FACTSoundBank_GetCueIndex(soundBank, cue);
    FACTSoundBank_Play(soundBank, cueI, 0, 0, NULL);
}

//=============================================================================
// stop a playing sound specified by cue from sound bank
//=============================================================================
void Audio::stopCue(const char cue[])
{
    if (soundBank == NULL)
    {
        return;
    }

    FACTINDEX cueI = FACTSoundBank_GetCueIndex(soundBank, cue);
    FACTSoundBank_Stop(soundBank, cueI, FACT_FLAG_STOP_IMMEDIATE);
}

//=============================================================================
// Pause sound specified by category.
//=============================================================================
void Audio::pauseCategory(const char category[])
{
    if (soundBank == NULL)
    {
        return;
    }

    FACTCATEGORY iCategory = FACTAudioEngine_GetCategory(audioEngine, category);
    FACTAudioEngine_Pause(audioEngine, iCategory, true);
}

//=============================================================================
// Resume playback of paused sound specified by category.
//=============================================================================
void Audio::resumeCategory(const char category[])
{
    if (soundBank == NULL)
    {
        return;
    }

    FACTCATEGORY iCategory = FACTAudioEngine_GetCategory(audioEngine, category);
    FACTAudioEngine_Pause(audioEngine, iCategory, false);
}

//=============================================================================
// WAVE_BANK must be location of .xwb file.
//=============================================================================
bool Audio::loadWaveBank(const char* waveBankFile)
{
    SDL_IOStream* hFile = NULL;
    uint32_t fileSize = 0;
    uint32_t result = 0;

    if (audioEngine == 0)
    {
        return false;
    }

    if (mapWaveBank)
    {
        SAFE_DELETE_ARRAY(mapWaveBank);
    }

    // Create an "in memory" wave bank file using memory mapped file IO
    result = 0;         // default to failure code, replaced on success
    hFile = SDL_IOFromFile(waveBankFile, "rb");

    if (hFile != 0)
    {
        fileSize = (uint32_t)SDL_GetIOSize(hFile);

        if (fileSize != -1)
        {
            mapWaveBank = new uint8_t[fileSize];

            if (mapWaveBank)
            {
                if (SDL_ReadIO(hFile, mapWaveBank, fileSize) != -1)
                {
                    result = FACTAudioEngine_CreateInMemoryWaveBank(audioEngine, mapWaveBank,
                        fileSize, 0, 0, &waveBank);
                }
            }
        }

        SDL_CloseIO(hFile);         // mapWaveBank maintains a handle on the file so close this unneeded handle
    }

    if (result != 0)
    {
        return false;
    }

    return true;
}

//=============================================================================
// SOUND_BANK must be location of .xsb file.
//=============================================================================
bool Audio::loadSoundBank(const char* soundBankFile)
{
    SDL_IOStream* hFile = NULL;
    uint32_t fileSize = 0;
    uint32_t result = 0;

    if (audioEngine == 0)
    {
        return false;
    }

    if (soundBankData)
    {
        SAFE_DELETE_ARRAY(soundBankData);
    }

    // Read and register the sound bank file.
    result = 0;         // default to failure code, replaced on success
    hFile = SDL_IOFromFile(soundBankFile, "rb");

    if (hFile != 0)
    {
        fileSize = (uint32_t)SDL_GetIOSize(hFile);

        if (fileSize != -1)
        {
            soundBankData = new uint8_t[fileSize];

            if (soundBankData)
            {
                if (SDL_ReadIO(hFile, soundBankData, fileSize) != -1)
                {
                    result = FACTAudioEngine_CreateSoundBank(audioEngine, soundBankData,
                        fileSize, 0, 0, &soundBank);
                }
            }
        }

        SDL_CloseIO(hFile);
    }

    if (result != 0)
    {
        return false;
    }

    return true;
}
