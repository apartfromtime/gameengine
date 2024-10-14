#pragma once
#include <fact.h>
#include "constants.h"
#include "gameError.h"

//-----------------------------------------------------------------------------
//
// AUDIO
//
//-----------------------------------------------------------------------------

class Audio
{
    // Audio properties
private:
    // FACT
    FACTAudioEngine* audioEngine;           // pointer to FACT sound engine
    FACTWaveBank* waveBank;         // pointer to FACT wave bank
    FACTSoundBank* soundBank;           // pointer to FACT sound bank
    void* mapWaveBank;
    void* soundBankData;

public:
    // Constructor
    Audio();

    // Destructor
    ~Audio();

    // member functions

    // Initialize Audio
    // XGS_FILE must be location of .xgs file.
    bool initialize(const char* globalSettingsFile);

    // Perform periodic sound engine tasks.
    void run();

    // Play sound specified by cue from sound bank.
    // If cue does not exist no error occurs, there is simply no sound played.
    void playCue(const char cue[]);

    // Stop a playing sound specified by cue from sound bank.
    // If cue does not exist no error occurs.
    void stopCue(const char cue[]);

    // Pause sound specified by category from sound bank.
    // If category does not exist no error occurs.
    void pauseCategory(const char category[]);

    // Resume playback of paused sound specified by category from sound bank.
    // If category does not exist no error occurs.
    void resumeCategory(const char category[]);

    // WAVE_BANK must be location of .xwb file.
    bool loadWaveBank(const char* waveBankFile);

    // SOUND_BANK must be location of .xsb file.
    bool loadSoundBank(const char* soundBankFile);
};

