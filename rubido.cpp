#if defined(TARGET_32BLIT_HW) || defined(PICO_BUILD)
#include <malloc.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <32blit.hpp>
#include "assets.hpp"
#include "cboardparts.hpp"
#include "cmainmenu.hpp"
#include "cpeg.hpp"
#include "cselector.hpp"
#include "commonvars.hpp"
#include "sound.hpp"

using namespace blit;

uint32_t startus = 0;

// Load the settings, if there isn't a settings file, set some initial values
void LoadSettings()
{
	saveData.BestPegsLeft[VeryEasy] = 0;
	saveData.BestPegsLeft[Easy] = 0;
	saveData.BestPegsLeft[Hard] = 0;
	saveData.BestPegsLeft[VeryHard] = 0;
	read_save(saveData);
}

// Save the settings
void SaveSettings()
{
	write_save(saveData);
}

void printDebugCpuRamFpsLoad(uint32_t start_frame, uint32_t end_frame)
{
    if(debugMode)
    {
 #if defined(TARGET_32BLIT_HW) || defined(PICO_BUILD)

        // memory stats
#ifdef TARGET_32BLIT_HW
        extern char _sbss, _end, __ltdc_start;

        auto static_used = &_end - &_sbss;
        auto heap_total = &__ltdc_start - &_end;
#else // pico
        extern char __bss_start__, end, __StackLimit;

        auto static_used = &end - &__bss_start__;
        auto heap_total = &__StackLimit - &end;
#endif

        auto heap_used = mallinfo().uordblks;

        auto total_ram = static_used + heap_total;

        Point pos(0, 0);
        int w = screen.bounds.w;
        int h = 10;

        screen.pen = {128, 128, 128};
        int static_px = static_used * w / total_ram;
        screen.rectangle({pos.x, pos.y, static_px, h});

        screen.pen = {255, 255, 255};
        int heap_px = heap_used * w / total_ram;
        screen.rectangle({pos.x + static_px, pos.y, heap_px, h});

        screen.pen = {64, 64, 64};
        screen.rectangle({pos.x + static_px + heap_px, pos.y, w - (static_px + heap_px), h});

        screen.pen = {0, 0, 0};
        screen.rectangle({pos.x, pos.y + h, w, h});

        screen.pen = {255, 255, 255};
        char buf[100];
        snprintf(buf, sizeof(buf), "Mem: %i + %i / %i", static_used, heap_used, total_ram);
        screen.text(buf, minimal_font, {pos.x, pos.y + h, w, h}, true, TextAlign::center_center);

#endif
        uint32_t us = end_frame - start_frame;
        if (us == 0)
            us = 1;   
        long int fps = 1000000.0 / us;
        char buf2[100];
        snprintf(buf2, sizeof(buf2), "FPS: %ld", fps);
        screen.pen = Pen(255,255,255);
        screen.rectangle(Rect(1, screen.bounds.h - 10,  12 * 6 + 2, 10));
        screen.pen = Pen(0,0,0);
        screen.text(buf2, minimal_font, {1, screen.bounds.h - 9}, true, TextAlign::top_left);
    }    
}


