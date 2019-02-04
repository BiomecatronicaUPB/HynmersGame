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

	for (int i = 0; i < Sequences.Num(); i++) {
		UAnimSequence* Sequence = Sequences[i];
		// Creating montages
		CreateMontage(Sequence, Montages[i]);

		NumBetterFrames.Add(UKismetMathLibrary::Clamp(NumBetterFramesDefault, 0, Sequence->GetNumberOfFrames()));
		PastAnimTime.Add(SelectSkeletonPose({0, Sequence->GetNumberOfFrames()}, i));
	}

	BonesAngles.Add("thigh_L", 0.f);
	BonesAngles.Add("shin_L", 0.f);
	BonesAngles.Add("thigh_R", 0.f);
	BonesAngles.Add("shin_R", 0.f);

}

void AHynmersPlayerController::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction & ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	if (CheckAllNeededAssets() && CheckMontages()) {

		int32 ActiveTile = ControlledPawn->GetCurrentTile();

		TArray<UAnimMontage*> ActiveMontages = { Montages[0], Montages[MapMontage[ActiveTile]] };
		UAnimMontage* CurrentMontage = (ControlledPawn->bInteractWith) ? ActiveMontages[0] : ActiveMontages[1];
		int32 ActiveMontageID = (ControlledPawn->bInteractWith) ? 0 : MapMontage[ActiveTile];

		float CurrentPosition = PawnAnimationInstance->Montage_GetPosition(CurrentMontage);
		int MinRangeValue = UKismetMathLibrary::Clamp(CurrentMontage->GetFrameAtTime(CurrentPosition) - NumSearchFrames / 2, 0, CurrentMontage->GetNumberOfFrames());
		int MaxRangeValue = UKismetMathLibrary::Clamp(CurrentMontage->GetFrameAtTime(CurrentPosition) + NumSearchFrames / 2, 0, CurrentMontage->GetNumberOfFrames());

		NumBetterFrames[ActiveMontageID] = UKismetMathLibrary::Clamp(NumBetterFrames[ActiveMontageID], 0, NumSearchFrames);
		float Error = 0;
		float CurrentTime = SelectSkeletonPose({MinRangeValue, MaxRangeValue}, ActiveMontageID, &Error);

		UE_LOG(LogTemp, Warning, TEXT("ERValue: %f"), Error)

		if (!bInitAccel && CurrentTime - PastAnimTime[ActiveMontageID] != 0) {
			bInitAccel = true;
		}

		if (bInitAccel && Error <MAxErrors[ActiveMontageID]) {
			ControlledPawn->MoveForward(1.f);
			PastAnimTime[ActiveMontageID] = CurrentTime;
			if (++AccelTimes >= MaxAccelTimes) {
				bInitAccel = false;
				AccelTimes = 0;
			}
		}

	}
}

float AHynmersPlayerController::SelectSkeletonPose(TArray<int> FrameSearchRange, uint32 SequenceID, float *Error)
{
	float ResultTime = FindPoseInMontage(BonesAngles, FrameSearchRange, SequenceID, Error);

	// Montage reproduction control
	PawnAnimationInstance->Montage_Play(Montages[SequenceID]);
	PawnAnimationInstance->Montage_SetPosition(Montages[SequenceID], ResultTime);
	PawnAnimationInstance->Montage_Pause(Montages[SequenceID]);

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
	float Angle = -(rate * 180.f / PI) + ThighOffset;
	Angle = (Angle < -180.f) ? 2 * 180 + Angle : Angle;
	BonesAngles.Add("thigh_L", Angle);
}

void AHynmersPlayerController::PosLeftKnee(float rate)
{
	BonesAngles.Add("shin_L", rate * 180.f / PI);
}

void AHynmersPlayerController::PosRightThight(float rate)
{
	float Angle = -(rate * 180.f / PI) + ThighOffset;
	Angle = (Angle < -180.f) ? 2 * 180 + Angle : Angle;
	BonesAngles.Add("thigh_R", Angle);
}

void AHynmersPlayerController::PosRightKnee(float rate)
{
	BonesAngles.Add("shin_R", rate * 180.f / PI);
}

float AHynmersPlayerController::FindPoseInMontage(TMap<FName,float> BonesRotation, TArray<int> FrameSearchRange, uint32 MontageID, float* Error)
{
	if(!BonesRotation.Num() || !Montages[MontageID])
		return 0.0f;

	const int32 NumBones = BonesRotation.Num();
	const int32 NumFrames = FrameSearchRange[1] - FrameSearchRange[0];

	TArray<float> FramesFound;
	FramesFound.SetNum(NumFrames);

	UAnimMontage* Montage = Montages[MontageID];

	TArray<int>BetterFrames;

	for (int frame = FrameSearchRange[0]; frame < FrameSearchRange[1]; frame++) {
		FramesFound[frame - FrameSearchRange[0]] = CalculateTimeValue(BonesRotation, Sequences[MontageID], Sequences[MontageID]->GetTimeAtFrame(frame));
	}
	BetterFrames = FindMinsInArray(FramesFound, NumBetterFrames[MontageID]);

	TArray<float> SelectedTimes;
	SelectedTimes.SetNum(NumBetterFrames[MontageID]);

	TArray<float> SelectedTimesError;
	SelectedTimesError.SetNum(NumBetterFrames[MontageID]);

	for (int i = 0; i < NumBetterFrames[MontageID]; i++) {
		float MinFrame = UKismetMathLibrary::Clamp(BetterFrames[i] + FrameSearchRange[0] - 1, 0, Montage->GetNumberOfFrames());
		float MaxFrame = UKismetMathLibrary::Clamp(BetterFrames[i] + FrameSearchRange[0] + 1, 0, Montage->GetNumberOfFrames());
		SelectedTimes[i] = FindTimeInRange(BonesRotation, TArray<float>({ Montage->GetTimeAtFrame(MinFrame),Montage->GetTimeAtFrame(MaxFrame) }, 2), Sequences[MontageID], 0, &SelectedTimesError[i]);
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
	int32 index = ControlledPawn->GetMesh()->GetBoneIndex(BonesKeys[bone]);
	FTransform Transform = (index == INDEX_NONE) ? FTransform::Identity : TransformArray[index];
	return Transform.GetRotation();
}

bool AHynmersPlayerController::CheckAnimAssets()
{
	return WalkSequence&& JumpSequence && SwimSequence && BoardSequence && BikeSequence && LegsMoveSequence;
}

bool AHynmersPlayerController::CheckAllNeededAssets()
{
	return MAxErrors.Num() == Sequences.Num() && CheckAnimAssets() && PawnAnimationInstance;
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
		
		FTransform FrameTransform;
		Sequence->GetBoneTransform(FrameTransform, ControlledPawn->GetMesh()->GetBoneIndex(BonesKeys[bone]), Time, true);
		FRotator FrameRotator = FrameTransform.GetRotation().Rotator();
		Value += ((BonesKeys[bone].ToString().Contains("thigh")) ? ThighWeight : KneeWeight)*UKismetMathLibrary::Abs(FrameRotator.Yaw - BonesRotation[BonesKeys[bone]]);
	}
	Value /= (float)NumBones;

	return Value;
}

void AHynmersPlayerController::CreateMontage(UAnimSequence* Sequence, UAnimMontage* &OutMontage)
{
	if (CheckAllNeededAssets()) {
		OutMontage = UAnimMontage::CreateSlotAnimationAsDynamicMontage(Sequence, FName("FullBody"));
	}
}
