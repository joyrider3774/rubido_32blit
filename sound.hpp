#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>
#include <32blit.hpp>

constexpr uint8_t musGame = 1;
constexpr uint8_t musWinner = 2;
constexpr uint8_t musLoser = 3;

void initSound();
void SelectMusic(uint8_t musicFile, uint8_t force = 0);
void initMusic();
void musicTimer(blit::Timer &t);
void pauseMusic();
void stopMusic();
void unpauseMusic();
void playGameAction();
void playMenuAcknowlege();
void playMenuBackSound();
void playMenuSelectSound();
void playErrorSound();
void setMusicOn(uint8_t value);
void setSoundOn(uint8_t value);
uint8_t isMusicOn();
uint8_t isSoundOn();


#endif