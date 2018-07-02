// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "HynmersGameGameMode.h"
#include "HynmersGameHUD.h"
#include "HynmersGameCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "HynmersBaseTile.h"
#include "HynmersConnector.h"

AHynmersGameGameMode::AHynmersGameGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Test/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AHynmersGameHUD::StaticClass();
}

void AHynmersGameGameMode::BeginPlay()
{
	Super::BeginPlay();

	InitBlueprints();
	MapSaved = LoadSavedGameData();
	if (ensure(MapSaved)) {
		for (FMapSavedParameters Session : MapSaved->SavedMap) {
			SpawnSessionMap(Session);
		}
		PostBeginPlay();
	}
}

bool AHynmersGameGameMode::CanSpawnAtLocation(FVector Location, FVector BoxHalfStent)
{
	FHitResult HitResult;
	return GetWorld()->SweepSingleByChannel(HitResult, Location, Location, FQuat::Identity, ECollisionChannel::ECC_WorldStatic, FCollisionShape::MakeBox(BoxHalfStent));
}

bool AHynmersGameGameMode::SpawnSessionMap(FMapSavedParameters MapParameters)
{
	if(MapParameters.TilesToBeSpawned.Num() == 0)
		return false;

	FTransform GateTransform = UpdateBridgeGatesTransform(MapParameters.BridgeInitialTransform);
	for (int i = 0; i < MapParameters.TilesToBeSpawned.Num(); i++) {
		auto Tile = MapParameters.TilesToBeSpawned[i];

		for (int attemp = 0; attemp < MaxGenerationTries; attemp++) {
			FVector SplineEndLocation = (MapParameters.SplinesEndLocations.Num() > 0) ? 
				MapParameters.SplinesEndLocations[i] : UKismetMathLibrary::RandomUnitVectorInConeInDegrees(FVector(1, 0, 0), ConeHalfAngle);

			AHynmersConnector* Connector = SpawnConnector(GateTransform, SplineEndLocation);
			break;

		}
	}

	return true;
}
