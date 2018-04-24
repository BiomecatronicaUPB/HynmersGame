// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "HynmersGameGameMode.h"
#include "HynmersGameHUD.h"
#include "HynmersGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

AHynmersGameGameMode::AHynmersGameGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AHynmersGameHUD::StaticClass();
}
