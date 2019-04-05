// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "HGameInstance.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FSessionInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 ExerciseIndex;
	UPROPERTY(BlueprintReadWrite)
	int32 Series;
	UPROPERTY(BlueprintReadWrite)
	int32 Repetitions;
	UPROPERTY(BlueprintReadWrite)
	TArray<float> UpperLimits;
	UPROPERTY(BlueprintReadWrite)
	TArray<float> LowerLimits;
};

UCLASS()
class HYNMERSGAME_API UHGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	TArray<FSessionInfo> SessionInfo; 

	TSubclassOf<class UUserWidget> MenuClass;
	class UHMainMenu* Menu;

	bool LookfForExercise(const FSessionInfo &IndexToFind, int32 &ExerciseFound);

public:

	UPROPERTY(BlueprintReadWrite)
	FString PatientName;

	UHGameInstance(const FObjectInitializer & ObjectInitializer);

	bool AddExercise(FSessionInfo& NewExercise);
	void RemoveExercise(FSessionInfo& NewExercise);

	void GetGameSessionInfo(TArray<FSessionInfo>& GameSessionInfo);

	void StartGame();

	UFUNCTION(BlueprintCallable)
	void LoadMenuWidget();
	
};
