#pragma once

//#include "SlateBasics.h"
//#include "SlateExtras.h"
//#include "GShiftOptions.h"
//
//class AGShiftPlayerController;
//class SWeakWidget;
//
//class FGShiftInGameMenu : public FGameMenuPage
//{
//public:
//
//	/* Sets owning Player Controller */
//	void MakeMenu(AGShiftPlayerController* InPCOwner);
//
//	/* Toggles in Game Menu */
//	void ToggleGameMenu();
//
//	/* Is Game Menu Currently Active*/
//	void GetIsGameMenuUp(bool& bIsUp) const;
//
//	void ResumeGame();
//
//	void ReturnToMainMenu();
//
//	void DetachGameMenu();
//
//protected:
//
//	/* Game Menu Widget Container - used for removing */
//	TSharedPtr<SWeakWidget> GameMenuContainer;
//
//	/* If Game menu is currently opened*/
//	uint32 bIsGameMenuUp : 1;
//
//	/* Called when going back to previous menu*/
//	void OnMenuGoBack();
//
//	/* Close the menu and return to Main menu*/
//	void CloseAndExit();
//
//	/* Cached actor hidden state */
//	uint32 bWasActorHidden : 1;
//
//
//};