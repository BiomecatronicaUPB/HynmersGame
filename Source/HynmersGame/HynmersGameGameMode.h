// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HynmersGameGameMode.generated.h"

USTRUCT(BlueprintType)
struct FMapSavedParameters {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Map")
	TArray<FVector> SplinesEndLocations;

	UPROPERTY(BlueprintReadWrite, Category = "Map")
	FTransform BridgeInitialTransfor;

	UPROPERTY(BlueprintReadWrite, Category = "Map")
	TArray<TSubclassOf<AActor>> TilesToBeSpawned;
};

UCLASS(minimalapi)
class AHynmersGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHynmersGameGameMode();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	bool CanSpawnAtLocation(FVector Location, FVector BoxHalfStent);

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnMapInSession(FMapSavedParameters MapParameters, TSubclassOf<AActor> BridgeClass);
};