// procedure that calculates how many moves are possible in the current board state
// we can simply do this by checking all parts and see if they can move to all directions
// the canmoveto method in CPegs is does all the checking
int MovesLeft()
{
    CPeg *BoardPart ;
    int TmpMoves=0,Y,X;
    for (Y=0;Y<NrOfRows;Y++)
        for (X=0;X<NrOfCols;X++)
        {
            BoardPart = CBoardParts_GetPart(BoardParts, X,Y);
            // if there is a boardpart on that X,Y Coordinate
            // check all direction if we can move to that if so increases the movesleft
            if (BoardPart)
            {
                if (CPeg_CanMoveTo(BoardPart,X+2,Y,false))
                    TmpMoves++;
                if (CPeg_CanMoveTo(BoardPart,X-2,Y,false))
                    TmpMoves++;
                if (CPeg_CanMoveTo(BoardPart,X,Y-2,false))
                    TmpMoves++;
                if (CPeg_CanMoveTo(BoardPart,X,Y+2,false))
                    TmpMoves++;
                if (CPeg_CanMoveTo(BoardPart,X+2,Y-2,false))
                    TmpMoves++;
                if (CPeg_CanMoveTo(BoardPart,X+2,Y+2,false))
                    TmpMoves++;
                if (CPeg_CanMoveTo(BoardPart,X-2,Y+2,false))
                    TmpMoves++;
                if (CPeg_CanMoveTo(BoardPart,X-2,Y-2,false))
                    TmpMoves++;
            }
        }
    return TmpMoves;
}

// Procedure that counts how many pegs are left on the board (animphase must be lower then 2 io a red or blue peg)
int PegsLeft()
{
    CPeg *BoardPart ;
    int Pegs=0,Y,X;
    for (Y=0;Y<NrOfRows;Y++)
        for (X=0;X<NrOfCols;X++)
        {
            BoardPart = CBoardParts_GetPart(BoardParts,X,Y);
            if (BoardPart)
                if ((CPeg_GetType(BoardPart) == IDPeg) && (CPeg_GetAnimPhase(BoardPart) < 2))
                    Pegs++;
        }
    return Pegs;
}

// procedure that draws the board, boardparts info and a boxed message over the playfield
// and waits till the A button is pressed

void PrintForm(char *msg)
{
	PrintFormShown = true;
	screen.pen = Pen(255,255,255,255);
	screen.rectangle(Rect(xoffset + 16*scale,yoffset + 80*scale,(221-16)*scale,(160-80)*scale));
	screen.pen = Pen(0,0,0,255);
	screen.rectangle(Rect(xoffset + 18*scale,yoffset + 82*scale,(219-18)*scale,(158-82)*scale));
    screen.pen  = {255,255,255,255};

	screen.text(msg,font, Point(xoffset+ 21*scale,yoffset + 85*scale));
}

void LoadGraphics()
{
	IMGSelector  = Surface::load_read_only(selector);
	IMGTitle = Surface::load_read_only(titlescreen);
	IMGNewGame1 = Surface::load_read_only(newgame1);
	IMGNewGame2 = Surface::load_read_only(newgame2);
	IMGCredits1 = Surface::load_read_only(credits1);
	IMGCredits2 = Surface::load_read_only(credits2);
    IMGBackground = Surface::load_read_only(background);
    IMGPeg = Surface::load_read_only(peg);
    IMGVeryEasyInfo = Surface::load_read_only(infoveryeasy);
    IMGEasyInfo = Surface::load_read_only(infoeasy);
    IMGHardInfo = Surface::load_read_only(infohard);
    IMGVeryHardInfo = Surface::load_read_only(infoveryhard);
    IMGVeryEasy1 = Surface::load_read_only(veryeasy1);
    IMGEasy1 = Surface::load_read_only(easy1);
    IMGHard1 = Surface::load_read_only(hard1);
    IMGVeryHard1 = Surface::load_read_only(veryhard1);
    IMGCredits = Surface::load_read_only(credits);
}

void UnLoadGraphics()
{
}

// this will ceate the initial board state, io a cross of pegs, with the middle on being empty (=animphase 6)
void InitBoard()
{
	CBoardParts_RemoveAll(BoardParts);
    int X,Y;
    for (Y=0;Y < NrOfRows;Y++)
        for (X=0;X < NrOfCols;X++)
        {
            if ((Y < 3) && (X > 2) && (X < 6))
                CBoardParts_Add(BoardParts,CPeg_Create(X,Y));
             if ((Y > 2) && (Y <6))
                CBoardParts_Add(BoardParts,CPeg_Create(X,Y));
            if ((Y > 5) && (X > 2) && (X <6))
                CBoardParts_Add(BoardParts,CPeg_Create(X,Y));
        }
     CPeg_SetAnimPhase(CBoardParts_GetPart(BoardParts,4,4), 6);

}

