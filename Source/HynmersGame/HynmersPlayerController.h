// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HynmersPlayerController.generated.h"

/**
 * 
 */

class UAnimSequence;

UCLASS()
class HYNMERSGAME_API AHynmersPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	/// ------------------------------------- Animation sequences for exercises ----------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations", meta = (AllowPrivateAccess = "true"))
	UAnimSequence* WalkSequence = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations", meta = (AllowPrivateAccess = "true"))
	UAnimSequence* JumpSequence = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations", meta = (AllowPrivateAccess = "true"))
	UAnimSequence* SwimSequence = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations", meta = (AllowPrivateAccess = "true"))
	UAnimSequence* BoardSequence = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations", meta = (AllowPrivateAccess = "true"))
	UAnimSequence* BikeSequence = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations", meta = (AllowPrivateAccess = "true"))
	UAnimSequence* LegsMoveSequence = nullptr;

	/// -------------------------------------------Algorithm parameters ----------------------------------------------------------------------------------------
	// Influence of the thigh in the final error. The knee influence would be the complementary value
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Walk Curve", meta = (AllowPrivateAccess = "true"))
	float ThighWeight = 0.8f;

	// Influence of the knee in the final error
	float KneeWeight =0.2f;

	///---------------------------------------------------------------------------------------------------------------------------------------------------------
	// Animation objects
	class AHynmersCharacter* ControlledPawn = nullptr;
	class UAnimInstance* PawnAnimationInstance = nullptr;

	/// --------------------------------------- Variable needed for animation of sekeltal mesh ------------------------------------------------------------------
	// Array with the animation sequecnes for exercises, to easily access in loops
	TArray<UAnimSequence*> Sequences;
	// Montages created of the sequeces. To put and specific frame in the game
	TArray<UAnimMontage*> Montages;
	// Map montages with maps
	TMap<int32, int32> MapMontage;
	// Angles read ir from the kinec with their corresponding bone names
	TMap<FName,float> BonesAngles;
	// Offset for each bones in aniamtion sequences
	TMap<FName, float> BonesOffsets;
	/// ---------------------------------------------------------------------------------------------------------------------------------------------------------
	
	// Function that conver radians to angles and move to 0
	float ConvertKinectAngle(float Rate);

	/// -----------------------------------------------------Algorithm functions --------------------------------------------------------------------------------
	// Get error using weights of joints
	float GetError(TMap<FName, float> &BonesValues, TArray<float> &AnimValues);
	TArray<float> GetAnimValues(UAnimSequence* AnimSequence, float Time, TArray<FName> BoneNames);
	void SetMontagePosition(UAnimMontage* Montage, float Time);
	// Convert frames as the animation is a loop aniamtion
	int32 ConvertFrame(int32 NumberOfFrames, int32 FrameToCorrect);
	/// ---------------------------------------------------------------------------------------------------------------------------------------------------------

protected:
	virtual void BeginPlay() override;

	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;

	virtual void SetupInputComponent() override;

	/// Check all assets used
	bool CheckAnimAssets();
	bool CheckAllNeededAssets();
	bool CheckMontages();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void CreateMontage(UAnimSequence* Sequence, UAnimMontage* &OutMontage);


public:
	AHynmersPlayerController();

	// Kinect Input Events
	void PosLeftThight(float rate);
	void PosLeftKnee(float rate);
	void PosRightThight(float rate);
	void PosRightKnee(float rate);
};
