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
#include "Engine/RectLight.h"
#include "Engine/StaticMeshActor.h"
#include "Components/RectLightComponent.h"
#include "Materials/MaterialInstanceConstant.h"

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
	currentTime = 65;
}

void AGoToHeavenCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	value = FMath::Clamp(value - DeltaTime, 0.0f, 200.0f);
	Episodes(DeltaTime);
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

void AGoToHeavenCharacter::Episodes(float deltaTime)
{	
	currentTime += deltaTime;

	TArray<bool> triggered;
	
	for (int i = 0; i < 5 * 3; i += 1)
	{
		triggered.Add(false);
	}

	//photo
	if (60 <= currentTime && currentTime <= 65)
	{
		AActor* object = getObjectName("light0"); 
		Cast<ARectLight>(object)->RectLightComponent->SetIntensity(FMath::Lerp(0, 312500, (currentTime - 60.0f) / 5.0f));
	}

	if (65 <= currentTime && currentTime <= 70)
	{
		AActor* object = getObjectName("light0");
		Cast<ARectLight>(object)->RectLightComponent->SetIntensity(FMath::Lerp(312500, 0, (currentTime - 65.0f) / 5.0f));

	}

	if (67.5f <= currentTime && currentTime <= 72.5f)
	{
		TArray<AActor*> posts;
		posts.Add(getObjectName("Plane2_8"));
		posts.Add(getObjectName("Plane3_11"));
		posts.Add(getObjectName("Plane4_14"));

		for (int i = 0; i < 3; i += 1) 
		{
			Cast<AStaticMeshActor>(posts[i])->GetStaticMeshComponent()->SetScalarParameterValueOnMaterials(TEXT("lightness"), FMath::Lerp(1.0f, 0.025f, (currentTime - 67.5f) / 5.0f));
		}
	}


	if (currentTime > 90 && !triggered[3])
	{
		AActor* object = getObjectName("light1");
		Cast<ARectLight>(object)->RectLightComponent->SetIntensity(312500);

		triggered[3] = true;
	}


	if (currentTime > 120 && !triggered[6])
	{
		AActor* object = getObjectName("light2");
		Cast<ARectLight>(object)->RectLightComponent->SetIntensity(312500);

		triggered[6] = true;
	}


	if (currentTime > 150 && !triggered[9])
	{
		AActor* object = getObjectName("light3");
		Cast<ARectLight>(object)->RectLightComponent->SetIntensity(312500);

		triggered[9] = true;
	}


	if (currentTime > 180 && !triggered[12])
	{
		AActor* object = getObjectName("light4");
		Cast<ARectLight>(object)->RectLightComponent->SetIntensity(312500);

		triggered[12] = true;
	}

}

AActor* AGoToHeavenCharacter::getObjectName(FString name)
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