// Checks if we won the game
bool IsWinningGame()
{
    if (PegsLeft() == 1) //must be 1 peg left
    {
        if ((Difficulty == VeryHard) || (Difficulty == Easy))
        {
            if(CPeg_GetAnimPhase(CBoardParts_GetPart(BoardParts,4,4)) < 2) // must be in the middle with veryhard or easy
                return true;
            else
                return false;
        }
        else
            return true;
    }
    else
        return false;
}

void GameInit()
{
	GameSelector = CSelector_Create(4,4);
	InitBoard();
    Moves = 0;
}

// The main Game Loop
void GameRender()
{
	if( GameState == GSGameInit)
	{
		GameInit();
		GameState -= GSInitDiff;
	}

    char Msg[300];
    screen.stretch_blit(IMGBackground,
        Rect(0,0,IMGBackground->bounds.w, IMGBackground->bounds.h),
        Rect(xoffset,yoffset, IMGBackground->bounds.w*scale, IMGBackground->bounds.h*scale));
	// Write some info to the Buffer
	screen.pen = Pen(255,255,255,175);
	sprintf(Msg,"Moves Left:%d",MovesLeft());
    screen.text(Msg,font,Point(xoffset + 161*scale,yoffset + 163*scale));
    sprintf(Msg,"Moves:%d",Moves);
	screen.text(Msg,font,Point(xoffset + 161*scale,yoffset + 179*scale));
    sprintf(Msg,"Pegs Left:%d",PegsLeft());
	screen.text(Msg,font,Point(xoffset + 161*scale,yoffset + 195*scale));
    if (saveData.BestPegsLeft[Difficulty] != 0)
    {
        sprintf(Msg,"Best Pegs:%d",saveData.BestPegsLeft[Difficulty]);
        screen.text(Msg,font,Point(xoffset + 161*scale,yoffset + 211*scale));
    }
	CBoardParts_Draw(BoardParts);
	CSelector_Draw(GameSelector);
	//need to repeat showing it until it's no longer shown
	if (PrintFormShown)
	{
		// if it's the winning game play the winning sound and show the form with the winning message
		if (IsWinningGame())
		{
			sprintf(Msg,"Congratulations you have solved\nthe puzzle! Try a new difficulty!\n\nPress (A) to continue");
			PrintForm(Msg);
		}
		else // show the loser messager, play loser sound
		{
			sprintf(Msg,"You couldn't solve the puzzle!\nDon't give up, try it again!\n\nPress (A) to continue");
			PrintForm(Msg); 
		}
	}
}

