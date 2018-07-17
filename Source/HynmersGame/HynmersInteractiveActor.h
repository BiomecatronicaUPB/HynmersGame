// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HynmersInteractiveActor.generated.h"

UCLASS()
class HYNMERSGAME_API AHynmersInteractiveActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHynmersInteractiveActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interaction")
	void Interact(APawn* InteractionPawn);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
