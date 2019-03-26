// Fill out your copyright notice in the Description page of Project Settings.

#include "HExerciseWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"

#include "HGameInstance.h"

bool UHExerciseWidget::Initialize()
{
	bool Succes = Super::Initialize();
	if (!Succes) return false;

	if (!ensure(btn_Add)) return false;
	btn_Add->OnClicked.AddDynamic(this, &UHExerciseWidget::OnAdd);

	if (!ensure(btn_Remove)) return false;
	btn_Remove->OnClicked.AddDynamic(this, &UHExerciseWidget::OnRemove);

	if (!ensure(btn_Return)) return false;
	btn_Return->OnClicked.AddDynamic(this, &UHExerciseWidget::onReturn);

	return true;
}

void UHExerciseWidget::OnAdd()
{
	if (!GameInstance)return;

	FSessionInfo NewExercise = {
		ExerciseIndex,
		FCString::Atoi(*etb_Series->GetText().ToString()),
		FCString::Atoi(*etb_Reps->GetText().ToString()),
		TArray<float>({
			FCString::Atof(*etb_HipLS->GetText().ToString()),
			FCString::Atof(*etb_KneeLS->GetText().ToString()),
			FCString::Atof(*etb_HipRS->GetText().ToString()),
			FCString::Atof(*etb_KneeRS->GetText().ToString())}),
		TArray<float>({
			FCString::Atof(*etb_HipLI->GetText().ToString()),
			FCString::Atof(*etb_KneeLI->GetText().ToString()),
			FCString::Atof(*etb_HipRI->GetText().ToString()),
			FCString::Atof(*etb_KneeRI->GetText().ToString())})
	};

	GameInstance->AddExercise(NewExercise);

	Teardown();
}

void UHExerciseWidget::OnRemove()
{
	if (!GameInstance)return;

	FSessionInfo RemoveExercise;
	RemoveExercise.ExerciseIndex = ExerciseIndex;
	GameInstance->RemoveExercise(RemoveExercise);
}

void UHExerciseWidget::onReturn()
{
	Teardown();
}

void UHExerciseWidget::Teardown()
{
	this->RemoveFromViewport();
}
