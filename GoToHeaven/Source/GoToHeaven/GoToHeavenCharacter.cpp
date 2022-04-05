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
#include "Engine/SpotLight.h"
#include "Engine/StaticMeshActor.h"
#include "Components/RectLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Engine/Light.h"
#include "Sound/AmbientSound.h"
#include "Components/AudioComponent.h"
#include "MySaveGame.h"

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
	currentTime = 178;//53;
	value = 0;
	ui->changeWBackgroundAlpha(0);

	setValueInLightSource(0.005f);

	triggeredEvent.Add(false);

	getLightSource();

	Cast<AAmbientSound>(getObjectName("BarSound"))->Stop();
	Cast<AAmbientSound>(getObjectName("PostSound"))->Stop();
	Cast<AAmbientSound>(getObjectName("DrumSound"))->Stop();
	Cast<AAmbientSound>(getObjectName("WineSound"))->Stop();
	Cast<AAmbientSound>(getObjectName("GiftSound"))->Stop();
	Cast<AAmbientSound>(getObjectName("RingSound"))->Stop();
	Cast<AAmbientSound>(getObjectName("BoomSound"))->Stop();
}

void AGoToHeavenCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	timelift -= DeltaTime;
	Episodes(DeltaTime);

	affectVision();
}

void AGoToHeavenCharacter::affectVision()
{
	float limit = 100.0f;
	float maskLimit = 80.0f;
	
	if (value <= limit) 
	{
		((FPostProcessSettings*)GetWorld()->PostProcessVolumes[0]->GetProperties().Settings)->DepthOfFieldFocalDistance = FMath::Lerp(100.0f, 0.01f, value / limit);

		//change mask a
		if (value > limit - maskLimit)
		{
			ui->changeWBackgroundAlpha(FMath::Lerp(0.0f, 1.0f, (value - (limit - maskLimit)) / maskLimit));
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
		value -= deltaTime * 20.0f;
	}

	if (53 <= currentTime && currentTime <= 56 && !Cast<AAmbientSound>(getObjectName("BarSound"))->GetAudioComponent()->IsPlaying())
	{
		Cast<AAmbientSound>(getObjectName("BarSound"))->GetAudioComponent()->FadeIn(5);
	}

	if (65 <= currentTime && currentTime < 85)
	{
		setValueInLightSource(FMath::Lerp(1.0f, 0.005f, (currentTime - 55.0f) / 30.0f));
	}

	if (75 <= currentTime && currentTime <= 85) 
	{
		Cast<AAmbientSound>(getObjectName("BarSound"))->GetAudioComponent()->FadeOut(10, 0);
	}

	float lightStrength = 135000;

	//photo
	if (85 <= currentTime && currentTime <= 90)
	{
		AActor* object = getObjectName("light0"); 
		Cast<ARectLight>(object)->RectLightComponent->SetIntensity(FMath::Lerp(0, 135000, (currentTime - 85.0f) / 5.0f));

		UE_LOG(LogTemp, Warning, TEXT("NAME = %f"), FVector::DotProduct(object->GetActorLocation() - GetActorLocation(), GetActorForwardVector()));

		if (FVector::DotProduct(object->GetActorLocation() - GetActorLocation(), GetActorForwardVector()) > 0)
		{
			//currentTime -= deltaTime;
		}

		if (85 <= currentTime && !Cast<AAmbientSound>(getObjectName("PostSound"))->GetAudioComponent()->IsPlaying())
		{
			Cast<AAmbientSound>(getObjectName("PostSound"))->GetAudioComponent()->FadeIn(1);
		}
	}

	if (95 <= currentTime && currentTime <= 98)
	{
		AActor* object = getObjectName("light0");
		Cast<ARectLight>(object)->RectLightComponent->SetIntensity(FMath::Lerp(135000, 0, (currentTime - 95.0f) / 3.0f));

		if (97 <= currentTime)
		{
			Cast<AAmbientSound>(getObjectName("PostSound"))->GetAudioComponent()->FadeOut(1, 0);
		}
	}

	//drum
	if (100 <= currentTime && currentTime <= 105)
	{
		AActor* object = getObjectName("DrumLight");
		Cast<ASpotLight>(object)->SpotLightComponent->SetIntensity(FMath::Lerp(0, 135000, (currentTime - 100.0f) / 5.0f));

		if (100 <= currentTime && !Cast<AAmbientSound>(getObjectName("DrumSound"))->GetAudioComponent()->IsPlaying())
		{
			Cast<AAmbientSound>(getObjectName("DrumSound"))->GetAudioComponent()->FadeIn(1);
		}
	}

	if (105 <= currentTime && currentTime <= 108)
	{
		AActor* object = getObjectName("DrumLight");
		Cast<ASpotLight>(object)->SpotLightComponent->SetIntensity(FMath::Lerp(135000, 0, (currentTime - 105.0f) / 3.0f));

		if (105 <= currentTime)
		{
			Cast<AAmbientSound>(getObjectName("DrumSound"))->GetAudioComponent()->FadeOut(1, 0);
		}
	}

	//wine
	if (110 <= currentTime && currentTime <= 115)
	{
		AActor* object = getObjectName("WineLight");
		Cast<ASpotLight>(object)->SpotLightComponent->SetIntensity(FMath::Lerp(0, 135000, (currentTime - 110.0f) / 5.0f));

		if (110 <= currentTime && !Cast<AAmbientSound>(getObjectName("WineSound"))->GetAudioComponent()->IsPlaying())
		{
			Cast<AAmbientSound>(getObjectName("WineSound"))->GetAudioComponent()->FadeIn(1);
		}
	}

	if (120 <= currentTime && currentTime <= 125)
	{
		AActor* object = getObjectName("WineLight");
		Cast<ASpotLight>(object)->SpotLightComponent->SetIntensity(FMath::Lerp(135000, 0, (currentTime - 120.0f) / 5.0f));
	}

	//gift
	if (125 <= currentTime && currentTime <= 130)
	{
		AActor* object = getObjectName("GiftLight");
		Cast<ASpotLight>(object)->SpotLightComponent->SetIntensity(FMath::Lerp(0, 135000, (currentTime - 125.0f) / 5.0f));
		
		if (125 <= currentTime && !Cast<AAmbientSound>(getObjectName("GiftSound"))->GetAudioComponent()->IsPlaying())
		{
			Cast<AAmbientSound>(getObjectName("GiftSound"))->GetAudioComponent()->FadeIn(1);
		}
	}

	if (140 <= currentTime && currentTime <= 145)
	{
		AActor* object = getObjectName("GiftLight");
		Cast<ASpotLight>(object)->SpotLightComponent->SetIntensity(FMath::Lerp(135000, 0, (currentTime - 140.0f) / 5.0f));
	}

	//ring
	if (145 <= currentTime && currentTime <= 150)
	{
		AActor* object = getObjectName("RingLight");
		Cast<ASpotLight>(object)->SpotLightComponent->SetIntensity(FMath::Lerp(0, 135000, (currentTime - 145.0f) / 5.0f));

		if (145 <= currentTime && !Cast<AAmbientSound>(getObjectName("RingSound"))->GetAudioComponent()->IsPlaying())
		{
			Cast<AAmbientSound>(getObjectName("RingSound"))->GetAudioComponent()->FadeIn(1);
		}
	}

	if (170 <= currentTime && currentTime <= 175)
	{
		AActor* object = getObjectName("RingLight");
		Cast<ASpotLight>(object)->SpotLightComponent->SetIntensity(FMath::Lerp(135000, 0, (currentTime - 170.0f) / 5.0f));
	}

	//become light
	if (180 <= currentTime && currentTime <= 195)
	{
		setValueInLightSource(FMath::Lerp(0.005f, 1.0f, (currentTime - 180.0f) / 10.0f));
		
		if (!Cast<AAmbientSound>(getObjectName("BoomSound"))->GetAudioComponent()->IsPlaying())
		{
			Cast<AAmbientSound>(getObjectName("BoomSound"))->GetAudioComponent()->FadeIn(10);
		}
	}

	if (currentTime > 180 && !triggered[0])
	{
		triggered[0] = true;

		//posts became black
		TArray<AActor*> posts;
		posts.Add(getObjectName("Plane2_8"));
		posts.Add(getObjectName("Plane3_11"));
		posts.Add(getObjectName("Plane4_14"));

		for (int i = 0; i < 3; i += 1)
		{
			Cast<AStaticMeshActor>(posts[i])->GetStaticMeshComponent()->SetScalarParameterValueOnMaterials(TEXT("lightness"), FMath::Lerp(1.0f, 0.025f, (currentTime - 67.5f) / 5.0f));
		}

		//cup
		TArray<AActor*> breakCup;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), "BreakCup", breakCup);

		for (int i = 0; i < breakCup.Num(); i += 1)
		{
			breakCup[i]->SetActorLocation(FVector(breakCup[i]->GetActorLocation().X, 257, breakCup[i]->GetActorLocation().Z));
		}

		//disappear
		TArray<AActor*> toDelete;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), "Key", toDelete);

		for (int i = 0; i < toDelete.Num(); i += 1)
		{
			toDelete[i]->Destroy();
		}
	}

	if (180 < currentTime && currentTime < 185) 
	{
		FVector startPos = FVector(-1070.0f, -100.0f, 240);
		FRotator startRotation = FRotator(0, -90, 0);
		FVector endPos = FVector(-880.0f, -100.0f, 140);
		FRotator endRotation = FRotator(0, -90, -290.000519f);

		AActor* object = getObjectName("mic_2");
		object->SetActorLocation(FMath::Lerp(startPos, endPos, (currentTime - 180.0f) / 5.0f));
		object->SetActorRotation(FMath::Lerp(startRotation, endRotation, (currentTime - 180.0f) / 5.0f));
	}

	if (200 < currentTime && currentTime < 205)
	{
		value += deltaTime * 20.0f;
	}

	if (207 < currentTime) 
	{
		UMySaveGame* saveData = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot("SaveData0", 0));
		saveData->gameStatus = 1;
		UGameplayStatics::SaveGameToSlot(saveData, "SaveData0", 0);

		UGameplayStatics::OpenLevel(GetWorld(), "Subway");
	}
}

