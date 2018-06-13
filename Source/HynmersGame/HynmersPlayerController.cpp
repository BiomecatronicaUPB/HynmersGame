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
	if (!WalkSequence) {
		UE_LOG(LogTemp, Error, TEXT("Select a valid animation sequence"));
		return;
	}

	CreateMontage();

	if (!WalkMontage) {
		UE_LOG(LogTemp, Error, TEXT("Montage could not be created"));
		return;
	}


	BonesAngles.Add("thigh_l", 0.f);
	BonesAngles.Add("calf_l", 0.f);
	BonesAngles.Add("thigh_r", 0.f);
	BonesAngles.Add("calf_r", 0.f);
	NumBetterFrames = UKismetMathLibrary::Clamp(NumBetterFrames,0, WalkSequence->GetNumberOfFrames());
	PastAnimTime = SelectSkeletonPose(TArray<int>({ 0, WalkSequence->GetNumberOfFrames() }, 2));

}

void AHynmersPlayerController::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction & ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	if (PawnAnimationInstance && WalkSequence && WalkMontage) {

		float CurrentPosition = PawnAnimationInstance->Montage_GetPosition(WalkMontage);
		int MinRangeValue = UKismetMathLibrary::Clamp(WalkSequence->GetFrameAtTime(CurrentPosition) - NumSearchFrames / 2, 0, WalkSequence->GetNumberOfFrames());
		int MaxRangeValue = UKismetMathLibrary::Clamp(WalkSequence->GetFrameAtTime(CurrentPosition) + NumSearchFrames / 2, 0, WalkSequence->GetNumberOfFrames());

		NumBetterFrames = UKismetMathLibrary::Clamp(NumBetterFrames, 0, NumSearchFrames);
		float CurrentTime = SelectSkeletonPose(TArray<int>({MinRangeValue, MaxRangeValue},2));

		if (!bInitAccel && CurrentTime - PastAnimTime != 0) {
			bInitAccel = true;
		}

		if (bInitAccel) {
			ControlledPawn->MoveForward(1.f);
			PastAnimTime = CurrentTime;
			if (++AccelTimes >= MaxAccelTimes) {
				bInitAccel = false;
				AccelTimes = 0;
			}
		}
	}
}

float AHynmersPlayerController::SelectSkeletonPose(TArray<int> FrameSearchRange)
{
	float Error;
	float ResultTime = FindPoseInMontage(BonesAngles, WalkSequence, FrameSearchRange, &Error);

	UE_LOG(LogTemp, Warning, TEXT("Time found = %f Err Value: %f"), ResultTime, Error);

	// Sequence reproduction control
	PawnAnimationInstance->Montage_Play(WalkMontage);
	PawnAnimationInstance->Montage_SetPosition(WalkMontage, ResultTime);
	PawnAnimationInstance->Montage_Pause(WalkMontage);

	return ResultTime;
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


void AHynmersPlayerController::CreateMontage()
{
	if (PawnAnimationInstance && WalkSequence && !WalkMontage) {
		WalkMontage = UAnimMontage::CreateSlotAnimationAsDynamicMontage(WalkSequence, FName("FullBody"));
	}
}

float AHynmersPlayerController::FindPoseInMontage(TMap<FName,float> BonesRotation, UAnimSequence* Sequence, TArray<int> FrameSearchRange, float* Error)
{
	if(!BonesRotation.Num() || !Sequence)
		return 0.0f;

	const int32 NumBones = BonesRotation.Num();
	const int32 NumFrames = FrameSearchRange[1] - FrameSearchRange[0];

	TArray<float> FramesFound;
	FramesFound.SetNum(NumFrames);

	TArray<int>BetterFrames;

	for (int frame = FrameSearchRange[0]; frame < FrameSearchRange[1]; frame++) {
		FramesFound[frame - FrameSearchRange[0]] = CalculateTimeValue(BonesRotation, Sequence, Sequence->GetTimeAtFrame(frame));
	}
	BetterFrames = FindMinsInArray(FramesFound, NumBetterFrames);

	TArray<float> SelectedTimes;
	SelectedTimes.SetNum(NumBetterFrames);

	TArray<float> SelectedTimesError;
	SelectedTimesError.SetNum(NumBetterFrames);

	for (int i = 0; i < NumBetterFrames; i++) {
		float MinFrame = UKismetMathLibrary::Clamp(BetterFrames[i] + FrameSearchRange[0] - 1, 0, Sequence->GetNumberOfFrames());
		float MaxFrame = UKismetMathLibrary::Clamp(BetterFrames[i] + FrameSearchRange[0] + 1, 0, Sequence->GetNumberOfFrames());
		SelectedTimes[i] = FindTimeInRange(BonesRotation, TArray<float>({ Sequence->GetTimeAtFrame(MinFrame),Sequence->GetTimeAtFrame(MaxFrame) }, 2), Sequence, 0, &SelectedTimesError[i]);
	}
	int32 BestTimeIndex;
	float BestError;
	UKismetMathLibrary::MinOfFloatArray(SelectedTimesError, BestTimeIndex, BestError);

	if (Error)*Error = BestError;

	return SelectedTimes[BestTimeIndex];
}

FQuat AHynmersPlayerController::GetRefPoseQuat(TMap<FName,float> &BonesRotation, int bone)
{
	TArray<FName> BonesKeys;
	BonesRotation.GetKeys(BonesKeys);

	// SkeletalMesh reference pose
	FTransformArrayA2 TransformArray = ControlledPawn->GetMesh()->SkeletalMesh->Skeleton->GetReferenceSkeleton().GetRefBonePose();
	FTransform Transform = TransformArray[ControlledPawn->GetMesh()->GetBoneIndex(BonesKeys[bone])];
	return Transform.GetRotation();
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
