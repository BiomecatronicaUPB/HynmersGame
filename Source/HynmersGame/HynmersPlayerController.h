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

	// Animation objects
	class AHynmersCharacter* ControlledPawn = nullptr;
	class UAnimInstance* PawnAnimationInstance = nullptr;

	// Variable needed for animation of sekeltal mesh
	TArray<UAnimSequence*> Sequences;
	TArray<UAnimMontage*> Montages;
	TMap<int32, int32> MapMontage;
	TMap<FName,float> BonesAngles;
	TMap<FName, float> BonesOffsets;

	float ConvertKinectAngle(float Rate);

protected:
	virtual void BeginPlay() override;

	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;

	virtual void SetupInputComponent() override;

	bool CheckAnimAssets();
	bool CheckAllNeededAssets();
	bool CheckMontages();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void CreateMontage(UAnimSequence* Sequence, UAnimMontage* &OutMontage);


public:

	// Kinect Input Events
	void PosLeftThight(float rate);
	void PosLeftKnee(float rate);
	void PosRightThight(float rate);
	void PosRightKnee(float rate);
};
