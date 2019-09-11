// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "HynmersCameraComponent.generated.h"

/**
 * 
 */
UCLASS()
class HYNMERSGAME_API UHynmersCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

private:
	USceneComponent* Parent;

	FQuat StartHMDOrientation;

	TSharedPtr<class IXRCamera, ESPMode::ThreadSafe> XRCamera;

	FQuat InitialRotation;

public:
	virtual void BeginPlay() override;

	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraSettings)
	uint32 bLockToOculus : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraSettings)
	bool bActiveCameraOffset = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraSettings, meta = (EditCondition = "bActiveCameraOffset"))
	float CameraOffset = 0.f;
	
	
};
