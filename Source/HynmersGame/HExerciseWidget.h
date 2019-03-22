// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HMenuWidget.h"
#include "HExerciseWidget.generated.h"

/**
 * 
 */
UCLASS()
class HYNMERSGAME_API UHExerciseWidget : public UHMenuWidget
{
	GENERATED_BODY()
		
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_Add;

	UPROPERTY(meta = (BindWidget))
	UButton* btn_Remove;

	UPROPERTY(meta = (BindWidget))
	UButton* btn_Return;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* etb_Series;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* etb_Reps;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* etb_HipLS;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* etb_KneeLS;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* etb_HipRS;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* etb_KneeRS;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* etb_HipLI;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* etb_KneeLI;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* etb_HipRI;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* etb_KneeRI;

	UFUNCTION()
		void OnAdd();

	UFUNCTION()
		void OnRemove();

	UFUNCTION()
		void onReturn();

protected:
	virtual bool Initialize() override;

public:

	UPROPERTY(meta = (BindWidget))
		class UImage *img_Image;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* txt_Description;

	virtual void Teardown() override;

};
