#include <32blit.hpp>
#include "cmainmenu.hpp"
#include "commonvars.hpp"
#include "assets.hpp"
#include "sound.hpp"

using namespace blit;

CMainMenu* CMainMenu_Create()
{
	CMainMenu* Result = (CMainMenu*) malloc(sizeof(CMainMenu));
    Result->Selection = 1;
    return Result;
}

 // return the current selection
int CMainMenu_GetSelection(CMainMenu* MainMenu)
{
	return MainMenu->Selection;
}

// Destructor will free the surface images
void CMainMenu_Destroy(CMainMenu* MainMenu)
{

}

// Increase the selection if it goes to far set i to the first selection
void CMainMenu_NextItem(CMainMenu* MainMenu)
{
    MainMenu->Selection++;
    if (MainMenu->Selection == 3)
        MainMenu->Selection = 1;
    playMenuSelectSound();
}

// decrease the selection if it goes to low set it to the last selection
void CMainMenu_PreviousItem(CMainMenu* MainMenu)
{
    MainMenu->Selection--;
    if (MainMenu->Selection == 0)
        MainMenu->Selection = 2;
    playMenuSelectSound();
}

// Draw the main menu
void CMainMenu_Draw(CMainMenu* MainMenu)
{
    Rect Dest;

  
    // draw the title screen background
    screen.stretch_blit(IMGTitle,
        Rect(0,0,IMGTitle->bounds.w, IMGTitle->bounds.h),
        Rect(xoffset,yoffset, IMGTitle->bounds.w*scale, IMGTitle->bounds.h*scale));

    // if selection = newgame draw the purple new game image in the middle of the screen
    if (MainMenu->Selection == 1)
    {
        Dest.x = xoffset + (WINDOW_WIDTH/2 - IMGNewGame1->bounds.w/2)*scale;
        Dest.y = yoffset + 100*scale;
        Dest.w = IMGNewGame1->bounds.w*scale;
        Dest.h = IMGNewGame1->bounds.h*scale;
        screen.stretch_blit(IMGNewGame1,
            Rect(0,0,IMGNewGame1->bounds.w, IMGNewGame1->bounds.h),Dest);
    
    }
    else
    // draw the blue one
    {
        Dest.x = xoffset + (WINDOW_WIDTH/2 - IMGNewGame2->bounds.w/2)*scale;
        Dest.y = yoffset + 100*scale;
        Dest.w = IMGNewGame2->bounds.w*scale;
        Dest.h = IMGNewGame2->bounds.h*scale;
        screen.stretch_blit(IMGNewGame2,
            Rect(0,0,IMGNewGame2->bounds.w, IMGNewGame2->bounds.h),Dest);
    }

    if (MainMenu->Selection == 2) // = credits draw purpe
    {
        Dest.x = xoffset + (WINDOW_WIDTH/2 - IMGCredits1->bounds.w/2)*scale;
        Dest.y = yoffset + 140*scale;
        Dest.w = IMGCredits1->bounds.w*scale;
        Dest.h = IMGCredits1->bounds.h*scale;
        screen.stretch_blit(IMGCredits1,
            Rect(0,0,IMGCredits1->bounds.w, IMGCredits1->bounds.h),Dest);
    }
    else // blue
    {
        Dest.x = xoffset + (WINDOW_WIDTH/2 - IMGCredits2->bounds.w/2)*scale;
        Dest.y = yoffset + 140*scale;
        Dest.w = IMGCredits2->bounds.w*scale;
        Dest.h = IMGCredits2->bounds.h*scale;
        screen.stretch_blit(IMGCredits2,
            Rect(0,0,IMGCredits2->bounds.w, IMGCredits2->bounds.h),Dest);
    }
}
