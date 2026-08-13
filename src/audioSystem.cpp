#include "audioSystem.h"
#include "FMOD/fmod.h"
#include "FMOD/fmod_common.h"
#include "arena.h"
#include "common.h"
#include <cassert>

// global
AudioSystem* g_audioSystem;

static const SoundDataEntry all_sound_data[] = {
  {SFX_ID::FALLBACK, "assets/audio/sfx/fallback.wav"},
  {SFX_ID::JUMP, "assets/audio/sfx/fallback.wav"},
};

void InitializeAudioSystem(AudioSystem* audio, Memory::Arena* arena_main){
  assert(audio->initialized == false);

  size_t memory_size = AUDIO_MEMORY_ALLOWANCE;
  audio->fmod_memory = Memory::CreateSubArena(arena_main, memory_size);
  FMOD_RESULT memory_init_ok = FMOD_Memory_Initialize(audio->fmod_memory, memory_size, nullptr, nullptr, nullptr, FMOD_MEMORY_ALL);
  assert(memory_init_ok == FMOD_OK);

  FMOD_RESULT system_creation_ok = FMOD_System_Create(&audio->sound_system, FMOD_VERSION);
  assert(system_creation_ok == FMOD_OK);

  FMOD_RESULT system_init_ok = FMOD_System_Init(audio->sound_system, AudioSystem::CHANNEL_COUNT, FMOD_INIT_NORMAL, nullptr);
  assert(system_init_ok == FMOD_OK);

  g_audioSystem = audio;

  audio->initialized = true;
}

const int NOT_FOUND = -1;
int GetAvailableChannelIndex(AudioSystem* audio){
  for (int i = 0; i < AudioSystem::CHANNEL_COUNT; i++) {
    FMOD_CHANNEL* channel = audio->channels[i];
    if(channel == nullptr){ // never used before
      return i;
    }
    FMOD_BOOL is_playing = false;
    FMOD_Channel_IsPlaying(channel, &is_playing);
    if(is_playing == false){
      return i;
    }
  }

  return NOT_FOUND;
}

void PlaySFX(SFX_ID id, float volume){
  assert(id != SFX_ID::COUNT);
  FMOD_SOUND* sfx = g_audioSystem->soundEffects[(int)id];
  if(sfx == nullptr){
    assert(id != SFX_ID::FALLBACK);
    PlaySFX(SFX_ID::FALLBACK);
  }
  int channel_index = GetAvailableChannelIndex(g_audioSystem);
  if(channel_index == NOT_FOUND){
    return;
  }
  FMOD_CHANNEL** channel_slot = &g_audioSystem->channels[channel_index];

  FMOD_System_PlaySound(g_audioSystem->sound_system, sfx, nullptr, false, channel_slot);
  FMOD_Channel_SetVolume(*channel_slot, volume);
}

void PlaySong(SONG_ID id){
  g_audioSystem->song_id = id;
  if(g_audioSystem->song != nullptr){
    FMOD_Channel_Stop(g_audioSystem->song_channel);
    FMOD_Sound_Release(g_audioSystem->song);
  }
  FMOD_SYSTEM* system = g_audioSystem->sound_system;
  const char* song_name;
  switch (id) {
  case SONG_ID::THEME:
    song_name = "assets/audio/music/hellofatime.mp3";
    break;
  case SONG_ID::NONE:
    break;
  }
  FMOD_System_CreateStream(system, song_name, FMOD_LOOP_NORMAL, nullptr, &g_audioSystem->song);
  int INFINITE = -1;
  FMOD_Sound_SetLoopCount(g_audioSystem->song, INFINITE);
  FMOD_System_PlaySound(system, g_audioSystem->song, nullptr, false, &g_audioSystem->song_channel);
}

void Update(AudioSystem* audio){
  if(g_audioSystem == nullptr || g_audioSystem != audio){
    g_audioSystem = audio;
  }
  assert(audio->initialized);
  FMOD_System_Update(audio->sound_system);
}


namespace AssetManagement{
  void LoadAllSFX(AudioSystem* audioSystem){
    for (const SoundDataEntry& sound_data : all_sound_data) {
      FMOD_RESULT sound_created_ok = FMOD_System_CreateSound(audioSystem->sound_system, sound_data.path, FMOD_DEFAULT, nullptr, &audioSystem->soundEffects[(int)sound_data.id]);
      assert(sound_created_ok == FMOD_OK);
    }
  }
}
