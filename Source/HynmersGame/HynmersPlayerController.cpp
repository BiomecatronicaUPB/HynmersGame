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

}

void AHynmersPlayerController::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction & ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	TArray<TPair<FName, float>> send;
	send.Add(TPair<FName, float>(FName("thigh_l"), -30.f));
	send.Add(TPair<FName, float>(FName("calf_l"), 0.f));
	send.Add(TPair<FName, float>(FName("thigh_r"), -18.f));
	send.Add(TPair<FName, float>(FName("calf_r"), 0.f));
	float Error;
	float ResultTime = FindPoseInMontage(send, WalkSequence, &Error);

	UE_LOG(LogTemp, Warning, TEXT("Time found = %f Error: %f"), ResultTime, Error);

	if (PawnAnimationInstance && WalkSequence && WalkMontage) {
		PawnAnimationInstance->Montage_SetPosition(WalkMontage,ResultTime);
		PawnAnimationInstance->Montage_Play(WalkMontage);
		PawnAnimationInstance->Montage_Pause(WalkMontage);
	}
}

void AHynmersPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	check(InputComponent);
}

void AHynmersPlayerController::CreateMontage()
{
	if (PawnAnimationInstance && WalkSequence) {
		WalkMontage = UAnimMontage::CreateSlotAnimationAsDynamicMontage(WalkSequence, FName("LeftLeg"));
	}
}

float AHynmersPlayerController::FindPoseInMontage(TArray<TPair<FName,float>> BonesRotation, UAnimSequence* Sequence, float* Error)
{
	if(!BonesRotation.Num() || !Sequence)
		return 0.0f;

	const int32 NumBones = BonesRotation.Num();
	const int32 NumFrames = Sequence->GetNumberOfFrames();

	// List of joint value in each frame for each bone
	TArray<TArray<float>> FramesFound;
	FramesFound.SetNum(NumBones);

	// List of better frames for each bone
	TArray<TArray<int>>BetterFrames;
	BetterFrames.SetNum(NumBones);
	
	for (int bone = 0; bone < NumBones; bone++) {

		FQuat RefPoseRotation = GetRefPoseQuat(BonesRotation, bone);
		FramesFound[bone].Init(0.f, NumFrames);

		for (int frame = 0; frame < NumFrames; frame++) {
			// Sequence Pose
			FTransform FrameTransform;
			Sequence->GetBoneTransform(FrameTransform, ControlledPawn->GetMesh()->GetBoneIndex(BonesRotation[bone].Key), Sequence->GetTimeAtFrame(frame), true);
			FQuat FrameRotation = FrameTransform.GetRotation();

			// Calculating Yaw angle
			FRotator FrameRotator = (RefPoseRotation.Inverse()*FrameRotation).Rotator();

			FramesFound[bone][frame] = UKismetMathLibrary::Abs(FrameRotator.Yaw - BonesRotation[bone].Value);
			//UE_LOG(LogTemp, Warning, TEXT("Difference: %f frame: %d"), FramesFound[bone][frame], frame)
			UE_LOG(LogTemp, Warning, TEXT("Rotation: %s frame: %d bone: %s"), *FrameRotator.ToString(), frame, *BonesRotation[bone].Key.ToString())
		}
		BetterFrames[bone] = FindMinsInArray(FramesFound[bone], NumBetterFrames);
		for (int k = 0; k < NumBetterFrames; k++) {
			UE_LOG(LogTemp, Warning, TEXT("Better frame: %d"), BetterFrames[bone][k])
		}
	}

	TArray<int> FrameRange = FindFrameRange(BetterFrames, NumBones, NumFrames);
	TArray<float> TimeRange({ Sequence->GetTimeAtFrame(FrameRange[0]) ,Sequence->GetTimeAtFrame(FrameRange[1]) }, 2);
	UE_LOG(LogTemp, Warning, TEXT("Range: [%d %d]"), FrameRange[0], FrameRange[1]);
	

	return FindTimeInRange(BonesRotation, TimeRange, Sequence, 0, Error);
}

FQuat AHynmersPlayerController::GetRefPoseQuat(TArray<TPair<FName, float>> &BonesRotation, int bone)
{
	// SkeletalMesh reference pose
	FTransformArrayA2 TransformArray = ControlledPawn->GetMesh()->SkeletalMesh->Skeleton->GetReferenceSkeleton().GetRefBonePose();
	FTransform Transform = TransformArray[ControlledPawn->GetMesh()->GetBoneIndex(BonesRotation[bone].Key)];
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

TArray<int> AHynmersPlayerController::FindFrameRange(TArray<TArray<int>> Frames, int NumBones, int NumFrames)
{
	TArray<int> Range;
	for (int i = 0; i < NumBetterFrames; i++) {
		Range = TArray<int>({ Frames[0][0],Frames[0][0] }, 2);
		bool found = true;
		for (int bone = 0; bone < NumBones; bone++) {
			if (!ChangeRange(Range, Frames[bone])) {
				found = false;
				break;
			}
		}
		if (found)break;
	}
	if (Range[0] == Range[1]) {
		Range[0] = UKismetMathLibrary::Clamp(Range[0] - 2.5, 0.f, NumFrames);
		Range[1] = UKismetMathLibrary::Clamp(Range[1] + 2.5, 0.f, NumFrames);
	}
	return Range;
}

bool AHynmersPlayerController::ChangeRange(TArray<int32> &Range, TArray<int32> Frames)
{
	for (int i = 0; i < NumBetterFrames; i++) {
		if (UKismetMathLibrary::Abs(Range[0] - Frames[i]) <= 5 || UKismetMathLibrary::Abs(Range[1] - Frames[i]) <= 5) {
			Range[0] = UKismetMathLibrary::Min(Range[0], Frames[i]);
			Range[1] = UKismetMathLibrary::Max(Range[1], Frames[i]);
			return true;
		}
	}
	return false;
}

float AHynmersPlayerController::FindTimeInRange(TArray<TPair<FName, float>> BonesRotation, TArray<float> Range, UAnimSequence * Sequence, int32 CurrentDepth, float* Error)
{
	int32 NumBones = BonesRotation.Num();

	float TimeRange[] = { Range[0], (Range[1] - Range[0]) / 2.f, Range[1] };
	float Values[] = { 0.f, 0.f, 0.f };

	for (int time = 0; time < 3; time ++) {

		for (int bone = 0; bone < NumBones; bone++) {
			FQuat RefPoseRotation = GetRefPoseQuat(BonesRotation, bone);
			FTransform FrameTransform;
			Sequence->GetBoneTransform(FrameTransform, ControlledPawn->GetMesh()->GetBoneIndex(BonesRotation[bone].Key), TimeRange[time], true);
			FQuat FrameRotation = FrameTransform.GetRotation();

			FRotator FrameRotator = (RefPoseRotation.Inverse()*FrameRotation).Rotator();
			Values[time] += UKismetMathLibrary::Abs(FrameRotator.Yaw - BonesRotation[bone].Value);
		}
		Values[time] /= (float)NumBones;
	}
	TArray<int> BetterIndexes = FindMinsInArray(TArray<float>(Values, 3), 2);
	Range[0] = FMath::Min(TimeRange[BetterIndexes[0]], TimeRange[BetterIndexes[1]]);
	Range[1] = FMath::Max(TimeRange[BetterIndexes[0]], TimeRange[BetterIndexes[1]]);

	if (Error)*Error = Values[BetterIndexes[0]];

	if (CurrentDepth >= BinarySearchDepth)return TimeRange[BetterIndexes[0]];

	return FindTimeInRange(BonesRotation, Range, Sequence, CurrentDepth + 1, Error);
}
