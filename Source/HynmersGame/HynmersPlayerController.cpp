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

DEFINE_LOG_CATEGORY_STATIC(LogHynmersPlayer, Warning, All);

AHynmersPlayerController::AHynmersPlayerController()
{
	// Assign Knee Weight as complementary value of the thigh weight
	KneeWeight = 1.f - ThighWeight;
}

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

	BonesAngles.Add("thigh_L", 30.f);
	BonesAngles.Add("shin_L", -61.f);
	BonesAngles.Add("thigh_R", 30.f);
	BonesAngles.Add("shin_R", -61.3f);

	BonesOffsets.Add("thigh_L", -168.f);
	BonesOffsets.Add("thigh_R", -168.f);
	BonesOffsets.Add("shin_L", -7.7f);
	BonesOffsets.Add("shin_R", -7.7f);

	for (int i = 0; i < Sequences.Num(); i++) {
		UAnimSequence* Sequence = Sequences[i];
		// Creating montages
		CreateMontage(Sequence, Montages[i]);
	}

	if (ensure(CheckAllNeededAssets())) {
		TArray<FName> BonesKeys;
		BonesAngles.GenerateKeyArray(BonesKeys);

		for (int i = 0; i < WalkSequence->GetNumberOfFrames(); i++)
		{
			TArray<float> AnimValues = GetAnimValues(WalkSequence, WalkSequence->GetTimeAtFrame(i), BonesKeys);
			for (int j = 0; j < BonesKeys.Num(); j++) {
				UE_LOG(LogTemp, Warning, TEXT("Animation Frame %d %s: %f"), i, *BonesKeys[j].ToString(), AnimValues[j]);
			}

		}

	}
}

void AHynmersPlayerController::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction & ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	if (!CheckAllNeededAssets())return;

	UAnimSequence* ActiveSequence = WalkSequence;
	UAnimMontage* ActiveMontage = Montages[0];

	// Determinar un rango de frames 
	int32 FrameRange = 6;

	// Obtener los dos mejores frames seguidos
	SetMontagePosition(ActiveMontage, 1);
	float CurrentTime = PawnAnimationInstance->Montage_GetPosition(ActiveMontage);
	int32 CurrentFrame = ActiveSequence->GetFrameAtTime(CurrentTime);

	TArray<int32> Frames = {CurrentFrame - FrameRange/2 + (FrameRange + 1) % 2, CurrentFrame + FrameRange / 2 };
	
	TArray<FName> BonesKeys;
	BonesAngles.GenerateKeyArray(BonesKeys);

	TArray<float> FramesError;
	for (int32 TestFrame = Frames[0]; TestFrame <= Frames[1]; TestFrame++) {
		TArray<float> AnimValues = GetAnimValues(ActiveSequence, ActiveSequence->GetTimeAtFrame(ConvertFrame(ActiveSequence->GetNumberOfFrames(), TestFrame)), BonesKeys);
		FramesError.Add(GetError(BonesAngles,AnimValues));
		UE_LOG(LogTemp, Warning, TEXT("Frame: %d, Difference: %f"), TestFrame, FramesError.Last());
	}

	int BetterIndex = 0;
	float BetterError = INFINITY;
	for (int i = 0; i < FramesError.Num() - 1; i++) {
		float CurrentError = FramesError[i] + FramesError[i + 1];
		if (CurrentError < BetterError) {
			BetterIndex = i;
			BetterError = CurrentError;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("BetterFrame %d"), ConvertFrame(ActiveSequence->GetNumberOfFrames(), Frames[0] + BetterIndex));

	// Convertir frames a tiempo
	// Realizar busqeuda binaria en la mitad de los dos mejores frames
		// Si ambos frames son primero y ultimo, 


	//TArray<float> AuxValues;
	//BonesAngles.GenerateValueArray(AuxValues);

	//AuxValues[2] += 10;
	//AuxValues[3] += 5;

	//float Error = GetError(BonesAngles, AuxValues);

	//UE_LOG(LogTemp, Warning, TEXT("Difference: %f"), Error);
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
	//BonesAngles.Add("thigh_L", ConvertKinectAngle(rate));
}

