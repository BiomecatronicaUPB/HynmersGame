// Fill out your copyright notice in the Description page of Project Settings.

#include "HGameInstance.h"
#include "UObject/ConstructorHelpers.h"

#include "HMainMenu.h"

UHGameInstance::UHGameInstance(const FObjectInitializer & ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/Hynmers/UI/WBP_MainMenu"));
	if (!ensure(MenuBPClass.Class != nullptr)) return;

	MenuClass = MenuBPClass.Class;
}

bool UHGameInstance::LookfForExercise(const FSessionInfo & ExerciseToFind, int32 & ExerciseFound)
{
	for(int Index = 0; Index < SessionInfo.Num(); Index++)
	{
		FSessionInfo& CurrentExercise = SessionInfo[Index];
		if (CurrentExercise.ExerciseIndex == ExerciseToFind.ExerciseIndex) {
			ExerciseFound = Index;
			return true;
		}
	}
	return false;
}

bool UHGameInstance::AddExercise(FSessionInfo & NewExercise)
{
	int32 ExerciseFound;
	if (LookfForExercise(NewExercise, ExerciseFound)) return false;
	SessionInfo.Add(NewExercise);
	return true;
}

void UHGameInstance::RemoveExercise(FSessionInfo & NewExercise)
{
	int32 ExerciseFound;
	if (!LookfForExercise(NewExercise, ExerciseFound)) return;
	UE_LOG(LogTemp, Warning, TEXT("Ima Removing"));
	SessionInfo.RemoveAt(ExerciseFound);
}

void UHGameInstance::GetGameSessionInfo(TArray<FSessionInfo>& GameSessionInfo)
{
	GameSessionInfo = SessionInfo;
}

void UHGameInstance::StartGame()
{
	if (!Menu)return;

	Menu->Teardown();

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	World->ServerTravel("/Game/Hynmers/Maps/HG_Level1");
}

void UHGameInstance::LoadMenuWidget()
{
	if (!ensure(MenuClass != nullptr)) return;

	Menu = CreateWidget<UHMainMenu>(this, MenuClass);
	if (!ensure(Menu != nullptr)) return;

	Menu->Setup();

	Menu->SetGameInstance(this);
}
