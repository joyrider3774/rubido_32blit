#include <32blit.hpp>
#include "commonvars.hpp"
#include "cboardparts.hpp"
#include "cselector.hpp"
#include "cmainmenu.hpp"

using namespace blit;

Surface *IMGSelector,*IMGTitle,*IMGNewGame1,*IMGNewGame2,*IMGCredits1,*IMGCredits2,*IMGCredits,*IMGPeg,
    *IMGBackground,*IMGVeryEasy1,*IMGEasy1,*IMGHard1,*IMGVeryHard1,*IMGEasyInfo,*IMGVeryEasyInfo,*IMGHardInfo,*IMGVeryHardInfo;
int Difficulty = VeryEasy;

CBoardParts *BoardParts; // boardparts instance that will hold all the boardparts
int GameState = GSTitleScreenInit; // the game state
bool GlobalSoundEnabled = true; // used to see if the sound was opened succesfully
int Moves=0,Volume = 128;
float scale = 1.0f;
int xoffset = 0;
int yoffset = 0;
bool debugMode = false;

SaveData saveData;

//game
CSelector *GameSelector;
bool PrintFormShown = false;


//titlescreen
CMainMenu* MainMenu;