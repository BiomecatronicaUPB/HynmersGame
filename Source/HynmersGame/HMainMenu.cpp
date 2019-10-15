// Fill out your copyright notice in the Description page of Project Settings.

#include "HMainMenu.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "Engine/Texture.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"

#include "HExerciseWidget.h"
#include "HynmersGame.h"
#include "HGameInstance.h"

UHMainMenu::UHMainMenu(const FObjectInitializer & ObjectInitializer):Super(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UHExerciseWidget> ExerciseWidgetBP(TEXT("/Game/Hynmers/UI/WBP_ExerciseWidget"));
	if (!ensure(ExerciseWidgetBP.Class)) return;

	ExerciseWidgetClass = ExerciseWidgetBP.Class;
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

	if (!ensure(btn_Gate))return false;
	btn_Gate->OnClicked.AddDynamic(this, &UHMainMenu::OnGate);

	if (!ensure(btn_Jump))return false;
	btn_Jump->OnClicked.AddDynamic(this, &UHMainMenu::OnJump);

	if (!ensure(btn_Swim))return false;
	btn_Swim->OnClicked.AddDynamic(this, &UHMainMenu::OnSwim);

	if (!ensure(btn_Bike))return false;
	btn_Bike->OnClicked.AddDynamic(this, &UHMainMenu::OnBike);

	if (!ensure(btn_Board))return false;
	btn_Board->OnClicked.AddDynamic(this, &UHMainMenu::OnBoard);

	if (!ensure(btn_Legs))return false;
	btn_Legs->OnClicked.AddDynamic(this, &UHMainMenu::OnLegs);


	return true;
}

void UHMainMenu::ExitGame()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Type::Quit);
}

void UHMainMenu::StartGame()
{
	if (!GameInstance)return;
	GameInstance->StartGame();
}

void UHMainMenu::IngressPatien()
{
	if (!GameInstance)return;

	GameInstance->PatientName = etb_Patient->GetText().ToString();
	GameInstance->PatientName = (GameInstance->PatientName.IsEmpty()) ? "Default" : GameInstance->PatientName;
	MenuSwitcher->SetActiveWidget(SelectExercises);
}

void UHMainMenu::ReturnToPatient()
{
	MenuSwitcher->SetActiveWidget(SelectPatient);
}

void UHMainMenu::OnGate()
{
	int32 ExerciseIndex = 0;
	FString ExerciseName = "Gate";

	if (!CheckImageDesc(ExerciseIndex, ExerciseName)) return;

	ExecOnExerciseWidgetOpen(btn_Gate, ExercisesList[ExerciseIndex], ExercisesDescription[ExerciseIndex], (int32)EActiveTile::Bridge);
}


void UHMainMenu::OnJump()
{
	int32 ExerciseIndex = 1;
	FString ExerciseName = "Jump";

	if (!CheckImageDesc(ExerciseIndex, ExerciseName)) return;

	ExecOnExerciseWidgetOpen(btn_Jump, ExercisesList[ExerciseIndex], ExercisesDescription[ExerciseIndex], (int32)EActiveTile::Lab);
}

void UHMainMenu::OnSwim()
{
	int32 ExerciseIndex = 2;
	FString ExerciseName = "Swim";

	if (!CheckImageDesc(ExerciseIndex, ExerciseName)) return;

	ExecOnExerciseWidgetOpen(btn_Swim, ExercisesList[ExerciseIndex], ExercisesDescription[ExerciseIndex], (int32)EActiveTile::Machine);
}

void UHMainMenu::OnBike()
{
	int32 ExerciseIndex = 3;
	FString ExerciseName = "Bike";

	if (!CheckImageDesc(ExerciseIndex, ExerciseName)) return;

	ExecOnExerciseWidgetOpen(btn_Bike, ExercisesList[ExerciseIndex], ExercisesDescription[ExerciseIndex], (int32)EActiveTile::Com);
}

void UHMainMenu::OnBoard()
{
	int32 ExerciseIndex = 4;
	FString ExerciseName = "Board";

	if (!CheckImageDesc(ExerciseIndex, ExerciseName)) return;

	ExecOnExerciseWidgetOpen(btn_Board, ExercisesList[ExerciseIndex], ExercisesDescription[ExerciseIndex], (int32)EActiveTile::Rooms);
}

void UHMainMenu::OnLegs()
{
	int32 ExerciseIndex = 5;
	FString ExerciseName = "Legs";

	if (!CheckImageDesc(ExerciseIndex, ExerciseName)) return;

	ExecOnExerciseWidgetOpen(btn_Legs, ExercisesList[ExerciseIndex], ExercisesDescription[ExerciseIndex], (int32)EActiveTile::Nav);
}

void UHMainMenu::ExecOnExerciseWidgetOpen(UButton* ClickedButton, UTexture2D* ImageToSet, FString ExerciseDescription, int32 ExerciseIndex)
{
	if (!ExerciseWidgetClass)return;

	UHExerciseWidget* ExerciseWidget = Cast<UHExerciseWidget>(CreateWidget<UUserWidget>(GetWorld(), ExerciseWidgetClass));
	if (!ExerciseWidget) return;

	ExerciseWidget->img_Image->SetBrushFromTexture(ImageToSet, true);
	ExerciseWidget->txt_Description->SetText(FText::FromString(ExerciseDescription));
	ExerciseWidget->ExerciseIndex = ExerciseIndex;
	ExerciseWidget->SetGameInstance(GameInstance);
	ExerciseWidget->btn_Pressed = ClickedButton;

	ExerciseWidget->Setup();
}

bool UHMainMenu::CheckImageDesc(const int32 &ExerciseIndex, FString &ExerciseName)
{
	if (!ExercisesList.IsValidIndex(ExerciseIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("Image for exercises %s is missing"), *ExerciseName);
		return false;
	}
	if (!ExercisesDescription.IsValidIndex(ExerciseIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("Description for exercises %s is missing"), *ExerciseName);
		return false;
	}
	return true;
}
