// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EActiveTile : uint8
{
	Bridge,				// Walk
	Lab,				// Jump
	Machine,			// Swim
	Rooms,				// Board
	Com,				// Bike
	Nav,				// Legs
	TotalTiles UMETA(Hidden = "true")
};