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

	if (!CheckMonatges()) {
		UE_LOG(LogTemp, Error, TEXT("Some montage is missing"));
		return;
	}

	// Init array with all the montages		Index
	Montages.Add(WalkMontage);			//	0
	Montages.Add(JumpMontage);			//	1
	Montages.Add(SwimMontage);			//	2
	Montages.Add(BoardMontage);			//	3
	Montages.Add(BikeMontage);			//	4
	Montages.Add(LegsMoveMontage);		//	5


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


	for (UAnimMontage* Montage : Montages) {

		NumBetterFrames.Add(UKismetMathLibrary::Clamp(NumBetterFramesDefault, 0, Montage->GetNumberOfFrames()));
		PastAnimTime.Add(SelectSkeletonPose({0, Montage->GetNumberOfFrames()}));
	}

	BonesAngles.Add("thigh_l", 0.f);
	BonesAngles.Add("calf_l", 0.f);
	BonesAngles.Add("thigh_r", 0.f);
	BonesAngles.Add("calf_r", 0.f);

}

void AHynmersPlayerController::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction & ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	if (CheckAllNeededAssets()) {

		int32 ActiveTile = ControlledPawn->GetCurrentTile();

		TArray<UAnimMontage*> ActiveMontages = { Montages[0], Montages[MapMontage[ActiveTile]] };
		UAnimMontage* CurrentMontage = (ControlledPawn->bInteractWith) ? ActiveMontages[0] : ActiveMontages[1];
		int32 ActiveMontageID = (ControlledPawn->bInteractWith) ? 0 : MapMontage[ActiveTile];

		float CurrentPosition = PawnAnimationInstance->Montage_GetPosition(CurrentMontage);
		int MinRangeValue = UKismetMathLibrary::Clamp(CurrentMontage->GetFrameAtTime(CurrentPosition) - NumSearchFrames / 2, 0, CurrentMontage->GetNumberOfFrames());
		int MaxRangeValue = UKismetMathLibrary::Clamp(CurrentMontage->GetFrameAtTime(CurrentPosition) + NumSearchFrames / 2, 0, CurrentMontage->GetNumberOfFrames());

		NumBetterFrames[ActiveMontageID] = UKismetMathLibrary::Clamp(NumBetterFrames[ActiveMontageID], 0, NumSearchFrames);
		//float CurrentTime = SelectSkeletonPose(TArray<int>({MinRangeValue, MaxRangeValue},2));

		//	if (!bInitAccel && CurrentTime - PastAnimTime != 0) {
		//		bInitAccel = true;
		//	}

		//	if (bInitAccel) {
		//		ControlledPawn->MoveForward(1.f);
		//		PastAnimTime = CurrentTime;
		//		if (++AccelTimes >= MaxAccelTimes) {
		//			bInitAccel = false;
		//			AccelTimes = 0;
		//		}
		//	}

	}
}

float AHynmersPlayerController::SelectSkeletonPose(TArray<int> FrameSearchRange)
{
	//float Error;
	//float ResultTime = FindPoseInMontage(BonesAngles, WalkSequence, FrameSearchRange, &Error);

	////UE_LOG(LogTemp, Warning, TEXT("Time found = %f Err Value: %f"), ResultTime, Error);

	//// Sequence reproduction control
	//PawnAnimationInstance->Montage_Play(WalkMontage);
	//PawnAnimationInstance->Montage_SetPosition(WalkMontage, ResultTime);
	//PawnAnimationInstance->Montage_Pause(WalkMontage);

	//return ResultTime;
	return 0;
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
	BonesAngles.Add("thigh_l", rate * 180.f / PI);
}

void AHynmersPlayerController::PosLeftKnee(float rate)
{
	BonesAngles.Add("calf_l", rate * 180.f / PI);
}

void AHynmersPlayerController::PosRightThight(float rate)
{
	BonesAngles.Add("thigh_r", rate * 180.f / PI);
}

void AHynmersPlayerController::PosRightKnee(float rate)
{
	BonesAngles.Add("calf_r", rate * 180.f / PI);
}

