// Fill out your copyright notice in the Description page of Project Settings.

#include "HMainMenu.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"

UHMainMenu::UHMainMenu(const FObjectInitializer & ObjectInitializer):Super(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> ExerciseWidgetBP(TEXT("/Game/Hynmers/UI/WBP_ExerciseWidget"));
	if (!ensure(ExerciseWidgetBP.Class)) return;

	ExerciseWidget = ExerciseWidgetBP.Class;
}

bool UHMainMenu::Initialize()
{
	bool Succes = Super::Initialize();
	if (!Succes) return false;

	if (!ensure(btn_Exit)) return false;
	btn_Exit->OnClicked.AddDynamic(this,&UHMainMenu::ExitGame);

	if (!ensure(btn_Ingress)) return false;
	btn_Ingress->OnClicked.AddDynamic(this, &UHMainMenu::IngressPatien);

	if (!ensure(btn_Init)) return false;
	btn_Init->OnClicked.AddDynamic(this, &UHMainMenu::StartGame);

	if (!ensure(btn_Return)) return false;
	btn_Return->OnClicked.AddDynamic(this, &UHMainMenu::ReturnToPatient);

	return true;
}

void UHMainMenu::ExitGame()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Type::Quit);
}

void UHMainMenu::StartGame()
{

}

void UHMainMenu::IngressPatien()
{
	MenuSwitcher->SetActiveWidget(SelectExercises);
}

void UHMainMenu::ReturnToPatient()
{
	MenuSwitcher->SetActiveWidget(SelectPatient);
}
