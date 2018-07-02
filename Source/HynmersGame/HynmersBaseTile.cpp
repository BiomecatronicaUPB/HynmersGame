// Fill out your copyright notice in the Description page of Project Settings.

#include "HynmersBaseTile.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"


// Sets default values
AHynmersBaseTile::AHynmersBaseTile():Super()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Center = CreateDefaultSubobject<USceneComponent>("Center");
	SetRootComponent(Center);
	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	Box->SetupAttachment(RootComponent);
	Arrow = CreateDefaultSubobject<UArrowComponent>("Arrow");
	Arrow->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AHynmersBaseTile::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHynmersBaseTile::MakeDefaultTransform(FVector Location, FRotator Rotation, USceneComponent* Component)
{
	FTransform CurrentTransform = Component->GetRelativeTransform();
	Component->ResetRelativeTransform();
	Component->AddRelativeRotation(Rotation);
	Component->AddRelativeLocation(Location);
	Component->AddLocalOffset(CurrentTransform.GetLocation());
	Component->AddLocalRotation(CurrentTransform.GetRotation().Rotator());
}

// Called every frame
void AHynmersBaseTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FTransform AHynmersBaseTile::GetAttachLocation()
{
	if (Arrow) {
		return Arrow->GetComponentTransform();
	}
	return FTransform::Identity;
}
