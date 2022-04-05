// Fill out your copyright notice in the Description page of Project Settings.


#include "subwayPawn.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h"
#include "Components/ActorComponent.h"

// Sets default values
AsubwayPawn::AsubwayPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	levels.Add("Subway");
	levels.Add("MainMap");


}

// Called when the game starts or when spawned
void AsubwayPawn::BeginPlay()
{
	Super::BeginPlay();

	currentTime = 0;
	
}

// Called every frame
void AsubwayPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	lightChange(DeltaTime);
}

// Called to bind functionality to input
void AsubwayPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AsubwayPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AsubwayPawn::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AsubwayPawn::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AsubwayPawn::LookUpAtRate);

}

void AsubwayPawn::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AsubwayPawn::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AsubwayPawn::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AsubwayPawn::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

AActor* AsubwayPawn::getObjectName(FString name)
{
	//get objects
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);

	for (AActor* Actor : Actors)
	{
		//UE_LOG(LogTemp, Warning, TEXT("NAME = %s"), *Actor->GetName());
		if (Actor->GetName() == name)
		{
			return Actor;
		}
	}

	return NULL;
}

void AsubwayPawn::lightChange(float deltaTime)
{
	currentTime += deltaTime;

	//if (30 <= currentTime && currentTime <= 40)
	//{
	//	AActor* object = getObjectName("light1");
	//	// dark to bright
	//	// Cast<Light>(object)->Component->SetIntensity(FMath::Lerp(begin, end, (currentTime - 60.0f) / 10.0f));
	//}

	//if (40 <= currentTime && currentTime <= 60)
	//{
	//	AActor* object = getObjectName("cloud");
	//	// cloud outside
	//	bool cubeHide = true;
	//	object->SetActorHiddenInGame(cubeHide);
	//}

	if (5 <= currentTime && currentTime <= 10)
	{
		// AActor* object = getObjectName("light2");
		// transit to club
		SwitchLevel(1);

	}
}	

void AsubwayPawn::SwitchLevel(float direction)
{
	if (direction)
	{
		FString current = GetWorld()->GetMapName();
		current.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

		for (int i = 0; i < levels.Num(); i++)
		{
			if (current == levels[i])
			{
				if (i == 0 && direction < 0)
				{
					i = levels.Num();
				}
				UGameplayStatics::OpenLevel(GetWorld(), FName(levels[i + direction]));
				break;
			}
		}

	}
}