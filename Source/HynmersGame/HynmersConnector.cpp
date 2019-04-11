// Fill out your copyright notice in the Description page of Project Settings.

#include "HynmersConnector.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Components/PointLightComponent.h"
#include "Curves/CurveVector.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AHynmersConnector::AHynmersConnector():Super()
{
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
	RootComponent = Spline;
}

FTransform AHynmersConnector::GetAttachLocation()
{
	if(!VerifySpline())
		return FTransform::Identity;

	FTransform BaseTransform = Spline->GetTransformAtSplinePoint(Spline->GetNumberOfSplinePoints(), ESplineCoordinateSpace::World);
	FVector FinalPointUpVector = Spline->GetUpVectorAtSplinePoint(Spline->GetNumberOfSplinePoints(), ESplineCoordinateSpace::World);
	BaseTransform.AddToTranslation(FinalPointUpVector * UpOffset);
	return BaseTransform;
}

bool AHynmersConnector::VerifySpline()
{
	return Spline && Spline->GetNumberOfSplinePoints();
}
