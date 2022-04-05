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
#include "Engine/Light.h"

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
	currentTime = 53.6f;

	getLightSource();
}

void AGoToHeavenCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	value = FMath::Clamp(value - DeltaTime, 0.0f, 200.0f);
	timelift -= DeltaTime;
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

	if (55 <= currentTime && currentTime <= 60)
	{
		setValueInLightSource(FMath::Lerp(1.0f, 0.00001f, (currentTime - 55.0f) / 5.0f));
	}

	//photo
	if (60 <= currentTime && currentTime <= 65)
	{
		AActor* object = getObjectName("light0"); 
		Cast<ARectLight>(object)->RectLightComponent->SetIntensity(FMath::Lerp(0, 82500, (currentTime - 60.0f) / 5.0f));
	}

	if (65 <= currentTime && currentTime <= 70)
	{
		AActor* object = getObjectName("light0");
		Cast<ARectLight>(object)->RectLightComponent->SetIntensity(FMath::Lerp(82500, 0, (currentTime - 65.0f) / 5.0f));
	}




	if (currentTime > 90 && !triggered[3])
	{
		AActor* object = getObjectName("light1");
		Cast<ARectLight>(object)->RectLightComponent->SetIntensity(82500);

		triggered[3] = true;
	}


	if (currentTime > 120 && !triggered[6])
	{
		AActor* object = getObjectName("light2");
		Cast<ARectLight>(object)->RectLightComponent->SetIntensity(82500);

		triggered[6] = true;
	}


	if (currentTime > 150 && !triggered[9])
	{
		AActor* object = getObjectName("light3");
		Cast<ARectLight>(object)->RectLightComponent->SetIntensity(82500);

		triggered[9] = true;
	}


	if (currentTime > 180 && !triggered[12])
	{
		AActor* object = getObjectName("light4");
		Cast<ARectLight>(object)->RectLightComponent->SetIntensity(312500);

		triggered[12] = true;
	}

	if (210 <= currentTime && currentTime <= 220)
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

void AGoToHeavenCharacter::getLightSource() 
{
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), actors);

	for (int i = 0; i < actors.Num(); i += 1)
	{
		if (!actors[i]->ActorHasTag("HighLight") && (actors[i]->GetClass()->GetName() == "RectLight" || actors[i]->GetClass()->GetName() == "PointLight" || actors[i]->GetClass()->GetName() == "SpotLight"))
		{
			lights.Add(actors[i]);
			lightIntensityRecords.Add(Cast<ALight>(actors[i])->GetBrightness());


			UE_LOG(LogTemp, Warning, TEXT("NAME = %s %f"), *actors[i]->GetName(), Cast<ALight>(actors[i])->GetBrightness());
		}
	}

	for (int i = 0; i < actors.Num(); i += 1)
	{
		if (actors[i]->GetName() == "BP_glasswindow_2" || actors[i]->GetName() == "BP_glasswindow2")
		{
			lightComponents.Add(Cast<ULightComponent>(actors[i]->GetDefaultSubobjectByName("RectLight")));
			lightIntensityRecords.Add(Cast<ULightComponent>(actors[i]->GetDefaultSubobjectByName("RectLight"))->Intensity);
		}

		if (actors[i]->GetName() == "BP_lightball_2")
		{
			lightComponents.Add(Cast<ULightComponent>(actors[i]->GetDefaultSubobjectByName("PointLight")));
			lightIntensityRecords.Add(Cast<ULightComponent>(actors[i]->GetDefaultSubobjectByName("PointLight"))->Intensity);
		}
	}

	for (int i = 0; i < actors.Num(); i += 1)
	{
		if (actors[i]->GetName() == "bartablelong_11" ||
			actors[i]->GetName() == "bartablelong2" ||
			actors[i]->GetName() == "bartablelong3" ||
			actors[i]->GetName() == "bartablelong4" ||
			actors[i]->GetName() == "bartablelong5" ||
			actors[i]->GetName() == "bartablelong6" ||
			actors[i]->GetName() == "bartablelong7")
		{
			meshes.Add(Cast<AStaticMeshActor>(actors[i])->GetStaticMeshComponent());
			lightIntensityRecords.Add(15);

		}

		if (actors[i]->GetName() == "superlight_2" ||
			actors[i]->GetName() == "superlight2" ||
			actors[i]->GetName() == "superlight3_9" ||
			actors[i]->GetName() == "superlight4_14" ||
			actors[i]->GetName() == "superlight5")
		{
			meshes.Add(Cast<AStaticMeshActor>(actors[i])->GetStaticMeshComponent());
			lightIntensityRecords.Add(150);
		}

		if (actors[i]->GetName() == "BP_glasswindow_2" || actors[i]->GetName() == "BP_glasswindow2")
		{
			meshes.Add(Cast<UStaticMeshComponent>(actors[i]->GetDefaultSubobjectByName("StaticMesh")));
			lightIntensityRecords.Add(2);
		}

		if (actors[i]->GetName() == "BP_lightball_2")
		{
			meshes.Add(Cast<UStaticMeshComponent>(actors[i]->GetDefaultSubobjectByName("Sphere")));
			lightIntensityRecords.Add(200);

			meshes.Add(Cast<UStaticMeshComponent>(actors[i]->GetDefaultSubobjectByName("Sphere1")));
			meshes.Add(Cast<UStaticMeshComponent>(actors[i]->GetDefaultSubobjectByName("Sphere2")));
			meshes.Add(Cast<UStaticMeshComponent>(actors[i]->GetDefaultSubobjectByName("Sphere3")));
			meshes.Add(Cast<UStaticMeshComponent>(actors[i]->GetDefaultSubobjectByName("Sphere4")));
			meshes.Add(Cast<UStaticMeshComponent>(actors[i]->GetDefaultSubobjectByName("Sphere5")));
			meshes.Add(Cast<UStaticMeshComponent>(actors[i]->GetDefaultSubobjectByName("Sphere6")));
			meshes.Add(Cast<UStaticMeshComponent>(actors[i]->GetDefaultSubobjectByName("Sphere7")));
			meshes.Add(Cast<UStaticMeshComponent>(actors[i]->GetDefaultSubobjectByName("Sphere8")));
			meshes.Add(Cast<UStaticMeshComponent>(actors[i]->GetDefaultSubobjectByName("Sphere9")));

			for (int j = 1; j <= 9; j += 1)
			{
				lightIntensityRecords.Add(1);
			}
		}
	}
}

void AGoToHeavenCharacter::setValueInLightSource(float a) 
{
	for (int i = 0; i < lights.Num(); i += 1)
	{
		Cast<ALight>(lights[i])->SetBrightness(a * lightIntensityRecords[i]);

		if (lights[i]->ActorHasTag("Environment")) 
		{
			Cast<ALight>(lights[i])->SetBrightness(FMath::Lerp(0.1f, 1.0f, a) * lightIntensityRecords[i]);
		}
	}

	for (int i = 0; i < lightComponents.Num(); i += 1)
	{
		Cast<ULightComponent>(lightComponents[i])->SetIntensity(a * lightIntensityRecords[lights.Num() + i]);
	}

	for (int i = 0; i < meshes.Num(); i += 1)
	{
		meshes[i]->SetScalarParameterValueOnMaterials("brightness", a * lightIntensityRecords[lights.Num() + lightComponents.Num()  + i]);
	}
}