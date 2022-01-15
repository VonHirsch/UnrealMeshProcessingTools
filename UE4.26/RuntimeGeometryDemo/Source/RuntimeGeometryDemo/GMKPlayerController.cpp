


#include "GMKPlayerController.h"

#include "GMKCheatManager.h"

AGMKPlayerController::AGMKPlayerController()
{
	// this is equivalent to setting the cheatclass on the playercontroller in the editor BP screen
	CheatClass = UGMKCheatManager::StaticClass();
}

void AGMKPlayerController::BeginPlay()
{
	AddCheats(true);
}