float AHynmersPlayerController::FindPoseInMontage(TMap<FName,float> BonesRotation, UAnimSequence* Sequence, TArray<int> FrameSearchRange, float* Error)
{
	//if(!BonesRotation.Num() || !Sequence)
	//	return 0.0f;

	//const int32 NumBones = BonesRotation.Num();
	//const int32 NumFrames = FrameSearchRange[1] - FrameSearchRange[0];

	//TArray<float> FramesFound;
	//FramesFound.SetNum(NumFrames);

	//TArray<int>BetterFrames;

	//for (int frame = FrameSearchRange[0]; frame < FrameSearchRange[1]; frame++) {
	//	FramesFound[frame - FrameSearchRange[0]] = CalculateTimeValue(BonesRotation, Sequence, Sequence->GetTimeAtFrame(frame));
	//}
	//BetterFrames = FindMinsInArray(FramesFound, NumBetterFrames);

	//TArray<float> SelectedTimes;
	//SelectedTimes.SetNum(NumBetterFrames);

	//TArray<float> SelectedTimesError;
	//SelectedTimesError.SetNum(NumBetterFrames);

	//for (int i = 0; i < NumBetterFrames; i++) {
	//	float MinFrame = UKismetMathLibrary::Clamp(BetterFrames[i] + FrameSearchRange[0] - 1, 0, Sequence->GetNumberOfFrames());
	//	float MaxFrame = UKismetMathLibrary::Clamp(BetterFrames[i] + FrameSearchRange[0] + 1, 0, Sequence->GetNumberOfFrames());
	//	SelectedTimes[i] = FindTimeInRange(BonesRotation, TArray<float>({ Sequence->GetTimeAtFrame(MinFrame),Sequence->GetTimeAtFrame(MaxFrame) }, 2), Sequence, 0, &SelectedTimesError[i]);
	//}
	//int32 BestTimeIndex;
	//float BestError;
	//UKismetMathLibrary::MinOfFloatArray(SelectedTimesError, BestTimeIndex, BestError);

	//if (Error)*Error = BestError;

	//return SelectedTimes[BestTimeIndex];
	return 0;
}

FQuat AHynmersPlayerController::GetRefPoseQuat(TMap<FName,float> &BonesRotation, int bone)
{
	TArray<FName> BonesKeys;
	BonesRotation.GetKeys(BonesKeys);

	// SkeletalMesh reference pose
	FTransformArrayA2 TransformArray = ControlledPawn->GetMesh()->SkeletalMesh->Skeleton->GetReferenceSkeleton().GetRefBonePose();
	int32 index = ControlledPawn->GetMesh()->GetBoneIndex(BonesKeys[bone]);
	FTransform Transform = (index == INDEX_NONE) ? FTransform::Identity : TransformArray[index];
	return Transform.GetRotation();
}

bool AHynmersPlayerController::CheckMonatges()
{
	return WalkMontage && JumpMontage && SwimMontage && BoardMontage && BikeMontage && LegsMoveMontage;
}

bool AHynmersPlayerController::CheckAllNeededAssets()
{
	return CheckMonatges() && PawnAnimationInstance;
}

TArray<int> AHynmersPlayerController::FindMinsInArray(TArray<float> Array, int32 Num)
{
	TArray<int> IndexArray;
	IndexArray.SetNum(Num);

	for (int i = 0; i < Num; i++) {
		float MinValue;
		UKismetMathLibrary::MinOfFloatArray(Array, IndexArray[i], MinValue);
		Array[IndexArray[i]] = 99.f;
	}

	return IndexArray;
}

float AHynmersPlayerController::FindTimeInRange(TMap<FName,float> BonesRotation, TArray<float> Range, UAnimSequence * Sequence, int32 CurrentDepth, float* Error)
{
	float TimeRange[] = { Range[0], (Range[1] - Range[0]) / 2.f + Range[0], Range[1] };
	float Values[] = { 0.f, 0.f, 0.f };

	for (int time = 0; time < 3; time ++) {

		Values[time] = CalculateTimeValue(BonesRotation, Sequence, TimeRange[time]);
	}
	TArray<int> BetterIndexes = FindMinsInArray(TArray<float>(Values, 3), 2);
	Range[0] = FMath::Min(TimeRange[BetterIndexes[0]], TimeRange[BetterIndexes[1]]);
	Range[1] = FMath::Max(TimeRange[BetterIndexes[0]], TimeRange[BetterIndexes[1]]);

	if (Error)*Error = Values[BetterIndexes[0]];

	if (CurrentDepth >= BinarySearchDepth)return TimeRange[BetterIndexes[0]];

	return FindTimeInRange(BonesRotation, Range, Sequence, CurrentDepth + 1, Error);
}

float AHynmersPlayerController::CalculateTimeValue(TMap<FName, float> &BonesRotation, UAnimSequence * Sequence, float  Time)
{
	float Value = 0.f;

	TArray<FName> BonesKeys;
	BonesRotation.GetKeys(BonesKeys);
	int32 NumBones = BonesRotation.Num();

	for (int bone = 0; bone < NumBones; bone++) {
		FQuat RefPoseRotation = GetRefPoseQuat(BonesRotation, bone);
		FTransform FrameTransform;
		Sequence->GetBoneTransform(FrameTransform, ControlledPawn->GetMesh()->GetBoneIndex(BonesKeys[bone]), Time, true);
		FQuat FrameRotation = FrameTransform.GetRotation();
		FRotator FrameRotator = (RefPoseRotation.Inverse()*FrameRotation).Rotator();
		Value += ((BonesKeys[bone].ToString().Contains("thigh")) ? ThighWeight : KneeWeight)*UKismetMathLibrary::Abs(FrameRotator.Yaw - BonesRotation[BonesKeys[bone]]);
	}
	Value /= (float)NumBones;

	return Value;
}
