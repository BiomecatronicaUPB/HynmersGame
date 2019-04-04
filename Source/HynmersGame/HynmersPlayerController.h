// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HynmersGame.h"
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Algorithm", meta = (AllowPrivateAccess = "true"))
	float ThighWeight = 0.8f;

	// Depth to stop the binary search
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Algorithm", meta = (AllowPrivateAccess = "true"))
	int32 BinarySearchDepth = 6;

	// Default number of frames to search
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Algorithm", meta = (AllowPrivateAccess = "true"))
	int32 FrameRange = 10;

	// Default number of frame forward
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Algorithm", meta = (AllowPrivateAccess = "true"))
	int32 OffsetFrames = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Algorithm", meta = (AllowPrivateAccess = "true"))
	TMap<EActiveTile, int32> AnimationSpecificFrameRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Algorithm", meta = (AllowPrivateAccess = "true"))
	TMap<EActiveTile, int32> AnimationSpecificFramesOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Algorithm", meta = (AllowPrivateAccess = "true"))
	TMap<EActiveTile, int32> AnimationSpecificThighLOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Algorithm", meta = (AllowPrivateAccess = "true"))
	TMap<EActiveTile, int32> AnimationSpecificThighROffset;

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
	// Angles read ir from the kinec with their corresponding bone names
	TMap<FName,float> BonesAngles;
	// Offset for each bones in aniamtion sequences
	TMap<FName, float> BonesOffsets;

	EActiveTile ActiveTile;
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
	// Return Min frame and Max frame that acomplish the number of rames specified by frame range with Current frame as center
	TArray<int32> GetFrameRange(UAnimMontage* ActiveMontage, int32 FrameRange);
	// Get Error of the specified frame
	float GetFrameErrorInTime(UAnimSequence* ActiveSequence, const float& FrameTime, TArray<FName> &BonesKeys);
	// Get two adjacent frames with the least combined error
	TArray<int32> GetBetterFrames(UAnimSequence* ActiveSequence, TArray<FName> &BonesKeys, const TArray<int32> &Frames);
	// Return time with least error in the frame range
	float BinarySearch(UAnimSequence* ActiveSequence, const TArray<float> &BetterTimes, TArray<FName> &BonesKeys, int32 Depth = 0);

	void TriggerNotifies(UAnimSequence * ActiveSequence, float CurrentTime, float BestTime);
	/// ---------------------------------------------------------------------------------------------------------------------------------------------------------


	/// ---------------------------------------Mechanics variables-----------------------------------------------------------------------------------------------
	int32 TagetRepetitions;
	int32 NumberOfRepetitions = 0;
	///-----------------------------------------------------------------------------------------------------------------------------------------------------------

	/// --------------------------------------Movement Mechanic------------------------------------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MovementAlgorithm", meta = (AllowPrivateAccess = "true"))
	AActor* ActivePickUpActor;

	UPROPERTY(BlueprintReadWrite, Category = "MovementAlgorithm", meta = (AllowPrivateAccess = "true"))
	float DistanceToPickUpActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MovementAlgorithm", meta = (AllowPrivateAccess = "true"))
	float MaxSpeed = 148.2f;

	UPROPERTY(BlueprintReadWrite, Category = "MovementAlgorithm", meta = (AllowPrivateAccess = "true"))
	float DistanceStep;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MovementAlgorithm", meta = (AllowPrivateAccess = "true"))
	float DistanceStepFrames;

	FVector PreviousLocation;
	
	float DistanceToMove;

	float DistanceMoved = 0.f;

	bool bCanPick = true;
	///-----------------------------------------------------------------------------------------------------------------------------------------------------------

	///-----------------------------------Dance Mechanic----------------------------------------------------------------------------------------------------------
	float TimeInterval = 5.f;

	float CurrentTime = 0.f;
	float PreviousTime = 0.f;

	int32 PreviousRepetitions = 0;

	bool bApplyTimePenalty = false;

	float TimePenalty;
	float TimeBonus;

	bool bIsInPC = false;
	///-----------------------------------------------------------------------------------------------------------------------------------------------------------

protected:
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

	void BeginPlayFromGM();

	UFUNCTION(BlueprintCallable, Category = "Mechanics")
		void SetCurrentPickUpActor(float DistanceToActor, int32 NumReps);

	UFUNCTION(BlueprintCallable, Category = "Mechanics")
		void InitNavigationTileSerie(int32 NumReps);

	UFUNCTION(BlueprintCallable, Category = "Mechanics")
		void InitLabTileSerie(int32 NumReps);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void PostBeginPlay();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void OnFinishRepetitions();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void PlayCountDown();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void TimePenaltyEffect();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void TimeBonusEffect();

};
