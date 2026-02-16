#pragma once



void InitAudio();
void UninitAudio();


int LoadAudio(const char* FileName);
void UnloadAudio(int Index);
void StopAudio(int Index);
void PlayAudio(int Index, bool Loop = false);

// aduio fade out
void AudioFade(int Index, float speed);
// aduio fade out to half
void AudioFadeHalf(int Index);
