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
#include "Components/BoxComponent.h"

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


	FMapSavedParameters CurrentSession;
	FTransform GateTransform = UpdateBridgeGatesTransform(MapParameters.BridgeInitialTransform);

	CurrentSession.BridgeInitialTransform = GateTransform;

	for (int i = 0; i < MapParameters.TilesToBeSpawned.Num(); i++) {

		int attemp;
		for (attemp = 0; attemp < MaxGenerationTries; attemp++) {
			FVector SplineEndLocation = (MapParameters.SplinesEndLocations.Num() > 0) ? 
				MapParameters.SplinesEndLocations[i] : UKismetMathLibrary::RandomUnitVectorInConeInDegrees(FVector(1, 0, 0), ConeHalfAngle);

			AHynmersConnector* Connector = SpawnConnector(GateTransform, SplineEndLocation);
			FTransform TileTransform = Connector->GetAttachLocation();

			AHynmersBaseTile* SpawnedTile =(AHynmersBaseTile*) GetWorld()->SpawnActor<AActor>(MapParameters.TilesToBeSpawned[i], TileTransform);

			TArray<AActor*> OverlappingActors;
			SpawnedTile->Box->GetOverlappingActors(OverlappingActors);

			if (OverlappingActors.Num() == 0) {
				CurrentSession.TilesToBeSpawned.Add(MapParameters.TilesToBeSpawned[i]);
				CurrentSession.SplinesEndLocations.Add(SplineEndLocation);
				GateTransform = SpawnedTile->GetAttachLocation();
				break;
			}

			SpawnedTile->Destroy();
			if(SpawnedTile)
				delete SpawnedTile;

			Connector->Destroy();
			if(Connector)
				delete Connector;
		}

		if (attemp == MaxGenerationTries - 1) {
			UE_LOG(LogTemp, Error, TEXT("Map generation failed"));
			return false;
		}
	}

	GameStateStruct.Add(CurrentSession);

	return true;
}
