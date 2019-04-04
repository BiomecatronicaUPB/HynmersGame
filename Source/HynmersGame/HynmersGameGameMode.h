// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "HynmersSaveGame.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HGameInstance.h"
#include "HynmersGame.h" 
#include "HynmersGameGameMode.generated.h"

class UHynmersSaveGame;
class AActor;

UCLASS(minimalapi)
class AHynmersGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

	UHGameInstance* GameInstance;

public:
	AHynmersGameGameMode();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	bool SpawnSessionMap(FMapSavedParameters MapParameters);


	UFUNCTION(BlueprintImplementableEvent, Category = "BeginPlay")
		void InitMechanics();

protected:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	bool SpawnCurrentSession();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "BeginPlay")
	bool InitBlueprints();

	UFUNCTION(BlueprintImplementableEvent, Category = "BeginPlay")
	void PostBeginPlay();
	   
	UFUNCTION(BlueprintImplementableEvent, Category = "SavedGame")
	UHynmersSaveGame* LoadSavedGameData();

	UFUNCTION(BlueprintImplementableEvent, Category = "BridgeGates")
	FTransform UpdateBridgeGatesTransform(FTransform TransformToDelete);

	UFUNCTION(BlueprintImplementableEvent, Category = "MapGeneration")
	class AHynmersConnector* SpawnConnector(FTransform Location, FVector SplineEndLocation);

	UFUNCTION(BlueprintImplementableEvent, Category = "MapGeneration")
	AActor* SpawnTile(TSubclassOf<AActor> TileToSpawn, FTransform Transform);

	UFUNCTION(BlueprintImplementableEvent, Category = "MapGeneration")
	AActor* CheckMachineRoom(const TArray<TSubclassOf<AActor>>& TilesToBeSpawned, int32& TileIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Utility")
	void ShuffleArray(TArray<TSubclassOf<AActor>>& Array);

	UPROPERTY(BlueprintReadWrite, Category = "SavedGame")
	UHynmersSaveGame* MapSaved = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "SavedGame")
	TArray<FMapSavedParameters> GameStateStruct;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SavedGame")
	FString SlotName = "Default";

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MapGeneration")
	int32 MaxGenerationTries = 100;

	UPROPERTY(BlueprintReadWrite, Category = "MapGeneration")
	TArray<FSessionInfo> GameSessionInfo;

	UPROPERTY(BlueprintReadWrite, Category = "MapGeneration")
		TArray<class AHynmersBaseTile*> TilesSpawned;

	UPROPERTY(BlueprintReadWrite, Category = "MapGeneration")
		TArray<AHynmersConnector*> ConnectorsSPawned;

	UPROPERTY(BlueprintReadWrite, Category = "MapGeneration")
		bool bActiveBridge = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MapGeneration")
	TMap<EActiveTile, TSubclassOf<AHynmersBaseTile>> Tiles;

	// Angle in degrees
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MapGeneration")
	float ConeHalfAngle = 90.f;
};