void GameLogic()
{
	if(buttons.pressed & Button::DPAD_LEFT) // move the selector to the left io currentposition.x - 1
		if (!PrintFormShown)
			CSelector_SetPosition(GameSelector, CSelector_GetPosition(GameSelector).X -1,CSelector_GetPosition(GameSelector).Y);
	if(buttons.pressed & Button::DPAD_RIGHT)
		if (!PrintFormShown)
			CSelector_SetPosition(GameSelector, CSelector_GetPosition(GameSelector).X +1,CSelector_GetPosition(GameSelector).Y);
	if(buttons.pressed & Button::DPAD_UP)
		if (!PrintFormShown)
			CSelector_SetPosition(GameSelector, CSelector_GetPosition(GameSelector).X,CSelector_GetPosition(GameSelector).Y-1);
	if(buttons.pressed & Button::DPAD_DOWN)
		if (!PrintFormShown)
			CSelector_SetPosition(GameSelector, CSelector_GetPosition(GameSelector).X,CSelector_GetPosition(GameSelector).Y+1);
	if(buttons.pressed & Button::Y)
		if (!PrintFormShown && GameSelector->HasSelection)
		{
			CPeg_SetAnimPhase(CBoardParts_GetPart(BoardParts,CSelector_GetSelection(GameSelector).X,CSelector_GetSelection(GameSelector).Y),0);
			CSelector_DeSelect(GameSelector);
		}
	if(buttons.pressed & Button::B)
	{
		playMenuBackSound();
		GameState = GSTitleScreenInit;
		PrintFormShown = false;
	}
	if(buttons.pressed & Button::A)
	{
		if(PrintFormShown)
		{
			GameState=GSTitleScreenInit;
			playMenuAcknowlege();
			PrintFormShown = false;
		}
		else
		{
			SPoint tmpSelection = CSelector_GetSelection(GameSelector);
			SPoint tmpPosition = CSelector_GetPosition(GameSelector);
			CPeg *tmpPegSelection = CBoardParts_GetPart(BoardParts,tmpSelection.X,tmpSelection.Y);
			CPeg *tmpPegPosition = CBoardParts_GetPart(BoardParts,tmpPosition.X,tmpPosition.Y);
			if (GameSelector->HasSelection) // if we have a selection
			{
				// see if the selected boardpart can move to the current position
				if (CPeg_CanMoveTo(tmpPegSelection, tmpPosition.X, tmpPosition.Y,true))
				{
					playGameAction();
					//if so play a sound, increase the moves, set the selected part to empty and the current part to red
					Moves++;
					CPeg_SetAnimPhase(tmpPegSelection, 6);
					CPeg_SetAnimPhase(tmpPegPosition, 0);
					// if no moves are left see if the best pegs left value for the current difficulty is
					// greater if so set te new value
					if (MovesLeft() == 0)
					{
						if (saveData.BestPegsLeft[Difficulty] != 0)
						{
							if (PegsLeft() < saveData.BestPegsLeft[Difficulty])
							saveData.BestPegsLeft[Difficulty] = PegsLeft();
						}
						else
							saveData.BestPegsLeft[Difficulty] = PegsLeft();
						// if it's the winning game play the winning sound and show the form with the winning message
						if (IsWinningGame())
						{
							SaveSettings();
							SelectMusic(musWinner, true);
							PrintFormShown = true;								
						}
						else // show the loser messager, play loser sound
						{
							SaveSettings();
							SelectMusic(musLoser, true);
							PrintFormShown = true;								
						}								
					}

				}
				else // if we can't move to the spot, play the wrong move sound, and reset the selection to a red peg (instead of blue / selected)
				{
					CPeg_SetAnimPhase(tmpPegSelection, 0);
					playErrorSound();
				}
				CSelector_DeSelect(GameSelector); // deselect the selection
			}
			else // we didn't have a selection, set the new selection
			{
				if (CPeg_GetAnimPhase(CBoardParts_GetPart(BoardParts,CSelector_GetPosition(GameSelector).X,CSelector_GetPosition(GameSelector).Y)) == 0)
				{
					playGameAction();
					CPeg_SetAnimPhase(CBoardParts_GetPart(BoardParts,CSelector_GetPosition(GameSelector).X,CSelector_GetPosition(GameSelector).Y),1);
					CSelector_Select(GameSelector);
				}

			}
		}
	}
}

void TitleScreenInit()
{

}

// main title screen loop
void TitleScreenLogic()
{
	if(GameState == GSTitleScreenInit)
	{
		TitleScreenInit();
		GameState -= GSInitDiff;
	}
	if(buttons.pressed & Button::DPAD_DOWN)
		CMainMenu_NextItem(MainMenu);
					
	if(buttons.pressed & Button::DPAD_UP)
		CMainMenu_PreviousItem(MainMenu);

	if(buttons.pressed & Button::A)
	{
		// set the gamestate according to the menu selection
		playMenuAcknowlege();
		switch(CMainMenu_GetSelection(MainMenu))
		{
			case 1:
				GameState = GSDifficultySelectInit;
				break;
			case 2:
				GameState = GSCreditsInit;
				break;
			case 3:
				GameState = GSQuit;
		}
	}
}

