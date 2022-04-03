// Copyright Epic Games, Inc. All Rights Reserved.

#include "GoToHeavenGameMode.h"
#include "GoToHeavenHUD.h"
#include "GoToHeavenCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGoToHeavenGameMode::AGoToHeavenGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
