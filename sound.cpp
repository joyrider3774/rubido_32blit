#include <cstring>
#include <stdint.h>
#include <32blit.hpp>

#include "commonvars.hpp"
#include "sound.hpp"

#include <32blit.hpp>

blit::Timer timer_music;

uint8_t prev_music, music_on, sound_on, music_pause, music_selecting;
uint16_t track[255];
uint16_t music_note, music_tempo, tracklen;
uint8_t music_loop;

constexpr uint8_t sfxSustain = 100 * 15 / 18;
constexpr uint8_t musModifier = 45;
constexpr uint8_t musModifier2 = 25;

const uint16_t winner_chime[] ={
    523, 100 / musModifier, 659, 100/ musModifier, 783, 100/ musModifier, 1046, 300/ musModifier, 1318, 500/ musModifier
};

const uint16_t loser_chime[] ={
    392, 200/ musModifier, 369, 200/ musModifier, 329, 300/ musModifier, 293, 300/ musModifier, 277, 500/ musModifier
};

void playMusicTone(uint16_t tone, uint16_t sustain)
{
    if((tone == 0) || music_selecting)
        channels[1].off();
    else
    {
        channels[1].waveforms   = Waveform::SQUARE;
        channels[1].attack_ms   = 1;
        channels[1].decay_ms    = 60*sustain;
        channels[1].sustain     = 0;
        channels[1].release_ms  = 0;
        channels[1].volume      = 0xaaaa;
        channels[1].frequency   = tone;
        channels[1].trigger_attack();
    }
}

void playNote()
{   
    if(music_selecting)
        return;

    if(music_note < tracklen)
    {
        //Set the new delay to wait
        music_tempo = track[music_note+1];
        playMusicTone(track[music_note],music_tempo);
        //Skip to the next note
        music_note += 2;
               
        if (music_note > tracklen - 1)
        {
            if(music_loop)
            {
                music_note = 0;
            }
        }
    }
}

void pauseMusic()
{
    music_pause = 1;
    stopMusic();
}

void unpauseMusic()
{
    music_pause = 0;
}

void musicTimer(blit::Timer &t)
{
    if(music_selecting)
        return;
    if(music_pause)
        return;
    //Play some music
    if (music_tempo == 0)
    {
        if(music_on)
        {
            playNote();
        }
    }
    //Else wait for the next note to play
    else 
    {
        music_tempo--;
    }
}

void stopMusic()
{
    channels[1].off();  
}

void setMusicOn(uint8_t value)
{
    music_on = value;
    if(music_on)
    {
        if (prev_music != 0)
            SelectMusic(prev_music, 1);
    }
    else
    {
        channels[1].off();
    }
}

void setSoundOn(uint8_t value)
{
    sound_on = value;
}

uint8_t isMusicOn()
{
    return music_on;
}

uint8_t isSoundOn()
{
    return sound_on;
}

void initSound()
{
    sound_on = 0;
}

void SelectMusic(uint8_t musicFile, uint8_t force)
{
    if (((prev_music != musicFile) || force) && music_on)
    {
        music_selecting = 1;
        music_pause = 0;
        prev_music = musicFile;
        channels[1].off();
        memset(track, 0, sizeof(track));
        switch (musicFile) 
        {
            case musWinner:
                memcpy(track, winner_chime, sizeof(winner_chime));
                tracklen = sizeof(winner_chime) / sizeof(uint16_t);
                music_loop = false;
                break;
            case musLoser:
                memcpy(track, loser_chime, sizeof(loser_chime));
                tracklen = sizeof(loser_chime) / sizeof(uint16_t);
                music_loop = false;
                break;            
        }
        music_note = 0;
        music_tempo = 0;
        music_selecting = 0;
    }
}

void playSound(uint16_t tone)
{
    if(!sound_on)
        return;
    
    channels[0].waveforms   = Waveform::SQUARE;
    channels[0].attack_ms   = 16;
    channels[0].decay_ms    = sfxSustain;
    channels[0].sustain     = 0;
    channels[0].release_ms  = 32;
    channels[0].volume      = 0xffff;
    channels[0].frequency   = tone;
    channels[0].trigger_attack();
}

void initMusic()
{
    music_on = 0;
    prev_music = 0;
    timer_music.init(musicTimer, 16, -1);
    timer_music.start();
}

void playErrorSound()
{
    playSound(210);
}

void playMenuSelectSound()
{
    playSound(1250);
}

void playMenuBackSound()
{
   playSound(1000);
}

void playMenuAcknowlege()
{
    playSound(900);
}

void playGameAction()
{
    playSound(600);
}
