// Fill out your copyright notice in the Description page of Project Settings.


#include "Candle.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "GoToHeavenCharacter.h"
#include "Components/PointLightComponent.h"

// Sets default values
ACandle::ACandle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACandle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACandle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float value = Cast<AGoToHeavenCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn())->value;

	if (value < 140) 
	{
		SetActorHiddenInGame(true);
	}
	else if(value < 180)
	{
		SetActorHiddenInGame(false);
		Cast<UParticleSystemComponent>(GetDefaultSubobjectByName(TEXT("Fire")))->SetHiddenInGame(false);

		if (value < 160) 
		{
			Cast<UPointLightComponent>(GetDefaultSubobjectByName(TEXT("Light")))->SetIntensity(FMath::Lerp(0, 800, (value - 140) / 20.0f));
		}
		else 
		{
			Cast<UPointLightComponent>(GetDefaultSubobjectByName(TEXT("Light")))->SetIntensity(FMath::Lerp(800, 0, (value - 160) / 20.0f));
		}
	}
	else 
	{
		SetActorHiddenInGame(false);
		Cast<UParticleSystemComponent>(GetDefaultSubobjectByName(TEXT("Fire")))->SetHiddenInGame(true);
	}
}

