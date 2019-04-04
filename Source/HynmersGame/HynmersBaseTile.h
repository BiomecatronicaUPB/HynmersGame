// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HynmersBaseTile.generated.h"

UCLASS()
class HYNMERSGAME_API AHynmersBaseTile : public AActor
{
	GENERATED_BODY()
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transform", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Center;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transform", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* Arrow;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transform", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* Box;

public:	
	// Sets default values for this actor's properties
	AHynmersBaseTile();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Defaults")
	virtual void MakeDefaultTransform(FVector Location, FRotator Rotation, USceneComponent* Component);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Attachment")
	virtual FTransform GetAttachLocation();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Transform")
		int32 Series;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Transform")
		int32 TotalSeries;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Transform")
		int32 Repetitions;

	FORCEINLINE UBoxComponent* GetBoxComponent() { return Box; }
	
};
