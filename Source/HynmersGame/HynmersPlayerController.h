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

	float PastAnimTime = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Walk Curve", meta = (AllowPrivateAccess = "true"))
	int MaxAccelTimes = 50;
	
	int AccelTimes = 0;
	bool bInitAccel = false;

	// Max range amplitude to make binary search
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Walk Curve", meta = (AllowPrivateAccess = "true"))
	int32 NumBetterFrames = 6;

	// Depth to stop the binary search
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Walk Curve", meta = (AllowPrivateAccess = "true"))
	int32 BinarySearchDepth = 6;

	// Range of frame to search for better frames
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Walk Curve", meta = (AllowPrivateAccess = "true"))
	int32 NumSearchFrames = 100;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Walk Curve", meta = (AllowPrivateAccess = "true"))
	float ThighWeight = 120.f / 40.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Walk Curve", meta = (AllowPrivateAccess = "true"))
	float KneeWeight = 120.f / 80.f;

	TArray<int> FindMinsInArray(TArray<float> Array, int32 Num);

	float FindTimeInRange(TMap<FName,float> BonesRotation, TArray<float> Range, UAnimSequence * Sequence, int32 CurrentDepth, float* Error);

	float CalculateTimeValue(TMap<FName, float> &BonesRotation, UAnimSequence * Sequence, float  Time);

	TMap<FName,float> BonesAngles;

protected:
	virtual void BeginPlay() override;

	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;

	float SelectSkeletonPose(TArray<int> FrameSearchRange);

	virtual void SetupInputComponent() override;
	
	UFUNCTION(BlueprintCallable, Category="Animation")
	void CreateMontage();

	// Returns the time found int he sequence with the minimun rotation errors
	float FindPoseInMontage(TMap<FName, float> BonesRotation, UAnimSequence* Sequence, TArray<int> FrameSearchRange, float* Error = nullptr);
	FQuat GetRefPoseQuat(TMap<FName,float> &BonesRotation, int bone);

public:

	// Kinect Input Events
	void PosLeftThight(float rate);
	void PosLeftKnee(float rate);
	void PosRightThight(float rate);
	void PosRightKnee(float rate);
};
