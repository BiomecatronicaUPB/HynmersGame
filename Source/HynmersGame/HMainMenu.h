// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HMenuWidget.h"
#include "HMainMenu.generated.h"

/**
 * 
 */
UCLASS()
class HYNMERSGAME_API UHMainMenu : public UHMenuWidget
{
	GENERATED_BODY()

	TSubclassOf<class UUserWidget> ExerciseWidget;

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	UWidget* SelectPatient;

	UPROPERTY(meta = (BindWidget))
	UWidget* SelectExercises;

	UPROPERTY(meta = (BindWidget))
	class UButton* btn_Exit;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* tb_Patient;

	UPROPERTY(meta = (BindWidget))
	UButton* btn_Ingress;

	UPROPERTY(meta = (BindWidget))
	UButton* btn_Init;

	UPROPERTY(meta = (BindWidget))
	UButton* btn_Return;

	UPROPERTY(meta = (BindWidget))
	UButton* btn_Gate;

	UPROPERTY(meta = (BindWidget))
	UButton* btn_Jump;

	UPROPERTY(meta = (BindWidget))
	UButton* btn_Swim;

	UPROPERTY(meta = (BindWidget))
	UButton* btn_Bike;

	UPROPERTY(meta = (BindWidget))
	UButton* btn_Board;

	UPROPERTY(meta = (BindWidget))
	UButton* btn_Legs;


	UFUNCTION()
	void ExitGame();

	UFUNCTION()
	void StartGame();

	UFUNCTION()
	void IngressPatien();

	UFUNCTION()
	void ReturnToPatient();




public:
	UHMainMenu(const FObjectInitializer & ObjectInitializer);

protected:
	virtual bool Initialize() override;
	
};
