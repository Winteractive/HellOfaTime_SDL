#pragma once
#include "FMOD/fmod_common.h"

namespace Memory{
  struct Arena;
}

enum class SFX_ID{
  FALLBACK,
  JUMP,
  
  COUNT
};

enum class SONG_ID{
  NONE,
  THEME
};

struct SoundDataEntry{
  SFX_ID id;
  const char* path;
};

struct AudioSystem{
  bool initialized;
  void* fmod_memory;
  FMOD_SYSTEM* sound_system;
  static const int CHANNEL_COUNT = 32;
  FMOD_CHANNEL* channels[CHANNEL_COUNT];
  FMOD_SOUND* soundEffects[(int)SFX_ID::COUNT];
  SONG_ID song_id;
  FMOD_SOUND* song;
  FMOD_CHANNEL* song_channel;
};

// global promise
extern AudioSystem* g_audioSystem;


void PlaySong(SONG_ID id);
void PlaySFX(SFX_ID id, float volume = 1);
void InitializeAudioSystem(AudioSystem* audio, Memory::Arena* arena_main);
void UpdateAudio(AudioSystem* audio);

namespace AssetManagement{
  void LoadAllSFX(AudioSystem* audioSystem);
}
