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

	TSubclassOf<class UHExerciseWidget> ExerciseWidgetClass;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Exercises", meta = (AllowPrivateAccess = "true"))
	TArray<class UTexture2D*> ExercisesList;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Exercises", meta = (AllowPrivateAccess = "true"))
	TArray<FString> ExercisesDescription;

	UFUNCTION()
	void ExitGame();

	UFUNCTION()
	void StartGame();

	UFUNCTION()
	void IngressPatien();

	UFUNCTION()
	void ReturnToPatient();

	UFUNCTION()
	void OnGate();
	
	UFUNCTION()
	void OnJump();

	UFUNCTION()
	void OnSwim();

	UFUNCTION()
	void OnBike();

	UFUNCTION()
	void OnBoard();

	UFUNCTION()
	void OnLegs();

	void ExecOnExerciseWidgetOpen(UTexture2D* ImageToSet, FString ExerciseDescription);

	bool CheckImageDesc(const int32 &ExerciseIndex, FString &ExerciseName);



public:
	UHMainMenu(const FObjectInitializer & ObjectInitializer);

protected:
	virtual bool Initialize() override;
	
};
