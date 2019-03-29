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

#include "HynmersMovementComponent.h"

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

	// Init array with all the animations		Index
	Sequences.Add(WalkSequence);				//	0
	Sequences.Add(JumpSequence);				//	1
	Sequences.Add(SwimSequence);				//	2
	Sequences.Add(BoardSequence);				//	3
	Sequences.Add(BikeSequence);				//	4
	Sequences.Add(LegsMoveSequence);			//	5

	Montages.SetNum(Sequences.Num());

	BonesAngles.Add("thigh_L", 0.f);
	BonesAngles.Add("shin_L", 0.f);
	BonesAngles.Add("thigh_R", 0.f);
	BonesAngles.Add("shin_R", -0.f);

	BonesOffsets.Add("thigh_L", -168.f);
	BonesOffsets.Add("thigh_R", -168.f);
	BonesOffsets.Add("shin_L", -7.7f);
	BonesOffsets.Add("shin_R", -7.7f);

	for (int i = 0; i < Sequences.Num(); i++) {
		UAnimSequence* Sequence = Sequences[i];
		// Creating montages
		CreateMontage(Sequence, Montages[i]);
	}

	if (ensure(CheckAllNeededAssets()))
	{
		PostBeginPlay();
	}


}

void AHynmersPlayerController::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction & ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	if (!CheckAllNeededAssets())return;
	
	ActiveTile = ControlledPawn->GetActiveTile();

	UAnimSequence* ActiveSequence = Sequences[(int32)ActiveTile];
	UAnimMontage* ActiveMontage = Montages[(int32)ActiveTile];


	int32 ActiveFrameRange = AnimationSpecificFrameRange.Contains(ActiveTile) ? AnimationSpecificFrameRange[ActiveTile] : FrameRange;

	ActiveFrameRange = FMath::Clamp(ActiveFrameRange, 0, ActiveSequence->GetNumberOfFrames());
	
	TArray<int32> Frames = GetFrameRange(ActiveMontage, ActiveFrameRange);
	
	TArray<FName> BonesKeys;
	BonesAngles.GenerateKeyArray(BonesKeys);

	TArray<int32> BetterFrames = GetBetterFrames(ActiveSequence, BonesKeys, Frames);

	float BestTime;

	// Only when the best frames are {last, frist}
	if (BetterFrames[1] - BetterFrames[0] < 0)
	{
		TArray<float> Errors;
		for (int32 Frame : BetterFrames) {
			Errors.Add(GetFrameErrorInTime(ActiveSequence, ActiveSequence->GetTimeAtFrame(Frame), BonesKeys));
		}
		BestTime = ActiveSequence->GetTimeAtFrame((Errors[0] < Errors[1]) ? BetterFrames[0] : BetterFrames[1]);

	}
	else {
		// Turn frames to times
		TArray<float> BestTimes =
		{
			ActiveSequence->GetTimeAtFrame(BetterFrames[0]),
			ActiveSequence->GetTimeAtFrame(BetterFrames[1])
		};

		BestTime = BinarySearch(ActiveSequence, BestTimes, BonesKeys);
	}
	
	float CurrentTime =  PawnAnimationInstance->Montage_GetPosition(ActiveMontage);

	TriggerNotifies(ActiveSequence, CurrentTime, BestTime);
	
	SetMontagePosition(ActiveMontage, BestTime);

	/// Movement algorithm

	int32 CurrentFrame = ActiveSequence->GetFrameAtTime(CurrentTime);
	int32 NewFrame = ActiveSequence->GetFrameAtTime(BestTime);

	int32 DeltaFrame = NewFrame - CurrentFrame;
	int32 DeltaFrameConverted = ActiveSequence->GetNumberOfFrames() + NewFrame - CurrentFrame;
	int32 EvalFrameRate = (AnimationSpecificFrameRange.Contains(ActiveTile)) ? AnimationSpecificFrameRange[ActiveTile] : FrameRange;

	if (DeltaFrame < 0)
	{
		DeltaFrame = DeltaFrameConverted;
		
		if(DeltaFrame <= EvalFrameRate)
			NumberOfRepetitions++;
	}

	if (DeltaFrame != 0 && DeltaFrame <= EvalFrameRate) {
		DistanceToMove +=(DeltaFrame * DistanceStepFrames);
	}

	UE_LOG(LogTemp, Warning, TEXT("Reps %d"), NumberOfRepetitions);

	if (DistanceMoved <= DistanceToMove) {
		DistanceMoved += (ControlledPawn->GetActorLocation() - PreviousLocation).Size2D();
		PreviousLocation = ControlledPawn->GetActorLocation();
		ControlledPawn->MoveForward(1.f);
	}

	if (TagetRepetitions != 0 && NumberOfRepetitions >= TagetRepetitions)
	{
		bCanPick = false;
		TagetRepetitions = 0;
		OnFinishRepetitions();
		ControlledPawn->bCanMoveWithController = true;
	}

	return;
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
	float Angle = ConvertKinectAngle(rate);
	if (AnimationSpecificThighLOffset.Contains(ActiveTile)) {
		Angle += AnimationSpecificThighLOffset[ActiveTile];
	}
	BonesAngles.Add("thigh_L", Angle);
}

