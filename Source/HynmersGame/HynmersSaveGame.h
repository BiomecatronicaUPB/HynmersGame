// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "HynmersSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FMapSavedParameters {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Map")
	TArray<FVector> SplinesEndLocations;

	UPROPERTY(BlueprintReadWrite, Category = "Map")
	FTransform BridgeInitialTransform;

	UPROPERTY(BlueprintReadWrite, Category = "Map")
	TArray<TSubclassOf<AActor>> TilesToBeSpawned;
};

/**
 * 
 */
UCLASS()
class HYNMERSGAME_API UHynmersSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, Category = "Data")
	TArray<FMapSavedParameters> SavedMap;
};
