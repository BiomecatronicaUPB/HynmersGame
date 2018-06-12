// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HynmersPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class HYNMERSGAME_API AHynmersPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Walk Curve", meta = (AllowPrivateAccess = "true"))
	class UAnimSequence* WalkSequence = nullptr;

	// Animation objects
	class AHynmersCharacter* ControlledPawn = nullptr;
	class UAnimInstance* PawnAnimationInstance = nullptr;
	class UAnimMontage* WalkMontage = nullptr;

	// Offset between left and rigth legs
	int32 OffsetFrame = 15;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Walk Curve", meta = (AllowPrivateAccess = "true"))
	int32 NumBetterFrames = 6;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Walk Curve", meta = (AllowPrivateAccess = "true"))
	int32 BinarySearchDepth = 6;

	TArray<int> FindMinsInArray(TArray<float> Array, int32 Num);
	TArray<int> FindFrameRange(TArray<TArray<int>> Frames, int NumBones, int NumFrames);

	bool ChangeRange(TArray<int32> &Range, TArray<int32> Frames);
	float FindTimeInRange(TArray<TPair<FName, float>> BonesRotation, TArray<float> Range, UAnimSequence * Sequence, int32 CurrentDepth, float* Error);

protected:
	virtual void BeginPlay() override;

	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;

	virtual void SetupInputComponent() override;
	
	UFUNCTION(BlueprintCallable, Category="Animation")
	void CreateMontage();

	// Returns the time found int he sequence with the minimun rotation errors
	float FindPoseInMontage(TArray<TPair<FName, float>> BonesRotation, UAnimSequence* Sequence, float* Error = nullptr);
	FQuat GetRefPoseQuat(TArray<TPair<FName, float>> &BonesRotation, int bone);
};