void AHynmersPlayerController::PosLeftKnee(float rate)
{
	BonesAngles.Add("shin_L", ConvertKinectAngle(rate));
}

void AHynmersPlayerController::PosRightThight(float rate)
{
	float Angle = ConvertKinectAngle(rate);
	if (AnimationSpecificThighROffset.Contains(ActiveTile)) {
		Angle += AnimationSpecificThighROffset[ActiveTile];
	}
	BonesAngles.Add("thigh_R", Angle);
}

void AHynmersPlayerController::PosRightKnee(float rate)
{
	BonesAngles.Add("shin_R", ConvertKinectAngle(rate));
}

void AHynmersPlayerController::SetCurrentPickUpActor(AActor * CurrentActor, int32 NumReps)
{
	if (!CurrentActor)return;

	if (!CheckAnimAssets()) return;

	ActivePickUpActor = CurrentActor;

	DistanceToPickUpActor = ControlledPawn->GetDistanceTo(CurrentActor);

	DistanceStep = DistanceToPickUpActor / ((float)NumReps+1);
	TagetRepetitions = NumReps;

	DistanceStepFrames = DistanceStep / (float)Sequences[(int32)ActiveTile]->GetNumberOfFrames();

	DistanceToMove = DistanceStepFrames;

	PreviousLocation = ControlledPawn->GetActorLocation();

	DistanceMoved = 0.f;

	ControlledPawn->bCanMoveWithController = false;

	NumberOfRepetitions = 0;

	bCanPick = true;
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
		AnimSequence->GetBoneTransform(FrameTransform, ControlledPawn->GetMesh()->GetBoneIndex(BoneNames[BoneIndex]), Time, true);
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

TArray<int32> AHynmersPlayerController::GetFrameRange(UAnimMontage * ActiveMontage, int32 FrameRange)
{
	float CurrentTime = PawnAnimationInstance->Montage_GetPosition(ActiveMontage);
	int32 CurrentFrame = ActiveMontage->GetFrameAtTime(CurrentTime);

	TArray<int32> Frames = { CurrentFrame + OffsetFrames, CurrentFrame + FrameRange -1 + OffsetFrames };
	return Frames;
}

float AHynmersPlayerController::GetFrameErrorInTime(UAnimSequence* ActiveSequence, const float& FrameTime, TArray<FName> &BonesKeys)
{
	TArray<float> AnimValues = GetAnimValues(ActiveSequence, FrameTime, BonesKeys);
	return GetError(BonesAngles, AnimValues);
}

TArray<int32> AHynmersPlayerController::GetBetterFrames(UAnimSequence * ActiveSequence, TArray<FName>& BonesKeys, const TArray<int32> &Frames)
{
	TArray<float> FramesError;
	for (int32 CurrentFrame = Frames[0]; CurrentFrame <= Frames[1]; CurrentFrame++) {
		FramesError.Add(GetFrameErrorInTime(ActiveSequence, ActiveSequence->GetTimeAtFrame(ConvertFrame(ActiveSequence->GetNumberOfFrames(), CurrentFrame)), BonesKeys));
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

	return { ConvertFrame(ActiveSequence->GetNumberOfFrames(), Frames[0] + BetterIndex), ConvertFrame(ActiveSequence->GetNumberOfFrames(), Frames[0] + BetterIndex + 1) };
}

float AHynmersPlayerController::BinarySearch(UAnimSequence* ActiveSequence, const TArray<float> &BetterTimes, TArray<FName> &BonesKeys, int32 Depth)
{
	TArray<float> NewTimes = { BetterTimes[0], (BetterTimes[0] + BetterTimes[1]) / 2, BetterTimes[1] };

	TArray<float> TimeErrors;
	for (float CurrentTime : NewTimes) {
		TimeErrors.Add(GetFrameErrorInTime(ActiveSequence, CurrentTime, BonesKeys));
	}

	int BetterIndex = 0;
	float BetterError = INFINITY;
	for (int i = 0; i < BetterTimes.Num() - 1; i++) {
		float CurrentError = TimeErrors[i] + TimeErrors[i + 1];
		if (CurrentError < BetterError) {
			BetterIndex = i;
			BetterError = CurrentError;
		}
	}

	if (Depth < BinarySearchDepth)
	{
		return BinarySearch(ActiveSequence, { NewTimes[BetterIndex],NewTimes[BetterIndex +1] }, BonesKeys, ++Depth);
	}

	int32 BestTimeIndex;
	FMath::Min(TimeErrors, &BestTimeIndex);

	return NewTimes[BestTimeIndex];
}

void AHynmersPlayerController::TriggerNotifies(UAnimSequence * ActiveSequence, float CurrentTime, float BestTime)
{
	TArray<FAnimNotifyEventReference> Notifies;
	ActiveSequence->GetAnimNotifiesFromDeltaPositions(CurrentTime, BestTime, Notifies);

	for (FAnimNotifyEventReference NotifyToSend : Notifies) {
		PawnAnimationInstance->TriggerSingleAnimNotify(NotifyToSend.GetNotify());
	}
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
