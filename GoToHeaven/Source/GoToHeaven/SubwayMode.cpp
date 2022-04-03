// Fill out your copyright notice in the Description page of Project Settings.


#include "SubwayMode.h"

ASubwayMode::ASubwayMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/subway"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	
}