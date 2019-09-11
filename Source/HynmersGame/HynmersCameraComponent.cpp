// Fill out your copyright notice in the Description page of Project Settings.

#include "HynmersCameraComponent.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Engine/Engine.h"
#include "IHeadMountedDisplay.h"
#include "IXRTrackingSystem.h"
#include "IXRCamera.h"
#include "OculusFunctionLibrary.h"

void UHynmersCameraComponent::BeginPlay() {
	Super::BeginPlay();

	TArray<USceneComponent*> Parents;
	GetParentComponents(Parents);

	if (Parents.Num() > 0) {
		Parent = Parents[0];
	}
	XRCamera = GEngine->XRSystem.Get()->GetXRCamera();

	if (Parent) {
		InitialRotation = Parent->RelativeRotation.Quaternion();
	}
	else {
		InitialRotation = RelativeRotation.Quaternion();
	}

	if (bLockToOculus && GEngine->XRSystem.IsValid() && GetWorld()->WorldType != EWorldType::Editor && GEngine && bActiveCameraOffset) {
		
		StartHMDOrientation = FRotator(CameraOffset, 0.f, 0.f).Quaternion();
		Parent->SetWorldRotation(FRotator(Parent->GetComponentRotation().Pitch, StartHMDOrientation.Rotator().Yaw, StartHMDOrientation.Rotator().Roll));
		UE_LOG(LogTemp, Warning, TEXT("%s"), *StartHMDOrientation.Rotator().ToString())
	}

}

void UHynmersCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) {

	if (bLockToOculus && GEngine->XRSystem.IsValid() && GEngine->XRSystem.Get()->IsHeadTrackingAllowed() && GetWorld()->WorldType != EWorldType::Editor)
	{
		const FTransform ParentWorld = CalcNewComponentToWorld(FTransform());
		XRCamera->SetupLateUpdate(ParentWorld, this);

		FQuat Orientation;
		FVector Position;
		FRotator OculusOrientation;
		if (XRCamera->UpdatePlayerCamera(Orientation, Position))
		{
			FQuat DeltaRotation = StartHMDOrientation.Inverse()* InitialRotation* Orientation;
			UE_LOG(LogTemp, Warning, TEXT("%s"), *DeltaRotation.Rotator().ToString());

			if (Parent){
				Parent->SetRelativeRotation(DeltaRotation);
			}
			else {
				SetRelativeRotation(DeltaRotation);
			}

		}
		else
		{
			ResetRelativeTransform();
		}
	}else if (bLockToHmd && GEngine && GEngine->XRSystem.IsValid() && GetWorld() && GetWorld()->WorldType != EWorldType::Editor)
	{
		IXRTrackingSystem* XRSystem = GEngine->XRSystem.Get();

		auto XRCamera = XRSystem->GetXRCamera();
		if (XRSystem->IsHeadTrackingAllowed() && XRCamera.IsValid())
		{
			const FTransform ParentWorld = CalcNewComponentToWorld(FTransform());

			XRCamera->SetupLateUpdate(ParentWorld, this);

			FQuat Orientation;
			FVector Position;
			if (XRCamera->UpdatePlayerCamera(Orientation, Position))
			{
				SetRelativeTransform(FTransform(Orientation, Position));
			}
			else
			{
				ResetRelativeTransform();
			}

			XRCamera->OverrideFOV(this->FieldOfView);
		}
	}


	if (bUsePawnControlRotation)
	{
		const APawn* OwningPawn = Cast<APawn>(GetOwner());
		const AController* OwningController = OwningPawn ? OwningPawn->GetController() : nullptr;
		if (OwningController && OwningController->IsLocalPlayerController())
		{
			const FQuat PawnViewRotation = OwningPawn->GetViewRotation().Quaternion();
			if (!PawnViewRotation.Equals(GetComponentRotation().Quaternion()))
			{
				if (Parent) {
					Parent->SetRelativeRotation(PawnViewRotation);
				}
				else {
					SetWorldRotation(PawnViewRotation);
				}
			}
		}
	}

	if (bUseAdditiveOffset)
	{
		FTransform OffsetCamToBaseCam = AdditiveOffset;
		FTransform BaseCamToWorld = GetComponentToWorld();
		FTransform OffsetCamToWorld = OffsetCamToBaseCam * BaseCamToWorld;

		DesiredView.Location = OffsetCamToWorld.GetLocation();
		DesiredView.Rotation = OffsetCamToWorld.Rotator();
	}
	else
	{
		DesiredView.Location = GetComponentLocation();
		DesiredView.Rotation = GetComponentRotation();
	}

	DesiredView.FOV = bUseAdditiveOffset ? (FieldOfView + AdditiveFOVOffset) : FieldOfView;
	DesiredView.AspectRatio = AspectRatio;
	DesiredView.bConstrainAspectRatio = bConstrainAspectRatio;
	DesiredView.bUseFieldOfViewForLOD = bUseFieldOfViewForLOD;
	DesiredView.ProjectionMode = ProjectionMode;
	DesiredView.OrthoWidth = OrthoWidth;
	DesiredView.OrthoNearClipPlane = OrthoNearClipPlane;
	DesiredView.OrthoFarClipPlane = OrthoFarClipPlane;

	// See if the CameraActor wants to override the PostProcess settings used.
	DesiredView.PostProcessBlendWeight = PostProcessBlendWeight;
	if (PostProcessBlendWeight > 0.0f)
	{
		DesiredView.PostProcessSettings = PostProcessSettings;
	}

}

