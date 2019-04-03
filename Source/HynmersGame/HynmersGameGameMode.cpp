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
#include "Kismet/KismetArrayLibrary.h"

#include "HGameInstance.h"

AHynmersGameGameMode::AHynmersGameGameMode()
{
	// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Test/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	//DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AHynmersGameHUD::StaticClass();
}

void AHynmersGameGameMode::BeginPlay()
{
	Super::BeginPlay();

	bool Init = InitBlueprints();

	GameInstance = Cast<UHGameInstance>(GetGameInstance());
	if (!GameInstance)return;

	SlotName = GameInstance->PatientName;
	GameInstance->GetGameSessionInfo(GameSessionInfo);

	MapSaved = LoadSavedGameData();

	if (ensure(MapSaved)) {
		for (FMapSavedParameters Session : MapSaved->SavedMap) {
			SpawnSessionMap(Session);
		}
		PostBeginPlay(); 
		SpawnCurrentSession();
	}
}

bool AHynmersGameGameMode::SpawnCurrentSession()
{
	if (Tiles.Num() < (int32)EActiveTile::TotalTiles)return false;

	TArray<TSubclassOf<AActor>> OutputTiles;

	for (FSessionInfo CurrentExercise : GameSessionInfo)
	{
		if(Tiles.Contains((EActiveTile)CurrentExercise.ExerciseIndex) && CurrentExercise.ExerciseIndex != (int32)EActiveTile::Bridge)
			OutputTiles.Add(Tiles[(EActiveTile)CurrentExercise.ExerciseIndex]);
	}

	ShuffleArray(OutputTiles);
	TArray<FVector> Locations;
	SpawnSessionMap({ Locations, FTransform::Identity, OutputTiles });

	return true;
}

bool AHynmersGameGameMode::SpawnSessionMap(FMapSavedParameters MapParameters)
{
	if(MapParameters.TilesToBeSpawned.Num() == 0)
		return false;

	FMapSavedParameters CurrentSession;
	FTransform GateTransform;
	int32 MachineRoomIndex;
	AHynmersBaseTile* MachineRoomTile = (AHynmersBaseTile*)CheckMachineRoom(MapParameters.TilesToBeSpawned, MachineRoomIndex);

	if (MachineRoomTile) {
		GateTransform = MachineRoomTile->GetAttachLocation();
		// Add to the save game the machine room
		CurrentSession.TilesToBeSpawned.Add(MapParameters.TilesToBeSpawned[MachineRoomIndex]);
		// Delete from the load game the machine room cause it is not spawned
		MapParameters.TilesToBeSpawned.RemoveAt(MachineRoomIndex);
	}
	else {
		GateTransform = UpdateBridgeGatesTransform(MapParameters.BridgeInitialTransform);
	}

	//UE_LOG(LogTemp, Warning, TEXT("%s"), *GateTransform.ToString());

	CurrentSession.BridgeInitialTransform = GateTransform;

	for (int i = 0; i < MapParameters.TilesToBeSpawned.Num(); i++) {

		int attemp;
		for (attemp = 0; attemp < MaxGenerationTries; attemp++) {
			FVector SplineEndLocation = (MapParameters.SplinesEndLocations.Num() > 0) ? 
				MapParameters.SplinesEndLocations[i] : UKismetMathLibrary::RandomUnitVectorInConeInDegrees(FVector(1, 0, 0), ConeHalfAngle);

			AHynmersConnector* Connector = SpawnConnector(GateTransform, SplineEndLocation);
			FTransform TileTransform = Connector->GetAttachLocation();

			AHynmersBaseTile* SpawnedTile = (AHynmersBaseTile*)SpawnTile(MapParameters.TilesToBeSpawned[i], TileTransform);

			TArray<AActor*> OverlappingActors;
			SpawnedTile->Box->GetOverlappingActors(OverlappingActors);

			if (OverlappingActors.Num() == 0) {
				CurrentSession.TilesToBeSpawned.Add(MapParameters.TilesToBeSpawned[i]);
				CurrentSession.SplinesEndLocations.Add(SplineEndLocation);
				break;
			}

			SpawnedTile->Destroy();
			Connector->Destroy();
		}

		if (attemp == MaxGenerationTries - 1) {
			UE_LOG(LogTemp, Error, TEXT("Map generation failed"));
			return false;
		}
	}

	GameStateStruct.Add(CurrentSession);

	return true;
}
