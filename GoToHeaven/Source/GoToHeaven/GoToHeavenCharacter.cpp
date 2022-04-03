// Copyright Epic Games, Inc. All Rights Reserved.

#include "GoToHeavenCharacter.h"
#include "GoToHeavenProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AGoToHeavenCharacter

AGoToHeavenCharacter::AGoToHeavenCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
}

void AGoToHeavenCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void AGoToHeavenCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	value = FMath::Clamp(value - DeltaTime, 0.0f, 200.0f);
	affectVision();

	timelift -= DeltaTime;
	currentTime += DeltaTime;
}

void AGoToHeavenCharacter::drink()
{
	FTimerHandle* curTimer = new FTimerHandle();
	timers.Add(curTimer);
	timersElipsed.Add(0);

	GetWorldTimerManager().SetTimer(*curTimer, this, &AGoToHeavenCharacter::drinkProcess, 0.167, true, 0);
}

void AGoToHeavenCharacter::drinkProcess()
{
	for (int i = 0; i < timers.Num(); i += 1)
	{
		float timerRate = GetWorldTimerManager().GetTimerRate(*timers[i]) * 11;

		value += timerRate;
		value = FMath::Clamp(value, 0.0f, 200.0f);
		timersElipsed[i] += timerRate;

		affectVision();

		UE_LOG(LogTemp, Warning, TEXT("i = %d, v = %f time = %f"), timers.Num(), value, GetWorldTimerManager().GetTimerRate(*timers[i]));

		if (timersElipsed[i] > 33.3f)
		{
			GetWorldTimerManager().ClearTimer(*timers[i]);
			delete(timers[i]);

			timers.RemoveAt(i);
			timersElipsed.RemoveAt(i);

			i -= 1;
		}
	}
}

void AGoToHeavenCharacter::affectVision()
{
	float limit = 100.0f;
	float maskLimit = 20.0f;
	
	if (value <= limit) 
	{
		((FPostProcessSettings*)GetWorld()->PostProcessVolumes[0]->GetProperties().Settings)->DepthOfFieldFocalDistance = FMath::Lerp(50.0f, 0.01f, value / limit);

		//change mask a
		if (value > limit - maskLimit)
		{
			ui->changeWBackgroundAlpha(FMath::Lerp(0.0f, 1.0f, (value - (limit - maskLimit)) / maskLimit));
		}
	}
	else 
	{
		((FPostProcessSettings*)GetWorld()->PostProcessVolumes[0]->GetProperties().Settings)->DepthOfFieldFocalDistance = FMath::Lerp(0.01f, 50.0f, (value - limit) / limit);

		if (value < limit + maskLimit)
		{
			ui->changeWBackgroundAlpha(FMath::Lerp(1.0f, 0.0f, (value - limit) / maskLimit));
		}
		else 
		{
			ui->changeWBackgroundAlpha(0.0f);
		}

		this->getname
	}
}

void AGoToHeavenCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AGoToHeavenCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGoToHeavenCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AGoToHeavenCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AGoToHeavenCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Drink", IE_Released, this, &AGoToHeavenCharacter::drink);
}


void AGoToHeavenCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AGoToHeavenCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AGoToHeavenCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AGoToHeavenCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AGoToHeavenCharacter::Episodes()
{	
	//episode 1
	float episode = 0; 
	
	if (currentTime < )
	{

	}


	





}