AActor* AGoToHeavenCharacter::getObjectName(FString name)
{	
	//get objects
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);

	for (AActor* Actor : Actors)
	{	
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

		if (actors[i]->GetName() == "BP_stage_2")
		{
			meshes.Add(Cast<UStaticMeshComponent>(actors[i]->GetDefaultSubobjectByName("line1")));
			meshes.Add(Cast<UStaticMeshComponent>(actors[i]->GetDefaultSubobjectByName("line2")));
			meshes.Add(Cast<UStaticMeshComponent>(actors[i]->GetDefaultSubobjectByName("line3")));
			lightIntensityRecords.Add(1);
			lightIntensityRecords.Add(1);
			lightIntensityRecords.Add(1);

			meshes.Add(Cast<UStaticMeshComponent>(actors[i]->GetDefaultSubobjectByName("ring")));
			meshes.Add(Cast<UStaticMeshComponent>(actors[i]->GetDefaultSubobjectByName("ring1")));
			meshes.Add(Cast<UStaticMeshComponent>(actors[i]->GetDefaultSubobjectByName("ring2")));
			lightIntensityRecords.Add(25);
			lightIntensityRecords.Add(25);
			lightIntensityRecords.Add(25);
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
			Cast<ALight>(lights[i])->SetBrightness(FMath::Lerp(0.18f, 1.0f, a) * lightIntensityRecords[i]);
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