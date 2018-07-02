// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "HynmersSaveGame.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HynmersGameGameMode.generated.h"

class UHynmersSaveGame;

UCLASS(minimalapi)
class AHynmersGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHynmersGameGameMode();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	bool CanSpawnAtLocation(FVector Location, FVector BoxHalfStent);

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	bool SpawnSessionMap(FMapSavedParameters MapParameters);

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "BeginPlay")
	void InitBlueprints();

	UFUNCTION(BlueprintImplementableEvent, Category = "BeginPlay")
	void PostBeginPlay();

	UFUNCTION(BlueprintImplementableEvent, Category = "SavedGame")
	UHynmersSaveGame* LoadSavedGameData();

	UFUNCTION(BlueprintImplementableEvent, Category = "BridgeGates")
	FTransform UpdateBridgeGatesTransform(FTransform TransformToDelete);

	UFUNCTION(BlueprintImplementableEvent, Category = "MapGeneration")
	class AHynmersConnector* SpawnConnector(FTransform Location, FVector SplineEndLocation);

	UPROPERTY(BlueprintReadWrite, Category = "SavedGame")
	UHynmersSaveGame* MapSaved = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MapGeneration")
	int32 MaxGenerationTries = 100;

	// Angle in degrees
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MapGeneration")
	float ConeHalfAngle = 90.f;
};