void TitleScreenRender()
{
	CMainMenu_Draw(MainMenu);
}

void DifficultySelectInit()
{

}

// Main difficulty select loop
void DifficultySelectLogic()
{
	if(GameState == GSDifficultySelectInit)
	{
		DifficultySelectInit();
		GameState -= GSInitDiff;
	}

	if(buttons.pressed & Button::B)
	{
		playMenuBackSound();
		GameState = GSTitleScreenInit;
	}
		
	if(buttons.pressed & Button::A)
	{
		playMenuAcknowlege();
		GameState = GSGameInit;
	}
		
	if(buttons.pressed & Button::DPAD_LEFT) // Change difficluly one lower if we pressed left
	{
		playMenuSelectSound();
		if(Difficulty == VeryHard)
		{
			Difficulty = Hard;
		}
		else
		{
			if(Difficulty == Hard)
			{
				Difficulty = Easy;
			}
			else
			{
				if(Difficulty == Easy)
				{
					Difficulty = VeryEasy;
				}
				else
				{
					if(Difficulty == VeryEasy)
						Difficulty = VeryHard;
				}
			}
		}
	}
	
	if(buttons.pressed & Button::DPAD_RIGHT) // change difficulty one higher
	{
		playMenuSelectSound();
		if(Difficulty == VeryEasy)
		{
			Difficulty = Easy;
		}
		else
		{
			if(Difficulty == Easy)
			{
				Difficulty = Hard;
			}
			else
			{
				if(Difficulty == Hard)
				{
					Difficulty = VeryHard;
				}
				else
				{
					if(Difficulty == VeryHard)
						Difficulty = VeryEasy;
				}
			}
		}
	}
}
	
void DifficultySelectRender()
{ 	
	Rect Dest;
	// decide what we draw to the buffer based on the difficuly
	switch(Difficulty)
	{
		case VeryEasy:
			screen.stretch_blit(IMGVeryEasyInfo,
				Rect(0,0,IMGVeryEasyInfo->bounds.w, IMGVeryEasyInfo->bounds.h),
				Rect(xoffset,yoffset, IMGVeryEasyInfo->bounds.w*scale, IMGVeryEasyInfo->bounds.h*scale));
			Dest.x = xoffset + (WINDOW_WIDTH/2 - IMGVeryEasy1->bounds.w/2) * scale;
			Dest.y = yoffset + 35 * scale;
			Dest.w = IMGVeryEasy1->bounds.w * scale;
			Dest.h = IMGVeryEasy1->bounds.h * scale;
			screen.stretch_blit(IMGVeryEasy1,
				Rect(0,0,IMGVeryEasy1->bounds.w, IMGVeryEasy1->bounds.h),Dest);
			break;
		case Easy:
			screen.stretch_blit(IMGEasyInfo,
				Rect(0,0,IMGEasyInfo->bounds.w, IMGEasyInfo->bounds.h),
				Rect(xoffset,yoffset, IMGEasyInfo->bounds.w*scale, IMGEasyInfo->bounds.h*scale));
			Dest.x = xoffset + (WINDOW_WIDTH/2 - IMGEasy1->bounds.w/2) * scale;
			Dest.y = yoffset + 35 * scale;
			Dest.w = IMGEasy1->bounds.w * scale;
			Dest.h = IMGEasy1->bounds.h * scale;
			screen.stretch_blit(IMGEasy1,
				Rect(0,0,IMGEasy1->bounds.w, IMGEasy1->bounds.h),Dest);
			break;
		case Hard:
			screen.stretch_blit(IMGHardInfo,
				Rect(0,0,IMGHardInfo->bounds.w, IMGHardInfo->bounds.h),
				Rect(xoffset,yoffset, IMGHardInfo->bounds.w*scale, IMGHardInfo->bounds.h*scale));
			Dest.x = xoffset + (WINDOW_WIDTH/2 - IMGHard1->bounds.w/2) * scale;
			Dest.y = yoffset + 35 * scale;
			Dest.w = IMGHard1->bounds.w * scale;
			Dest.h = IMGHard1->bounds.h * scale;
			screen.stretch_blit(IMGHard1,
				Rect(0,0,IMGHard1->bounds.w, IMGHard1->bounds.h),Dest);
			break;
		case VeryHard:
			screen.stretch_blit(IMGVeryHardInfo,
				Rect(0,0,IMGVeryHardInfo->bounds.w, IMGVeryHardInfo->bounds.h),
				Rect(xoffset,yoffset, IMGVeryHardInfo->bounds.w*scale, IMGVeryHardInfo->bounds.h*scale));
			Dest.x = xoffset + (WINDOW_WIDTH/2 - IMGVeryHard1->bounds.w/2) * scale;
			Dest.y = yoffset + 35 * scale;
			Dest.w = IMGVeryHard1->bounds.w * scale;
			Dest.h = IMGVeryHard1->bounds.h * scale;
			screen.stretch_blit(IMGVeryHard1,
				Rect(0,0,IMGVeryHard1->bounds.w, IMGVeryHard1->bounds.h),Dest);
			break;

	}
}

