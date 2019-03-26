// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class HYNMERSGAME_API UHMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	class UHGameInstance* GameInstance;

public:
	void Setup();
	virtual void Teardown();
	

	void SetGameInstance(UHGameInstance* CurrentGameInstance);
};
