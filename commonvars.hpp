#ifndef COMMONVARS_H
#define COMMONVARS_H

#include <32blit.hpp>
#include "cmainmenu.hpp"
#include "assets.hpp"

using namespace blit;

typedef struct CBoardParts CBoardParts;
typedef struct CPeg CPeg;
typedef struct SPoint SPoint;
typedef struct CSelector CSelector;

// The diffrent difficultys
#define VeryEasy 0
#define Easy 1
#define Hard 2
#define VeryHard 3

// The diffrent gameStates possible in the game
#define GSQuit 0
#define GSGame 1 
#define GSTitleScreen 2
#define GSDifficultySelect 3 
#define GSCredits 4

#define GSInitDiff 50

#define GSGameInit (GSGame + GSInitDiff)
#define GSTitleScreenInit (GSTitleScreen + GSInitDiff)
#define GSDifficultySelectInit (GSDifficultySelect + GSInitDiff)
#define GSCreditsInit (GSCredits + GSInitDiff)

// window size
#define WINDOW_WIDTH 240
#define WINDOW_HEIGHT 240

//game defines
#define NrOfRows 9
#define NrOfCols 9
#define TileWidth 24
#define TileHeight 24
#define IDPeg 1
#define BoardXOffSet 10
#define BoardYOffSet 11
#define NrOfSounds 6

#define SND_SELECT 0
#define SND_GOOD 1
#define SND_WRONG 2
#define SND_START 3
#define SND_WINNER 4
#define SND_LOSER 5

const Font font(gamefont);


struct SaveData {
    int BestPegsLeft[4]; // array that holds the best amount of pegs left for each difficulty
};
typedef struct SaveData SaveData;

extern SaveData saveData;

extern Surface *IMGSelector,*IMGTitle,*IMGNewGame1,*IMGNewGame2,*IMGCredits1,*IMGCredits2,*IMGCredits,*IMGPeg,
    *IMGBackground,*IMGVeryEasy1,*IMGEasy1,*IMGHard1,*IMGVeryHard1,*IMGEasyInfo,*IMGVeryEasyInfo,*IMGHardInfo,*IMGVeryHardInfo;
extern int Difficulty;

extern CBoardParts *BoardParts; // boardparts instance that will hold all the boardparts
extern int GameState; // the game state
extern bool GlobalSoundEnabled; // used to see if the sound was opened succesfully
extern int Moves,Volume;
extern int BestPegsLeft[4]; // array that holds the best amount of pegs left for each difficulty

extern float scale;
extern int xoffset;
extern int yoffset;
extern bool debugMode;

//game
extern CSelector *GameSelector;
extern bool PrintFormShown;

//titlescreen
extern CMainMenu* MainMenu;

#endif