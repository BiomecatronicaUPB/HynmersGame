// Fill out your copyright notice in the Description page of Project Settings.

#include "HynmersPlayerController.h"
#include "Animation/AnimSequence.h"
#include "HynmersCharacter.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InputComponent.h"
#include "Animation/AnimCompositeBase.h"
#include "Kismet/KismetMathLibrary.h"

void AHynmersPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ControlledPawn = Cast<AHynmersCharacter>(GetPawn());
	if (!ensure(ControlledPawn))return;

	PawnAnimationInstance = ControlledPawn->GetMesh()->GetAnimInstance();

	if (!CheckAnimAssets()) {
		UE_LOG(LogTemp, Error, TEXT("Some sequences is missing"));
		return;
	}

	// Init array with all the montages		Index
	Sequences.Add(WalkSequence);			//	0
	Sequences.Add(JumpSequence);			//	1
	Sequences.Add(SwimSequence);			//	2
	Sequences.Add(BoardSequence);			//	3
	Sequences.Add(BikeSequence);			//	4
	Sequences.Add(LegsMoveSequence);		//	5

	// Map Enum number
	// Bridge		0
	// Rooms		1
	// Labs			2
	// Com			3
	// Machine		4
	// Nav			5
	MapMontage.Add(0, 0); // Bridge		Walk
	MapMontage.Add(1, 3); // Rooms		Board
	MapMontage.Add(2, 1); // Labs		Jump
	MapMontage.Add(3, 4); // Com		Bike
	MapMontage.Add(4, 2); // Machine	Swim
	MapMontage.Add(5, 5); // Nav		Legs

	Montages.SetNum(Sequences.Num());

	BonesAngles.Add("thigh_L", 0.f);
	BonesAngles.Add("shin_L", 0.f);
	BonesAngles.Add("thigh_R", 0.f);
	BonesAngles.Add("shin_R", 0.f);

	BonesOffsets.Add("thigh_L", -168.f);
	BonesOffsets.Add("thigh_R", -168.f);
	BonesOffsets.Add("shin_L", -7.7f);
	BonesOffsets.Add("shin_R", -7.7f);

	for (int i = 0; i < Sequences.Num(); i++) {
		UAnimSequence* Sequence = Sequences[i];
		// Creating montages
		CreateMontage(Sequence, Montages[i]);
	}

	if (CheckAllNeededAssets()) {
		TArray<FName> BonesKeys;
		BonesAngles.GenerateKeyArray(BonesKeys);

		for (int i = 0; i < WalkSequence->GetNumberOfFrames(); i++)
		{
			for (int j = 0; j < BonesKeys.Num(); j++) {
				FTransform FrameTransform;
				WalkSequence->GetBoneTransform(FrameTransform, ControlledPawn->GetMesh()->GetBoneIndex(BonesKeys[j]), WalkSequence->GetTimeAtFrame(i), true);
				UE_LOG(LogTemp, Warning, TEXT("Animation Frame %d %s: %f"), i, *BonesKeys[j].ToString(), (FrameTransform.GetRotation().Rotator().Yaw - BonesOffsets[BonesKeys[j]]));
			}
		}

	}
}

void AHynmersPlayerController::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction & ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	if (!ensure(CheckAllNeededAssets() && CheckMontages()))return;
	
	for (auto Pair : BonesAngles)
	{
		UE_LOG(LogTemp, Warning, TEXT("Kinect %s: %f"), *Pair.Key.ToString(), Pair.Value);
	}


}

void AHynmersPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	check(InputComponent);

	InputComponent->BindAxis("PosLeftThight", this, &AHynmersPlayerController::PosLeftThight);
	InputComponent->BindAxis("PosLeftKnee", this, &AHynmersPlayerController::PosLeftKnee);
	InputComponent->BindAxis("PosRightThight", this, &AHynmersPlayerController::PosRightThight);
	InputComponent->BindAxis("PosRightKnee", this, &AHynmersPlayerController::PosRightKnee);

}

void AHynmersPlayerController::PosLeftThight(float rate)
{
	BonesAngles.Add("thigh_L", ConvertKinectAngle(rate));
}

void AHynmersPlayerController::PosLeftKnee(float rate)
{
	BonesAngles.Add("shin_L", ConvertKinectAngle(rate));
}

void AHynmersPlayerController::PosRightThight(float rate)
{
	BonesAngles.Add("thigh_R", ConvertKinectAngle(rate));
}

void AHynmersPlayerController::PosRightKnee(float rate)
{
	BonesAngles.Add("shin_R", ConvertKinectAngle(rate));
}

float AHynmersPlayerController::ConvertKinectAngle(float Rate)
{
	float AngleOffset = 180;
	return  ((Rate > 0) ? Rate * 180.f / PI - AngleOffset : Rate * 180.f / PI + AngleOffset) * -1;
}

bool AHynmersPlayerController::CheckAnimAssets()
{
	return WalkSequence && JumpSequence && SwimSequence && BoardSequence && BikeSequence && LegsMoveSequence;
}

bool AHynmersPlayerController::CheckAllNeededAssets()
{
	return CheckAnimAssets() && PawnAnimationInstance;
}

bool AHynmersPlayerController::CheckMontages()
{
	for (UAnimMontage* Montage : Montages) {
		if (Montage == nullptr) {
			UE_LOG(LogTemp, Error, TEXT("Montage not created"));
			return false;
		}
	}
	return true;
}

void AHynmersPlayerController::CreateMontage(UAnimSequence* Sequence, UAnimMontage* &OutMontage)
{
	if (CheckAllNeededAssets()) {
		OutMontage = UAnimMontage::CreateSlotAnimationAsDynamicMontage(Sequence, FName("FullBody"));
	}
}