void AHynmersPlayerController::PosLeftKnee(float rate)
{
	//BonesAngles.Add("shin_L", ConvertKinectAngle(rate));
}

void AHynmersPlayerController::PosRightThight(float rate)
{
	//BonesAngles.Add("thigh_R", ConvertKinectAngle(rate));
}

void AHynmersPlayerController::PosRightKnee(float rate)
{
	//BonesAngles.Add("shin_R", ConvertKinectAngle(rate));
}

float AHynmersPlayerController::ConvertKinectAngle(float Rate)
{
	float AngleOffset = 180;
	return  ((Rate > 0) ? Rate * 180.f / PI - AngleOffset : Rate * 180.f / PI + AngleOffset) * -1;
}

float AHynmersPlayerController::GetError(TMap<FName, float> &BonesValues, TArray<float> &AnimValues)
{
	float Error = 0.f;

	TArray<FName> Keys;
	BonesValues.GenerateKeyArray(Keys);

	for (int BoneIndex = 0; BoneIndex < Keys.Num(); BoneIndex++) {
		Error += FMath::Abs( ((Keys[BoneIndex].ToString().Contains("thigh"))? ThighWeight:KneeWeight)*(BonesValues[Keys[BoneIndex]] - AnimValues[BoneIndex]) );
	}
	return Error;
}

TArray<float> AHynmersPlayerController::GetAnimValues(UAnimSequence* AnimSequence, float Time, TArray<FName> BoneNames)
{
	if (!AnimSequence)
	{
		UE_LOG(LogHynmersPlayer, Error, TEXT("Passed an null animations sequence"))
		return TArray<float>();
	}
	// Yaw angle is the one in the sagittal plane
	TArray<float> AnimValues;
	for (int BoneIndex = 0;  BoneIndex < BoneNames.Num(); BoneIndex++) {
		FTransform FrameTransform;
		WalkSequence->GetBoneTransform(FrameTransform, ControlledPawn->GetMesh()->GetBoneIndex(BoneNames[BoneIndex]), Time, true);
		AnimValues.Add(FrameTransform.GetRotation().Rotator().Yaw - BonesOffsets[BoneNames[BoneIndex]]);
	}
	return AnimValues;
}

void AHynmersPlayerController::SetMontagePosition(UAnimMontage * Montage, float Time)
{
	if (!PawnAnimationInstance)return;

	if (PawnAnimationInstance->GetCurrentActiveMontage() != Montage)
	{
		PawnAnimationInstance->Montage_Stop(0,PawnAnimationInstance->GetCurrentActiveMontage());
	}

	if (Time < 0 || Time > Montage->GetTimeAtFrame(Montage->GetNumberOfFrames() - 1)) {
		UE_LOG(LogHynmersPlayer, Warning, TEXT("Out of frame bounds of active montage. Clamped to a valid frame") )
	}

	PawnAnimationInstance->Montage_Play(Montage);
	PawnAnimationInstance->Montage_SetPosition(Montage, Time);
	PawnAnimationInstance->Montage_Pause(Montage);
}

int32 AHynmersPlayerController::ConvertFrame(int32 NumberOfFrames, int32 FrameToCorrect)
{
	FrameToCorrect = (FrameToCorrect + NumberOfFrames) % NumberOfFrames;
	return FrameToCorrect;
}

bool AHynmersPlayerController::CheckAnimAssets()
{
	return WalkSequence && JumpSequence && SwimSequence && BoardSequence && BikeSequence && LegsMoveSequence;
}

bool AHynmersPlayerController::CheckAllNeededAssets()
{
	return CheckAnimAssets() && PawnAnimationInstance && CheckMontages();
}

bool AHynmersPlayerController::CheckMontages()
{
	for (UAnimMontage* Montage : Montages) {
		if (Montage == nullptr) {
			UE_LOG(LogHynmersPlayer, Error, TEXT("Montage not created"));
			return false;
		}
	}
	return true;
}

void AHynmersPlayerController::CreateMontage(UAnimSequence* Sequence, UAnimMontage* &OutMontage)
{
	if (CheckAnimAssets()) {
		OutMontage = UAnimMontage::CreateSlotAnimationAsDynamicMontage(Sequence, FName("FullBody"));
	}
}
