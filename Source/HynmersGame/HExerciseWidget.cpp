// Fill out your copyright notice in the Description page of Project Settings.

#include "HExerciseWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"


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
}

void UHExerciseWidget::OnRemove()
{
}

void UHExerciseWidget::onReturn()
{
	Teardown();
}

void UHExerciseWidget::Teardown()
{
	this->RemoveFromViewport();
}
