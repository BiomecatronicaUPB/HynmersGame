// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HynmersConnector.generated.h"

UCLASS()
class HYNMERSGAME_API AHynmersConnector : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spline", meta = (AllowPrivateAccess = "true"))
	class USplineComponent* Spline = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spline", meta = (AllowPrivateAccess = "true"))
	class UStaticMesh* StartConnection = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spline", meta = (AllowPrivateAccess = "true"))
	class UStaticMesh* MidConnection = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spline", meta = (AllowPrivateAccess = "true"))
	class UStaticMesh* EndConnection = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curve", meta = (AllowPrivateAccess = "true"))
	class UCurveVector* SplineCurve = nullptr;
	
	FVector EndLocation;

	bool VerifySpline();
	
public:	
	AHynmersConnector();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curve")
	float CurveEndOffset = 0.05f;

	// Angle in degrees
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spline")
	float ConeHalfAngle = 90.f;

public:	
	UFUNCTION(BlueprintCallable)
	FTransform GetAttachLocation();
	
};