//Main Credits loop, will just show an image and wait for a button to be pressed
void CreditsLogic()
{
	if((buttons.pressed & Button::A) || (buttons.pressed & Button::B))
	{
		playMenuAcknowlege();
		GameState = GSTitleScreenInit;
	}
}

void CreditsRender()
{
	screen.stretch_blit(IMGCredits,
		Rect(0,0,IMGCredits->bounds.w, IMGCredits->bounds.h),
		Rect(xoffset,yoffset, IMGCredits->bounds.w*scale, IMGCredits->bounds.h*scale));
}

void init() 
{
	set_screen_mode(ScreenMode::hires);
    scale = (float)screen.bounds.w / WINDOW_WIDTH;
    if ((float)WINDOW_HEIGHT * scale > screen.bounds.h)
        scale = (float)screen.bounds.h / WINDOW_HEIGHT;
    xoffset = (screen.bounds.w - ((float)WINDOW_WIDTH*scale)) / 2;
    yoffset = (screen.bounds.h - ((float)WINDOW_HEIGHT*scale)) / 2;
	BoardParts = CBoardParts_Create();
	MainMenu = CMainMenu_Create();
	initSound();
	initMusic();
	setSoundOn(true);
	setMusicOn(true);

	LoadSettings();
    LoadGraphics();		

}

void render(uint32_t time) 
{
	
	
	screen.pen = Pen(69,88,234);
	screen.clear();

	switch(GameState)
	{
			case GSGameInit:
			case GSGame :
				GameRender();
				break;
			case GSTitleScreenInit:
			case GSTitleScreen:
				TitleScreenRender();
				break;
			case GSDifficultySelectInit:
			case GSDifficultySelect:
				DifficultySelectRender();
				break;
			case GSCreditsInit:
			case GSCredits:
				CreditsRender();
				break;
			default:
				break;
	}
	printDebugCpuRamFpsLoad(startus, now_us());
	startus = now_us();
}


void update(uint32_t time) 
{	
	switch(GameState)
	{
			case GSGameInit:
			case GSGame :
				GameLogic();
				break;
			case GSTitleScreenInit:
			case GSTitleScreen:
				TitleScreenLogic();
				break;
			case GSDifficultySelectInit:
			case GSDifficultySelect:
				DifficultySelectLogic();
				break;
			case GSCreditsInit:
			case GSCredits:
				CreditsLogic();
				break;
			default:
				break;
	}
}
