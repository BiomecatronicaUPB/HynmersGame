// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HGameInstance.h"
#include "HynmersGame.h"
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transform", meta = (AllowPrivateAccess = "true"))
	EActiveTile TileType;
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

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void InitParameters(const TArray<FSessionInfo>& GameSessionInfo);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mechanics")
	int32 Series;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mechanics")
	int32 TotalSeries;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mechanics")
	int32 Repetitions;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mechanics")
	TArray<float> UpperLimits;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mechanics")
	TArray<float> LowerLimits;

	UFUNCTION()
	void OnBoxOverlapping(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	FORCEINLINE UBoxComponent* GetBoxComponent() { return Box; }
	
};
