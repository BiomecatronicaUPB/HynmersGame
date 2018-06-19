// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HynmersBaseTile.generated.h"

UCLASS()
class HYNMERSGAME_API AHynmersBaseTile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHynmersBaseTile();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USceneComponent* Center;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* Box;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* Arrow;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void MakeDefaultTransform(FVector Location, FRotator Rotation, USceneComponent* Component);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual FTransform GetAttachLocation();

	
	
};
