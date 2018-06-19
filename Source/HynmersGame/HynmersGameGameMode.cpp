// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "HynmersGameGameMode.h"
#include "HynmersGameHUD.h"
#include "HynmersGameCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

AHynmersGameGameMode::AHynmersGameGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Test/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AHynmersGameHUD::StaticClass();
}

bool AHynmersGameGameMode::CanSpawnAtLocation(FVector Location, FVector BoxHalfStent)
{
	FHitResult HitResult;
	return GetWorld()->SweepSingleByChannel(HitResult, Location, Location, FQuat::Identity, ECollisionChannel::ECC_WorldStatic, FCollisionShape::MakeBox(BoxHalfStent));
}

void AHynmersGameGameMode::SpawnMapInSession(FMapSavedParameters MapParameters, TSubclassOf<AActor> BridgeClass)
{
	if (!MapParameters.TilesToBeSpawned.Num())
		return;
	TArray<AActor*> BridgeTiles;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),BridgeClass, BridgeTiles);

	

}
